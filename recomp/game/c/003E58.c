#include "game.h"

/* $003E58-$003F4F: Op2_RunAnim (the a2 op that steps a sprite block's animation script) and the
 * animation-script ops, plus the two speed-band switches that share the frame mapping. An
 * animation script is a second a2 script: Op2_RunAnim pushes the object script's a2, calls the
 * animation script's first op (`jsr (a0)`), and the ops chain by NEXT until one returns (rts):
 * Op2_AnimFrame, Op2_AnimStop, Op2_AnimYield, Op2_AnimHoldByVelY. */

/* the 68000's divs.w: quotient in the low word, remainder (the dividend's sign) in the high;
 * a quotient outside a signed word leaves the dividend (overflow) */
static uint32_t divs_w(uint32_t dividend, uint16_t divisor)
{
    int64_t n = (int32_t)dividend, d = (int16_t)divisor;
    if (d == 0)
        return dividend;                    /* the zero-divide trap: never taken by the game */
    int64_t q = n / d, r = n % d;
    if (q < -32768 || q > 32767)
        return dividend;
    return ((uint32_t)(uint16_t)r << 16) | (uint16_t)q;
}

/* the 68000's divu.w: as divs_w, unsigned (overflow above $FFFF) */
static uint32_t divu_w(uint32_t dividend, uint16_t divisor)
{
    if (divisor == 0)
        return dividend;
    uint32_t q = dividend / divisor, r = dividend % divisor;
    if (q > 0xFFFFu)
        return dividend;
    return (r << 16) | q;
}

/* the script address an offset word names: the word's own address plus its value */
static const u16 *rel_target(const u16 *word)
{
    return (const u16 *)((const u8 *)word + (int16_t)(uint16_t)*word);
}

/* NEXT: the handler the script's next word names, jumped to */
static uint32_t op_next(const u16 **script)
{
    uint32_t handler = dk::code((int16_t)(uint16_t)*(*script)++);
    dk::jump(handler);
    return handler;
}

/* the same on a script held in a2 as a register value */
static void op_next_reg(uint32_t *a0, uint32_t *a2)
{
    PTR(const u16) script;
    script.v = *a2;
    *a0 = dk::code((int16_t)(uint16_t)*script.get());
    *a2 += 2;
    dk::jump(*a0);
}

/* Op2_RunAnim - a2 op: steps the current sprite block's animation: the frame timer counts down by
 * the object's speed and, when it runs out, the animation script runs from where it stopped
 * (its ops chained by NEXT until one returns) and the new stop is saved.
 * In:  a4 = the VM block (speed), a5 = the sprite block (anim_timer, anim); a2 = the object script
 * Out: d0 low word = the speed; the animation ops' registers; spr->anim; NEXT in the object script
 *      (VM2_Next while the timer runs)
 * Evidence: move.w $2E(a4),d0 / sub.w d0,$1E(a5) / bgt.w VM2_Next / move.l a2,-(a7) /
 *           movea.l $2E(a5),a2 / movea.w (a2)+,a0 / jsr (a0) / move.l a2,$2E(a5) /
 *           movea.l (a7)+,a2 / NEXT; every object loop (127k TAS frames, all 160 runs)
 */
void Op2_RunAnim(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6,
                 uint32_t *d7, uint32_t *a0, uint32_t *a1, uint32_t *a2, uint32_t *a3, uint32_t *a4, uint32_t *a5,
                 uint32_t *a6)
{
    PTR(const Vm2Block) vm;
    vm.v = *a4;
    PTR(Vm2Sprite) spr;
    spr.v = *a5;
    uint16_t speed = vm->speed;
    *d0 = (*d0 & 0xFFFF0000u) | speed;
    int16_t timer = (int16_t)spr->anim_timer;
    spr->anim_timer = (uint16_t)(timer - speed);
    if (timer > (int16_t)speed) {           /* bgt: still running */
        dk::jump(at_VM2_Next);
        return;
    }
    dk::push_long(*a2);
    *a2 = spr->anim.raw();
    PTR(const u16) anim;
    anim.v = *a2;
    *a0 = dk::code((int16_t)(uint16_t)*anim.get());
    *a2 += 2;
    AnimOpFn(*a0, d0, d1, d2, d3, d4, d5, d6, d7, a0, a1, a2, a3, a4, a5, a6);
    PTR(Vm2Sprite) after;                   /* a5 as the animation left it */
    after.v = *a5;
    after->anim.v = *a2;
    *a2 = dk::pop_long();
    op_next_reg(a0, a2);
}

