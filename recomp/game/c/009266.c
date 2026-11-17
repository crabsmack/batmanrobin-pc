#include "game.h"

/* $009266-$00932D: the gravity-arc natives of the a4 effect scripts (Arc_Plan solves an arc, Arc_Step
 * flies it one frame) and the wave-director natives (Wave_EndAll, Wave_StartScriptArg,
 * Wave_StartFromEntry, which falls into Wave_Start in the next file).
 *
 * An arcing a4 object keeps its gravity in the radius long (+$26) and flies by its x / y velocity
 * longs; +$56/+$58 (the drawn point's words) hold where the arc started. */

/* a 68000 address held in a register, as a pointer (PTR(T): the 68000 value kept) */
template <class T> static T *ptr(uint32_t a)
{
    PTR(T) p;
    p.v = a;
    return p;
}

/* divu.w: remainder:quotient, or the dividend unchanged when the quotient overflows a word */
static uint32_t divu_w(uint32_t dividend, uint16_t divisor)
{
    if (divisor == 0)
        dk::fatal("Arc: divu.w by zero (the 68000 traps)");
    uint32_t q = dividend / divisor;
    if (q > 0xFFFFu)
        return dividend;
    return (dividend % divisor) << 16 | q;
}

/* divs.w: the same, signed (both round toward zero; the remainder takes the dividend's sign) */
static uint32_t divs_w(uint32_t dividend, int16_t divisor)
{
    if (divisor == 0)
        dk::fatal("Arc: divs.w by zero (the 68000 traps)");
    int64_t n = (int32_t)dividend;
    int64_t q = n / divisor, r = n % divisor;
    if (q < -32768 || q > 32767)
        return dividend;
    return (uint32_t)(uint16_t)r << 16 | (uint16_t)q;
}

static uint32_t ext_l(uint32_t r)
{
    return (uint32_t)(int32_t)(int16_t)(uint16_t)r;
}

static uint32_t low_word(uint32_t r, uint16_t w)
{
    return (r & 0xFFFF0000u) | w;
}

/* Arc_Plan (a static helper: only this file's entries reach it, by bra.s and by falling in) -
 * solves the arc that rises `height` pixels in `frames` frames and comes down on the target: the
 * gravity, the launch speed up, and the x speed over the whole flight.
 * In:  d0/d1 = the target x/y (low words); a5 = the object; a4 = the script at dc.w height, frames
 * Out: a6 = the object; gravity (+$26) = height << 16 / (frames * (frames + 1) / 2); y velocity =
 *      -(gravity * frames + the extra speed to climb to a target above the start); x velocity =
 *      dx << 16 / (the rise and fall frames); +$56/+$58 = the start x/y; d0 = the x velocity,
 *      d1 = the fall's divu result (low word / 4), d6 = gravity * frames >> 8; a4 past the words;
 *      the condition codes of the last move.w (the start y)
 * Evidence: movem.l d2-d5,-(a7) / movea.l a5,a6 / move.w (a4)+,d2 / move.w (a4)+,d3 / d4 = (d3 + 1)
 *           asr 1, mulu d3 / d5 = d2 << 16, divu d4, ext.l / move.l d5,$26(a6) / d6 = d5 mulu d3 /
 *           move.l d6,$42(a6) / lsr.l #8,d6 / sub.w $32(a6),d1 / bpl ... / neg.l $42(a6) / ... /
 *           divs.w d3,d0 / ... / move.l d0,$3E(a6) / movem.l (a7)+,d2-d5. The C keeps d2-d5 in
 *           locals (the movem restores them; the function calls nothing, so no stack slot is read)
 */
