#include "game.h"

/* $00646E-$006541: the shared script countdown, sound, screen flash / shake, the y line test and
 * the terrain moves of the a4 objects. The terrain ops move the box from the last drawn position
 * (+$56 / +$58) to the new one in d4 / d5 through the scene's terrain routine (g_TerrainFn) and
 * branch on any wall or floor contact. */

static void next(uint32_t *handler, const u16 **script)
{
    *handler = dk::code((int16_t)(uint16_t)*(*script)++);
    dk::jump(*handler);
}

/* Op4_BranchScriptTimerZero - a4 op: branches when the shared countdown g_ScriptTimer has run out
 * (0); otherwise continues past the offset.
 * In:  script = word branch offset
 * Out: the branch (Op4_Goto) or NEXT after the offset
 * Evidence: tst.w (g_ScriptTimer).w / beq.w Op4_Goto / addq.l #2,a4 / NEXT; script use $3700C
 *           (`646E 000C`); 29 corpus runs
 */
void Op4_BranchScriptTimerZero(uint32_t *handler, const u16 **script)
{
    if (g_ScriptTimer == 0) {
        dk::jump(at_Op4_Goto);
        return;
    }
    (*script)++;
    next(handler, script);
}

/* Op4_AddScriptTimer - a4 op: adds a word to the shared countdown g_ScriptTimer (Camera_UpdateView
 * counts it down each frame).
 * In:  script = word frames
 * Out: d0's low word = the frames; g_ScriptTimer; the add.w's condition codes; NEXT
 * Evidence: move.w (a4)+,d0 / add.w d0,(g_ScriptTimer).w / NEXT; script use $3701A (`647C 0004`);
 *           29 corpus runs
 */
void Op4_AddScriptTimer(uint16_t *d0, uint32_t *handler, const u16 **script, dk::Flags *ccr)
{
    *d0 = *(*script)++;
    uint16_t old = g_ScriptTimer;
    uint16_t sum = (uint16_t)(old + *d0);
    bool carry = sum < old;
    *ccr = dk::Flags{ carry, (sum & 0x8000) != 0, sum == 0, ((~(old ^ *d0)) & (old ^ sum) & 0x8000) != 0, carry };
    g_ScriptTimer = sum;
    next(handler, script);
}

/* Op4_PlaySound - a4 op: plays sound effect n (Snd_PlaySfx, as Op2_PlaySound).
 * In:  script = word sound number
 * Out: d0 as Snd_PlaySfx leaves it; NEXT
 * Evidence: move.w (a4)+,d0 / jsr (Snd_PlaySfx).l / NEXT; 53 script uses, e.g. $13246 (`6486
 *           0012`, sound $12 = the health pickup), $14C48 (`6486 0017`); 149 corpus runs
 */
void Op4_PlaySound(uint32_t *d0, const u16 **script)
{
    *d0 = (*d0 & 0xFFFF0000u) | (uint16_t)*(*script)++;
    Snd_PlaySfx(d0);
    uint32_t handler;
    next(&handler, script);
}

/* Op4_FlashRandom - a4 op: on a coin toss (Rand_Next negative) flashes the screen white for 4
 * frames; otherwise does nothing.
 * In:  script (no operands)
 * Out: d0 = the random number; g_FlashReq / g_FlashTimer; the condition codes of the tst or the
 *      move.b #4; NEXT (VM4_Next)
 * Evidence: bsr Rand_Next / bpl.w VM4_Next / st (g_FlashReq).w / move.b #4,(g_FlashTimer).w / NEXT;
 *           script use $20EC2; 3 corpus runs
 */
void Op4_FlashRandom(uint32_t *d0, uint32_t *handler, const u16 **script, dk::Flags *ccr)
{
    *d0 = (uint32_t)Rand_Next();
    if ((int32_t)*d0 >= 0) {
        *ccr = dk::Flags::test(*d0, 4);
        dk::jump(at_VM4_Next);
        return;
    }
    g_FlashReq = 0xFF;
    g_FlashTimer = 4;
    *ccr = dk::Flags::test(4, 1);
    next(handler, script);
}