/* Op2_AnimGoto - animation op: a relative jump that keeps running animation ops (Op2_Goto's code,
 * used to loop animation scripts).
 * In:  script = word offset (from the word itself)
 * Out: NEXT at the target
 * Evidence: adda.w (a2),a2 / NEXT; 152 corpus runs; $1B3BA's script ends `3E78 FFEA`
 */
void Op2_AnimGoto(const u16 **script)
{
    *script = rel_target(*script);
    op_next(script);
}

/* Op2_AnimStop - animation op: ends the animation: the script stays on this op for good and the
 * done flag Op2_BranchAnimFlag tests is set.
 * In:  script = past this op's word; spr
 * Out: script = this op again; spr->flags | SPR_ANIM_DONE; ccr Z = the bit was clear (bset);
 *      return to Op2_RunAnim
 * Evidence: subq.l #2,a2 / bset #0,$18(a5) / rts; 152 corpus runs, 195 script uses (e.g. $19AB0 `3E7E`)
 */
void Op2_AnimStop(const u16 **script, Vm2Sprite *spr, dk::Flags *ccr)
{
    (*script)--;
    ccr->z = (spr->flags & SPR_ANIM_DONE) == 0;
    spr->flags = (uint8_t)(spr->flags | SPR_ANIM_DONE);
}

/* Op2_SetAnimLoop - animation op: sets the animation loop counter.
 * In:  script = word count
 * Out: spr->anim_loop; NEXT
 * Evidence: move.w (a2)+,$20(a5) / NEXT; counted down by Op2_AnimLoop; 34 corpus runs; $27430 `3E88 0007`
 */
void Op2_SetAnimLoop(const u16 **script, Vm2Sprite *spr)
{
    spr->anim_loop = *(*script)++;
    op_next(script);
}

/* Op2_SetAnimLoopRand - animation op: the loop counter = base + Rand_Below(range).
 * In:  script = word base, word range
 * Out: base (d1) = the base; rand (d0) low word = the count (its upper word Rand_Below's);
 *      spr->anim_loop; NEXT
 * Evidence: move.w (a2)+,d1 / move.w (a2)+,d0 / bsr.w Rand_Below / add.w d1,d0 /
 *           move.w d0,$20(a5) / NEXT; 5 corpus runs; $1BC98 `3E90 0001 0001`
 */
void Op2_SetAnimLoopRand(uint32_t *rand, uint16_t *base, const u16 **script, Vm2Sprite *spr)
{
    *base = *(*script)++;
    *rand = (*rand & 0xFFFF0000u) | *(*script)++;
    Rand_Below(rand);
    *rand = (*rand & 0xFFFF0000u) | (uint16_t)(*rand + *base);
    spr->anim_loop = (uint16_t)*rand;
    op_next(script);
}

/* Op2_AnimLoop - animation op: counts the loop counter down and jumps back (Op2_Goto) while it
 * stays above 0.
 * In:  script = word offset; spr->anim_loop
 * Out: spr->anim_loop - 1; the branch, else handler = NEXT's after the offset
 * Evidence: subq.w #1,$20(a5) / bgt.w Op2_Goto / addq.l #2,a2 / NEXT; 40 corpus runs; $1BCB2 `3EA2 FFEA`
 */
void Op2_AnimLoop(uint32_t *handler, const u16 **script, Vm2Sprite *spr)
{
    int16_t loop = (int16_t)spr->anim_loop;
    spr->anim_loop = (uint16_t)(loop - 1);
    if (loop > 1) {                         /* bgt on loop - 1 */
        dk::jump(at_Op2_Goto);
        return;
    }
    (*script)++;
    *handler = op_next(script);
}

/* Op2_AnimGotoYield - animation op: jumps to a relative target and ends this step with the timer
 * cleared (Op2_AnimYield, which it falls into), so the animation resumes there on the next run.
 * In:  script = word offset (from the word itself)
 * Out: script = the target; spr->anim_timer = 0; return to Op2_RunAnim
 * Evidence: adda.w (a2),a2, falls into $3EB2; 3 corpus runs; $19D58 `3EB0 FFEA`
 */
void Op2_AnimGotoYield(const u16 **script, Vm2Sprite *spr, dk::Flags *ccr)
{
    *script = rel_target(*script);
    Op2_AnimYield(spr, ccr);
}

/* Op2_AnimYield - animation op: ends this animation step at once with the timer cleared (the
 * animation resumes at the next op on the next run).
 * In:  spr
 * Out: spr->anim_timer = 0; ccr = clr.w's (Z); return to Op2_RunAnim
 * Evidence: clr.w $1E(a5) / rts; 14 corpus runs; $250A0 `3EB2 3EB8 0449`
 */