static void arc_plan(uint32_t *d0, uint32_t *d1, uint32_t *d6, uint32_t *a4, uint32_t a5, uint32_t *a6,
                     dk::Flags *ccr)
{
    *a6 = a5;
    Vm4Obj *self = ptr<Vm4Obj>(a5);
    const u16 *args = ptr<const u16>(*a4);
    uint16_t height = args[0], frames = args[1];
    *a4 += 4;

    /* the rise: gravity g with frames * (frames + 1) / 2 * g = height */
    uint16_t half = (uint16_t)((int16_t)(uint16_t)(frames + 1) >> 1);
    uint32_t steps = (uint32_t)half * frames;
    uint32_t gravity = ext_l(divu_w((uint32_t)height << 16, (uint16_t)steps));
    vm4_set_fix(self->radius, self->radius_frac, gravity);
    uint32_t launch = (uint32_t)(uint16_t)gravity * frames;
    self->vel_y = (int32_t)launch;
    *d6 = launch >> 8;

    /* the fall to the target's y: a target below the start (dy >= 0) adds fall frames; one above
       it adds launch speed instead */
    uint16_t dy = (uint16_t)((uint16_t)*d1 - (uint16_t)self->y);
    if ((int16_t)dy >= 0) {
        *d1 = divu_w(ext_l(dy) << 8, (uint16_t)*d6);
    } else {
        *d1 = divu_w(ext_l((uint16_t)-dy) << 8, (uint16_t)*d6);
        uint32_t extra = (uint32_t)(uint16_t)*d1 * (uint16_t)gravity;
        self->vel_y = (int32_t)((uint32_t)self->vel_y + extra);
    }
    self->vel_y = (int32_t)(0u - (uint32_t)self->vel_y);   /* up */

    /* the flight's frames: 2 * frames + the fall's - a quarter of it */
    uint16_t flight = (uint16_t)(frames * 2 + (uint16_t)*d1);
    *d1 = low_word(*d1, (uint16_t)((int16_t)(uint16_t)*d1 >> 2));
    flight = (uint16_t)(flight - (uint16_t)*d1);
    uint16_t dx = (uint16_t)((uint16_t)*d0 - (uint16_t)self->x);
    *d0 = ext_l(divs_w(ext_l(dx) << 8, (int16_t)flight)) << 8;
    self->vel_x = (int32_t)*d0;

    self->draw_x = self->x;
    self->draw_y = self->y;
    *ccr = dk::Flags::test((uint16_t)self->y, 2, ccr->x);
}

/* Arc_PlanToArgs - a4 native: plans a gravity arc for the running a4 object to the target point the
 * script gives.
 * In:  a5 = the object; a4 = the script: dc.w target x, y, rise height, rise frames
 * Out: d0/d1 low words = the target, then Arc_Plan's outputs; a4 past the four words
 * Evidence: move.w (a4)+,d0 / move.w (a4)+,d1 / bra.s Arc_Plan; 58 script uses after the a4
 *           native-call op $5438 ($2790E...), each followed by Arc_Step calls. Prototype: &ccr
 *           added (Arc_Plan's last move.w, its ccout)
 */
void Arc_PlanToArgs(uint32_t *d0, uint32_t *d1, uint32_t *d6, uint32_t *a4, uint32_t a5, uint32_t *a6, dk::Flags *ccr)
{
    const u16 *args = ptr<const u16>(*a4);
    *d0 = low_word(*d0, args[0]);
    *d1 = low_word(*d1, args[1]);
    *a4 += 4;
    arc_plan(d0, d1, d6, a4, a5, a6, ccr);
}

/* Arc_PlanToMark - a4 native: plans a gravity arc for the running a4 object to the point
 * g_ArcTargetX/Y (stored by the a2 op $5378).
 * In:  a5 = the object; a4 = the script: dc.w rise height, rise frames
 * Out: d0/d1 low words = the mark, then Arc_Plan's outputs
 * Evidence: move.w g_ArcTargetX,d0 / move.w g_ArcTargetY,d1, falling into Arc_Plan; script uses at
 *           $1AB84, $1B528, $1F0EA, $42CF8. Prototype: &ccr added, as Arc_PlanToArgs
 */
void Arc_PlanToMark(uint32_t *d0, uint32_t *d1, uint32_t *d6, uint32_t *a4, uint32_t a5, uint32_t *a6, dk::Flags *ccr)
{
    *d0 = low_word(*d0, g_ArcTargetX);
    *d1 = low_word(*d1, g_ArcTargetY);
    arc_plan(d0, d1, d6, a4, a5, a6, ccr);
}