/* Op4_ShakeRandom - a4 op: on a coin toss (Rand_Next negative) starts the standard screen shake
 * (Op4_Shake); otherwise does nothing.
 * In:  script (no operands)
 * Out: d0 = the random number; NEXT (VM4_Next), or Op4_Shake's; the condition codes of the last
 *      call (Rand_Next's tst, or Shake_Start's)
 * Evidence: bsr Rand_Next / bpl.w VM4_Next, falling into Op4_Shake; script use $20E98; 3 corpus runs
 */
void Op4_ShakeRandom(uint32_t *d0, uint32_t *handler, const u16 **script)
{
    *d0 = (uint32_t)Rand_Next();
    if ((int32_t)*d0 >= 0) {
        dk::jump(at_VM4_Next);
        return;
    }
    Op4_Shake(handler, script);
}

/* Op4_Shake - a4 op: starts the standard screen shake (Shake_Start).
 * In:  script (no operands)
 * Out: NEXT
 * Evidence: jsr (Shake_Start).l / NEXT; an entry inside Op4_ShakeRandom (computed-jump target in 7
 *           corpus runs); script uses $1AAA2 (`64B0 63A2`), $1F124, $1F214
 */
void Op4_Shake(uint32_t *handler, const u16 **script)
{
    Shake_Start();
    next(handler, script);
}

/* Op4_BranchCrossY - a4 op: line test on the drawn y: saves d4 as the drawn x and, when d5 reached
 * or crossed the line y (from either side, judged by the last drawn y), snaps d5 and the drawn y
 * to the line and branches; otherwise stores d5 as the drawn y and continues.
 * In:  script = word line y (world), word branch offset; d4 / d5 = the drawn x / y words; self = a6
 * Out: d0 = the line; self->draw_x / draw_y; d5 (snapped); the branch or NEXT after the offset
 * Evidence: move.w d4,$56(a6) / move.w (a4)+,d0 / cmp.w $58(a6),d0 / bgt / cmp.w d5,d0 / blt;
 *           script uses $1A3A2 (`64BA 00C3 0004`), $1E36C (`64BA 00B4`, after Op4_Move); 4 corpus runs
 */
void Op4_BranchCrossY(uint16_t *d0, uint16_t d4, uint16_t *d5, const u16 **script, Vm4Obj *self)
{
    self->draw_x = (int16_t)d4;
    *d0 = *(*script)++;
    int16_t line = (int16_t)*d0;
    bool crossed = line > self->draw_y ? line <= (int16_t)*d5   /* the line below the last y: reached going down */
                                       : line >= (int16_t)*d5;  /* at or above it: reached going up */
    uint32_t handler;
    if (crossed) {
        *d5 = *d0;
        self->draw_y = (int16_t)*d5;
        *script = (const u16 *)((const u8 *)*script + (int16_t)(uint16_t)**script);
        next(&handler, script);
        return;
    }
    self->draw_y = (int16_t)*d5;
    (*script)++;
    next(&handler, script);
}

/* the terrain move: d1 / d2 = the move from the last drawn point, d4 / d5 = that point (movem.w:
 * sign-extended) plus the box offset, the routine called; then d4 / d5 minus the offset are the
 * new drawn point; d7 |= d6 is the contact */
static void terrain_move(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5,
                         uint32_t *d6, uint32_t *d7, uint32_t *a0, uint32_t *a1, uint32_t *a2, uint32_t *a3,
                         uint32_t *a4, uint32_t *a5, uint32_t *a6, int16_t dx, int16_t dy)
{
    PTR(Vm4Obj) self;
    self.v = *a6;
    *d1 = (*d1 & 0xFFFF0000u) | (uint16_t)*d4;
    *d2 = (*d2 & 0xFFFF0000u) | (uint16_t)*d5;
    *d4 = (uint32_t)(int32_t)self->draw_x;
    *d5 = (uint32_t)(int32_t)self->draw_y;
    *d1 = (*d1 & 0xFFFF0000u) | (uint16_t)(*d1 - *d4);
    *d2 = (*d2 & 0xFFFF0000u) | (uint16_t)(*d2 - *d5);
    *d4 = (*d4 & 0xFFFF0000u) | (uint16_t)(*d4 + (uint16_t)dx);
    *d5 = (*d5 & 0xFFFF0000u) | (uint16_t)(*d5 + (uint16_t)dy);
    *a0 = g_TerrainFn;
    Op4TerrainFn(dk::code(*a0), d0, d1, d2, d3, d4, d5, d6, d7, a0, a1, a2, a3, a4, a5, a6);
    *d4 = (*d4 & 0xFFFF0000u) | (uint16_t)(*d4 - (uint16_t)dx);
    *d5 = (*d5 & 0xFFFF0000u) | (uint16_t)(*d5 - (uint16_t)dy);
    self.v = *a6;
    self->draw_x = (int16_t)(uint16_t)*d4;
    self->draw_y = (int16_t)(uint16_t)*d5;
    *d7 = (*d7 & 0xFFFF0000u) | (uint16_t)(*d7 | *d6);
}