void Op2_AnimYield(Vm2Sprite *spr, dk::Flags *ccr)
{
    spr->anim_timer = 0;
    *ccr = dk::Flags::test(0, 2, ccr->x);
}

/* Op2_SetAnimDelay - animation op: sets the per-frame display time (1/256 timer units: $100 = one
 * run at speed $100) and clears the timer.
 * In:  script = word delay
 * Out: spr->anim_delay, spr->anim_timer = 0; handler = NEXT's
 * Evidence: move.w (a2)+,$1C(a5) / clr.w $1E(a5) / NEXT; Op2_AnimFrame adds $1C to the timer
 *           Op2_RunAnim counts down; 152 runs; $1B3BA `3EB8 0A00`
 */
void Op2_SetAnimDelay(uint32_t *handler, const u16 **script, Vm2Sprite *spr)
{
    spr->anim_delay = *(*script)++;
    spr->anim_timer = 0;
    *handler = op_next(script);
}

/* Op2_SetAnimDelayRand - animation op: the display time = base + Rand_Signed(spread), the timer
 * cleared.
 * In:  script = word spread, word base
 * Out: rand (d0) low word = the delay (its upper word Rand_Signed's); spr->anim_delay,
 *      spr->anim_timer = 0; NEXT
 * Evidence: move.w (a2)+,d0 / bsr.w Rand_Signed / add.w (a2)+,d0 / move.w d0,$1C(a5) /
 *           clr.w $1E(a5) / NEXT; 14 corpus runs; $2624A `3EC4 0300 0400`
 */
void Op2_SetAnimDelayRand(uint32_t *rand, const u16 **script, Vm2Sprite *spr)
{
    *rand = (*rand & 0xFFFF0000u) | *(*script)++;
    Rand_Signed(rand);
    *rand = (*rand & 0xFFFF0000u) | (uint16_t)(*rand + *(*script)++);
    spr->anim_delay = (uint16_t)*rand;
    spr->anim_timer = 0;
    op_next(script);
}

/* Op2_AddAnimDelay - animation op: adds a signed word to the display time (speeds the animation
 * up or slows it down).
 * In:  script = word delta
 * Out: spr->anim_delay; NEXT
 * Evidence: move.w (a2)+,d0 / add.w d0,$1C(a5) / NEXT; 50 corpus runs; $28524 `3ED8 FE00 3EA2 FFE6`
 */
void Op2_AddAnimDelay(const u16 **script, Vm2Sprite *spr)
{
    spr->anim_delay = (uint16_t)(spr->anim_delay + *(*script)++);
    op_next(script);
}

/* Op2_AnimHoldByVelY - animation op: every run picks a case by the vertical speed band
 * ((vel_y + accel_y + frame_bias) / frame_div, unsigned; a negative sum takes the first case),
 * runs that case's one op (a frame) and stays on this op; once the speed is above every threshold
 * it falls through to the op after the table.
 * In:  script (a2) = word n, then n pairs (word threshold, word offset); a3 = the motion block;
 *      a5 = the sprite block
 * Out: a case: its op run (`jsr`), spr->anim_timer = 0, a2 = this op again, return to
 *      Op2_RunAnim; none: d1 = $FFFF, NEXT after the table
 * Evidence: pea.l -$2(a2) / move.l $1C(a3),d0 / add.l $28(a3),d0 / add.l $32(a5),d0 / bmi /
 *           divu.w $36(a5),d0 / cmp.w (a2)+,d0 / ble -> adda.w (a2),a2 / jsr (a0) /
 *           clr.w $1E(a5) / movea.l (a7)+,a2 / rts; 89 corpus runs; $1B4A6 `3EE2 0001 0028 000E`
 */