/* Arc_Step - a4 native: flies the object one frame along its arc.
 * In:  a6 = the object
 * Out: x += x velocity; y velocity += gravity; y += y velocity; d4/d5 = the x velocity / new y
 *      velocity longs with the new x / y in their low words; the condition codes of the last
 *      move.w (y)
 * Evidence: move.l $3E(a6),d4 / add.l d4,$2E(a6) / move.w $2E(a6),d4 / move.l $26(a6),d5 /
 *           add.l $42(a6),d5 / move.l d5,$42(a6) / add.l d5,$32(a6) / move.w $32(a6),d5; script
 *           uses after op $5438 at $1AB98, $1B53C, $26AF6, $3DA0A, $42D10. Prototype: &ccr added
 */
void Arc_Step(uint32_t *d4, uint32_t *d5, uint32_t a6, dk::Flags *ccr)
{
    Vm4Obj *self = ptr<Vm4Obj>(a6);
    *d4 = (uint32_t)self->vel_x;
    vm4_add_fix(self->x, self->x_frac, *d4);
    *d4 = low_word(*d4, (uint16_t)self->x);
    *d5 = vm4_fix(self->radius, self->radius_frac) + (uint32_t)self->vel_y;
    self->vel_y = (int32_t)*d5;
    vm4_add_fix(self->y, self->y_frac, *d5);
    *d5 = low_word(*d5, (uint16_t)self->y);
    *ccr = dk::Flags::test((uint16_t)self->y, 2, ccr->x);
}

/* Wave_EndAll - native callback: starts a new wave generation, so every running wave director
 * ends on its next frame (its serial is then below g_WaveGen).
 * In:  none
 * Out: g_WaveSerial + 1, g_WaveGen = it; d0 = 0 (moveq: tst(d0.l)'s flags)
 * Evidence: addq.w #1,g_WaveSerial / move.w g_WaveSerial,g_WaveGen / moveq #0,d0; the wave handler
 *           $6A14 ends objects whose serial ($A) is below g_WaveGen; script uses after
 *           Op2_CallNative ($1C824, $1CFA2, $20CEE, $3B2EC)
 */
uint32_t Wave_EndAll(void)
{
    g_WaveSerial++;
    g_WaveGen = g_WaveSerial;
    return 0;
}

/* Wave_StartScriptArg - native callback: starts a wave director running the a5 script the script's
 * dc.l names.
 * In:  a2 = the script at the dc.l; d4/d5 = the position; a1-a6 saved around the call
 * Out: a2 past the dc.l; d0 = Wave_Start's (0, or -1 when the pool is short) and its flags
 * Evidence: movea.l (a2)+,a0 / movem.l a1-a6,-(a7) / movea.l a0,a5 / bsr.w Wave_Start /
 *           movem.l (a7)+,a1-a6 / rts; 38 script uses after Op2_CallNative ($1C7E6...).
 *           Prototype: a1, a3-a5 inputs (the movem pushes them)
 */
void Wave_StartScriptArg(uint32_t *d0, uint16_t d4, uint16_t d5, uint32_t a1, uint32_t *a2, uint32_t a3, uint32_t a4,
                         uint32_t a5, uint32_t a6)
{
    uint32_t script = *ptr<const u32>(*a2);
    *a2 += 4;
    dk::push_long(a6);   /* movem.l a1-a6,-(a7) */
    dk::push_long(a5);
    dk::push_long(a4);
    dk::push_long(a3);
    dk::push_long(*a2);
    dk::push_long(a1);
    uint32_t r0 = script, r1 = a1, r6 = a6;
    Wave_Start(d0, d4, d5, &r0, &r1, script, &r6);
    for (int i = 0; i < 6; i++)   /* movem.l (a7)+,a1-a6 */
        (void)dk::pop_long();
}

/* Wave_StartFromEntry - level trigger callback: starts a wave director running the a5 script the
 * trigger entry names (Wave_Start, which it falls into).
 * In:  a1 = the trigger entry at its script pointer; d4/d5 = the position
 * Out: a5 = the script; then Wave_Start's
 * Evidence: movea.l (a1),a5, falling into Wave_Start (the next file); 120 uses in the stage trigger
 *           tables (`dc.l Wave_StartFromEntry, script`, read at pc $932C)
 */
void Wave_StartFromEntry(uint16_t d4, uint16_t d5, uint32_t *a0, uint32_t *a1, uint32_t *a5, uint32_t *a6)
{
    (void)d4, (void)d5, (void)a0, (void)a6;
    *a5 = *ptr<const u32>(*a1);
    dk::jump(at_Wave_Start);
}