/* the branch on contact (bne.w Op4_Goto), else NEXT past the offset */
static void branch_on_contact(uint32_t *d7, uint32_t *a0, uint32_t *a4)
{
    if ((uint16_t)*d7 != 0) {
        dk::jump(at_Op4_Goto);
        return;
    }
    PTR(const u16) script;
    script.v = *a4 + 2;
    *a0 = dk::code((int16_t)(uint16_t)*script);
    *a4 += 4;
    dk::jump(*a0);
}

/* Op4_TerrainMove - a4 op: moves the object's box against the scene terrain (g_TerrainFn) from its
 * last drawn point to d4 / d5, keeps the result in d4 / d5 and the drawn point, and branches on
 * any wall or floor contact.
 * In:  a4 = script: word wall mask (d6), word floor mask (d7), words box dx, dy (added for the
 *      test), word branch offset; d4 / d5 = the drawn x / y words; a6 = the object
 * Out: d4 / d5 = the corrected position; the terrain routine's registers; d7 |= d6; the branch
 *      (Op4_Goto) or NEXT after the offset
 * Evidence: move.w d4,d1 / move.w d5,d2 / move.w (a4)+,d6 / move.w (a4)+,d7 / movem.w $56(a6),d4-d5 /
 *           sub.w / add.w (a4),d4 / add.w $2(a4),d5 / movea.l (g_TerrainFn).w,a0 / jsr (a0) /
 *           sub.w (a4)+ / movem.w d4-d5,$56(a6) / or.w d6,d7 / bne.w Op4_Goto; script uses $1F102
 *           (`64E4 0001 0001 0000 0008 0018`), $26AFA; 26 corpus runs
 */
void Op4_TerrainMove(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5,
                     uint32_t *d6, uint32_t *d7, uint32_t *a0, uint32_t *a1, uint32_t *a2, uint32_t *a3,
                     uint32_t *a4, uint32_t *a5, uint32_t *a6)
{
    PTR(const u16) script;
    script.v = *a4;
    *d6 = (*d6 & 0xFFFF0000u) | (uint16_t)script[0];
    *d7 = (*d7 & 0xFFFF0000u) | (uint16_t)script[1];
    int16_t dx = (int16_t)(uint16_t)script[2];
    int16_t dy = (int16_t)(uint16_t)script[3];
    *a4 += 4;   /* at the box offsets during the call */
    terrain_move(d0, d1, d2, d3, d4, d5, d6, d7, a0, a1, a2, a3, a4, a5, a6, dx, dy);
    *a4 += 4;   /* past the offsets */
    branch_on_contact(d7, a0, a4);
}

/* Op4_TerrainMoveDefault - a4 op: Op4_TerrainMove with masks 9 / 9 and no box offset.
 * In:  a4 = script: word branch offset; d4 / d5 = the drawn x / y words; a6 = the object
 * Out: as Op4_TerrainMove (d6 = d7 = 9 going in)
 * Evidence: moveq #9,d6 / moveq #9,d7 / the Op4_TerrainMove code without the offsets; script uses
 *           $1A9B4 (`6518 0004 677E 63E2`: on contact Op4_BounceYRandom), $358B2; 5 corpus runs
 */
void Op4_TerrainMoveDefault(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5,
                            uint32_t *d6, uint32_t *d7, uint32_t *a0, uint32_t *a1, uint32_t *a2, uint32_t *a3,
                            uint32_t *a4, uint32_t *a5, uint32_t *a6)
{
    *d6 = 9;
    *d7 = 9;
    terrain_move(d0, d1, d2, d3, d4, d5, d6, d7, a0, a1, a2, a3, a4, a5, a6, 0, 0);
    branch_on_contact(d7, a0, a4);
}