void Op2_AnimHoldByVelY(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t d3, uint32_t d4, uint32_t d5,
                        uint32_t d6, uint32_t *d7, uint32_t *a0, uint32_t *a1, uint32_t *a2, uint32_t a3,
                        uint32_t a4, uint32_t a5, uint32_t a6, dk::Flags *ccr)
{
    PTR(const Vm2Motion) mot;
    mot.v = a3;
    PTR(Vm2Sprite) spr;
    spr.v = a5;
    dk::push_long(*a2 - 2);                 /* this op: where the animation stays */
    PTR(const u16) table;
    table.v = *a2;
    const u16 *script = table;
    *d0 = (uint32_t)mot->vel_y + (uint32_t)mot->accel_y + (uint32_t)spr->frame_bias;
    bool found = true;
    if ((int32_t)*d0 < 0) {
        script += 2;                        /* the first case's offset */
    } else {
        uint32_t dividend = *d0;
        *d0 = divu_w(dividend, spr->frame_div);
        /* divu.w's condition codes: the quotient's (V on overflow, d0 kept) */
        bool overflow = *d0 == dividend && dividend / spr->frame_div > 0xFFFFu;
        *ccr = dk::Flags{ ccr->x, overflow || (*d0 & 0x8000) != 0, !overflow && (uint16_t)*d0 == 0, overflow, false };
        *d1 = (*d1 & 0xFFFF0000u) | *script++;
        found = false;
        for (;;) {
            *d1 = (*d1 & 0xFFFF0000u) | (uint16_t)(*d1 - 1);
            if ((uint16_t)*d1 == 0xFFFF)
                break;
            uint16_t threshold = *script++;
            *ccr = dk::Flags::compare(*d0, threshold, 2, ccr->x);
            if ((int16_t)*d0 <= (int16_t)threshold) {
                found = true;
                break;
            }
            script++;
        }
    }
    if (!found) {
        dk::pop_long();                     /* addq.l #4,a7: the pushed op */
        *a2 = ADDR68(script);
        op_next_reg(a0, a2);
        return;
    }
    script = rel_target(script);
    *a0 = dk::code((int16_t)(uint16_t)*script++);
    *a2 = ADDR68(script);
    AnimOpFn(*a0, d0, d1, d2, &d3, &d4, &d5, &d6, d7, a0, a1, a2, &a3, &a4, &a5, &a6);
    spr.v = a5;
    spr->anim_timer = 0;
    *ccr = dk::Flags::test(0, 2, ccr->x);   /* clr.w $1E(a5) */
    *a2 = dk::pop_long();
}

/* the band search of the two velocity switches: (velocity + frame_bias) / frame_div (signed)
 * against ascending thresholds, the last case the default */
static void switch_band(uint32_t *d0, uint16_t *d1, const u16 **script, const Vm2Sprite *spr)
{
    *d0 = divs_w(*d0 + (uint32_t)spr->frame_bias, spr->frame_div);
    *d1 = (uint16_t)(*(*script)++ - 2);
    bool found = false;
    if ((int16_t)*d1 >= 0) {
        for (;;) {
            if ((int16_t)*d0 <= (int16_t)*(*script)++) {
                found = true;
                break;
            }
            (*script)++;
            *d1 = (uint16_t)(*d1 - 1);
            if (*d1 == 0xFFFF)
                break;
        }
    }
    if (!found)
        (*script)++;                        /* the last (unused) threshold */
    *script = rel_target(*script);
    op_next(script);
}

/* Op2_SwitchVelXBand - a2 op: branches by the horizontal speed band: (vel_x + frame_bias) /
 * frame_div against ascending thresholds; the last case is the default.
 * In:  script = word n, then n pairs (word threshold, word offset), the last threshold unused;
 *      mot->vel_x; spr->frame_bias / frame_div
 * Out: d0 = the divs.w result; d1 = the dbra counter; NEXT at the first case whose threshold >= the band
 * Evidence: move.l $18(a3),d0 / add.l $32(a5),d0 / divs.w $36(a5),d0 / move.w (a2)+,d1 /
 *           subq.w #2,d1 / bmi / cmp.w (a2)+,d0 / ble; 25 corpus runs; $2819C `3F1C 0004 000A 0058 ...`
 */
void Op2_SwitchVelXBand(uint32_t *d0, uint16_t *d1, const u16 **script, const Vm2Motion *mot, const Vm2Sprite *spr)
{
    *d0 = (uint32_t)mot->vel_x;
    switch_band(d0, d1, script, spr);
}

/* Op2_SwitchVelYBand - a2 op: Op2_SwitchVelXBand on the vertical velocity.
 * In:  script = word n, then n pairs (word threshold, word offset); mot->vel_y
 * Out: as Op2_SwitchVelXBand
 * Evidence: move.l $1C(a3),d0 then the shared band search $3F26; 54 corpus runs; $28078 `3F22 0005 FFC4 004A ...`
 */
void Op2_SwitchVelYBand(uint32_t *d0, uint16_t *d1, const u16 **script, const Vm2Motion *mot, const Vm2Sprite *spr)
{
    *d0 = (uint32_t)mot->vel_y;
    switch_band(d0, d1, script, spr);
}
