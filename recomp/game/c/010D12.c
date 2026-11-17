#include "game.h"

/* The skid-effect natives' spawn tests and spawn ($010D12-$010D99). */

static constexpr uint32_t Scr4_010D9A = 0x010D9Au;   /* the skid effect, moving right */
static constexpr uint32_t Scr4_010DAC = 0x010DACu;   /* mirrored, moving left */

/* Fx_SpawnSkidEffect - native op: while the mover touches terrain, Fx_SkidFrameTest.
 * In:  mot = the motion block (a3: +$54 contact, +$24 target speed, +$18 velocity, +$2 impulse);
 *      script = the operands (a2: dx, dy, jitter range); a6 = the object
 * Out: as Fx_SkidFrameTest; off terrain: d1 low word = 0 and the andi's flags, the operands skipped
 * Evidence: move.w $54(a3),d1 / andi.w #$B,d1 / beq.s Fx_SkipSkidArgs; native callback (a2 op
 *           $2CBC) from 9 scripts incl. the players' ($4AD3E, $4D87A)
 */
void Fx_SpawnSkidEffect(uint32_t *d0, uint32_t *d1, uint32_t d2, uint32_t d3, uint32_t *d4, uint32_t *d5,
                        uint32_t d6, uint32_t d7, uint32_t a1, uint32_t *a2, const Vm2Motion *mot, uint32_t a4,
                        uint32_t a5, uint32_t a6, dk::Flags *ccr)
{
    uint16_t contact = mot->contact & 0xB;
    *d1 = (*d1 & 0xFFFF0000u) | contact;
    *ccr = dk::Flags::test(contact, 2, ccr->x);
    if (ccr->z) {
        dk::jump(at_Fx_SkipSkidArgs);
        return;
    }
    dk::jump(at_Fx_SkidFrameTest);   /* the same file's next entry: a jump, so what its spawn leaves reaches the caller */
}

/* Fx_SkidFrameTest - Fx_SpawnSkidEffect past the contact test: on every 4th frame, spawns when the
 * mover skids: with no target speed when |velocity + impulse| > 1.25, with a positive target when
 * it is below -1.5 (moving left against it), with a negative target when it is at least 1.5.
 * In:  as Fx_SpawnSkidEffect
 * Out: d1 = the frame counter test, then velocity + impulse; the flags of the last test
 * Evidence: move.l (g_FrameCounter).w,d1 / andi.b #3,d1 / bne Fx_SkipSkidArgs / tst.l $24(a3) /
 *           bgt / blt / the cmpi.l against $14000 / $FFFEC000, $FFFE8000, $18000 (blt.w); a
 *           generated-code function after the beq.s (the recompiler splits there)
 */
void Fx_SkidFrameTest(uint32_t *d0, uint32_t *d1, uint32_t d2, uint32_t d3, uint32_t *d4, uint32_t *d5,
                      uint32_t d6, uint32_t d7, uint32_t a1, uint32_t *a2, const Vm2Motion *mot, uint32_t a4,
                      uint32_t a5, uint32_t a6, dk::Flags *ccr)
{
    (void)d0, (void)d2, (void)d3, (void)d4, (void)d5, (void)d6, (void)d7, (void)a1, (void)a4, (void)a5, (void)a6;
    uint32_t fc = g_FrameCounter;
    *d1 = (fc & 0xFFFFFF00u) | (fc & 3u);
    *ccr = dk::Flags::test(*d1, 1, ccr->x);
    if (!ccr->z) {
        dk::jump(at_Fx_SkipSkidArgs);
        return;
    }
    int32_t target = mot->target_x;
    int32_t v = (int32_t)((uint32_t)(int32_t)mot->vel_x + (uint32_t)(int32_t)mot->impulse_x);
    *d1 = (uint32_t)v;
    bool spawn;
    if (target > 0) {
        *ccr = dk::Flags::compare((uint32_t)v, 0xFFFE8000u, 4, ccr->x);
        spawn = ccr->n != ccr->v;                          /* blt -1.5 */
    } else if (target < 0) {
        *ccr = dk::Flags::compare((uint32_t)v, 0x18000u, 4, ccr->x);
        spawn = ccr->n == ccr->v;                          /* not blt 1.5 */
    } else {
        *ccr = dk::Flags::compare((uint32_t)v, 0x14000u, 4, ccr->x);
        spawn = !ccr->z && ccr->n == ccr->v;               /* bgt 1.25 */
        if (!spawn) {
            *ccr = dk::Flags::compare((uint32_t)v, 0xFFFEC000u, 4, ccr->x);
            spawn = ccr->n != ccr->v;                      /* blt -1.25 */
        }
    }
    if (spawn) {
        dk::jump(at_Fx_SpawnSkidAtArgs);
        return;
    }
    dk::jump(at_Fx_SkipSkidArgs);
}

/* Fx_SpawnSkidAtArgs - spawns the skid effect at the operands' offset from the object: dx plus a
 * random jitter in +-range (mirrored, with the left-moving script, when d1 is negative), dy, as
 * the 16.16 position's integer parts, then Fx_RunScript.
 * In:  script = the operands (a2: dx, dy, jitter range); d1 = the velocity (its sign picks the
 *      side); a6 = the object (x +$3E, y +$42)
 * Out: a2 past the operands; d4/d5 = the spawn position (high words); a0 = the effect script;
 *      d0 = Rand_Signed's
 * Evidence: move.w (a2)+,d4 / move.w (a2)+,d5 / move.w (a2)+,d0 / bsr Rand_Signed / add.w d0,d4 / lea
 *           Scr4_010D9A,a0 / tst.l d1 / bpl / lea Scr4_010DAC,a0 / neg.w d4 / add.w $3E(a6),d4 /
 *           add.w $42(a6),d5 / swap d4 / swap d5 / jmp Fx_RunScript
 */
void Fx_SpawnSkidAtArgs(uint32_t *d0, uint32_t d1, uint32_t d2, uint32_t d3, uint32_t *d4, uint32_t *d5, uint32_t d6,
                        uint32_t d7, uint32_t *a0, uint32_t a1, uint32_t *a2, uint32_t a3, uint32_t a4, uint32_t a5,
                        uint32_t a6)
{
    (void)d2, (void)d3, (void)d6, (void)d7, (void)a1, (void)a3, (void)a4, (void)a5;
    PTR(const u16) ops;
    ops.v = *a2;
    uint16_t dx = *ops++;
    uint16_t dy = *ops++;
    uint16_t range = *ops++;
    *a2 = ops.raw();
    *d4 = (*d4 & 0xFFFF0000u) | dx;
    *d5 = (*d5 & 0xFFFF0000u) | dy;
    *d0 = (*d0 & 0xFFFF0000u) | range;
    Rand_Signed(d0);
    uint16_t x = (uint16_t)(dx + (uint16_t)*d0);
    *a0 = Scr4_010D9A;
    if ((int32_t)d1 < 0) {
        *a0 = Scr4_010DAC;
        x = (uint16_t)-x;
    }
    PTR(const Vm2Obj) self;
    self.v = a6;
    x = (uint16_t)(x + (uint16_t)self->x);
    uint16_t y = (uint16_t)(dy + (uint16_t)self->y);
    *d4 = ((uint32_t)x << 16) | (*d4 >> 16);
    *d5 = ((uint32_t)y << 16) | (*d5 >> 16);
    dk::jump(at_Fx_RunScript);
}
