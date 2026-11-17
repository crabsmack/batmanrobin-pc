#include "game.h"

/* Op2_RunSpawnAction, the speed ops and the sprite attribute / depth ops ($000ACC-$000B5D). */

/* NEXT: the handler the script's next word names, jumped to */
static void next(uint32_t *handler, const u16 **script)
{
    *handler = dk::code((int16_t)(uint16_t)*(*script)++);
    dk::jump(*handler);
}

/* the sprite attributes' high byte (the flip / palette / priority bits the byte ops touch) */
static uint8_t attr_hi(const Vm2Sprite *spr) { return (uint8_t)(spr->attr >> 8); }

/* Op2_RunSpawnAction - a2 op: when the object has a spawn action, clears it and runs that entry
 * of Tbl_012EF0 (SpawnAction_Run) with d1 = its argument; then NEXT. No action: NEXT at once.
 * In:  self->spawn_action / spawn_arg; every register passes through SpawnAction_Run
 * Out: d0's low byte = the action; d1's low word = the argument; the rest as SpawnAction_Run
 *      leaves them; NEXT
 * Evidence: move.b $19(a6),d0 / beq.w VM2_Next / clr.b $19(a6) / move.w $1A(a6),d1 /
 *           jsr SpawnAction_Run; also the tail of Op2_AwardScoreToHitter / ToPlayers; 1.7k TAS frames
 */
void Op2_RunSpawnAction(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5,
                        uint32_t *d6, uint32_t *d7, uint32_t *a0, uint32_t *a1, const u16 **script, uint32_t *a3,
                        uint32_t *a4, uint32_t *a5, Vm2Obj **self)
{
    uint8_t action = (*self)->spawn_action;
    *d0 = (*d0 & 0xFFFFFF00u) | action;
    if (action == 0) {
        dk::jump(at_VM2_Next);
        return;
    }
    (*self)->spawn_action = 0;
    *d1 = (*d1 & 0xFFFF0000u) | (*self)->spawn_arg;
    SpawnAction_Run(d0, d1, d2, d3, d4, d5, d6, d7, a1, dk::addr(*script), *a3, *a4, *a5, dk::addr(*self));
    next(a0, script);
}

/* the speed tail ($AF4): speed = operand * g_TimeScale >> 8, tick = speed >> 4, tick_sq = tick^2;
   d0 as the asm leaves it (mulu, lsr.l, lsr.w, mulu) */
static void set_speed(uint32_t *speed, uint32_t *handler, const u16 **script, Vm2Block *vm)
{
    uint32_t d0 = (uint32_t)(uint16_t)*speed * (uint16_t)g_TimeScale >> 8;
    vm->speed = (uint16_t)d0;
    d0 = (d0 & 0xFFFF0000u) | (uint16_t)((uint16_t)d0 >> 4);
    vm->tick = (uint16_t)d0;
    d0 = (uint32_t)(uint16_t)d0 * (uint16_t)d0;
    vm->tick_sq = (uint16_t)d0;
    *speed = d0;
    next(handler, script);
}

/* Op2_SetSpeedRand - a2 op: Op2_SetSpeed with the operand base + a random value below range.
 * In:  script = words range, base
 * Out: vm->speed / tick / tick_sq; speed = tick squared (d0 as left); NEXT
 * Evidence: move.w (a2)+,d0 / jsr Rand_Below / add.w (a2)+,d0 / bra.s $AF4; executed in the TAS
 */
void Op2_SetSpeedRand(uint32_t *speed, uint32_t *handler, const u16 **script, Vm2Block *vm)
{
    *speed = (*speed & 0xFFFF0000u) | *(*script)++;
    Rand_Below(speed);
    *speed = (*speed & 0xFFFF0000u) | (uint16_t)(*speed + *(*script)++);
    set_speed(speed, handler, script, vm);
}

/* Op2_SetSpeed - a2 op: the object's speed factor = operand * g_TimeScale >> 8 ($100 = 1.0); its
 * timer tick = factor >> 4 (what every timer op subtracts per run), tick_sq = tick squared.
 * In:  script = word speed
 * Out: vm->speed / tick / tick_sq; speed = tick squared (d0 as left); NEXT
 * Evidence: move.w (a2)+,d0 / mulu.w (g_TimeScale).w,d0 / lsr.l #8 / move.w d0,$2E(a4) / lsr.w #4 /
 *           move.w d0,$30(a4) / mulu.w d0,d0 / move.w d0,$32(a4); executed in the TAS
 */
void Op2_SetSpeed(uint32_t *speed, uint32_t *handler, const u16 **script, Vm2Block *vm)
{
    *speed = (*speed & 0xFFFF0000u) | *(*script)++;
    set_speed(speed, handler, script, vm);
}

/* Op2_SetSprAttr - a2 op: the sprite attribute word (priority, palette, flips; d6 of Spr_Add*).
 * In:  script = word attributes
 * Out: spr->attr; NEXT
 * Evidence: move.w (a2)+,$38(a5); $4524 passes $38(a5) as d6 to Spr_AddSorted; executed in the TAS
 */
void Op2_SetSprAttr(const u16 **script, Vm2Sprite *spr)
{
    uint32_t handler;
    spr->attr = *(*script)++;
    next(&handler, script);
}

/* Op2_ModSprAttr - a2 op: and, then eor, on the attributes' high byte (sets, clears or toggles
 * flip / palette bits).
 * In:  script = byte and-mask, byte eor-mask
 * Out: spr->attr's high byte; mask = the eor byte (d0's low byte); NEXT
 * Evidence: move.b (a2)+,d0 / and.b d0,$38(a5) / move.b (a2)+,d0 / eor.b d0,$38(a5); executed in the TAS
 */
void Op2_ModSprAttr(uint8_t *mask, uint32_t *handler, const u16 **script, Vm2Sprite *spr)
{
    const u8 *bytes = (const u8 *)*script;
    uint8_t hi = (uint8_t)(attr_hi(spr) & bytes[0]);
    *mask = bytes[1];
    hi ^= *mask;
    spr->attr = (uint16_t)((hi << 8) | (spr->attr & 0xFF));
    (*script)++;
    next(handler, script);
}

/* Op2_BranchSprAttrEq - a2 op: branches when the attributes' high byte, masked, equals a value.
 * In:  script = byte mask, byte value, word offset
 * Out: bits = the masked byte; the branch (Op2_Goto), else NEXT after the offset
 * Evidence: move.b $38(a5),d0 / and.b (a2)+,d0 / cmp.b (a2)+,d0 / beq.w Op2_Goto / addq.l #2,a2
 */
void Op2_BranchSprAttrEq(uint8_t *bits, uint32_t *handler, const u16 **script, const Vm2Sprite *spr)
{
    const u8 *bytes = (const u8 *)*script;
    *bits = (uint8_t)(attr_hi(spr) & bytes[0]);
    (*script)++;
    if (*bits == bytes[1]) {
        dk::jump(at_Op2_Goto);
        return;
    }
    (*script)++;
    next(handler, script);
}

/* Op2_BranchSprAttrNe - a2 op: branches when the attributes' high byte, masked, differs from a
 * value.
 * In:  script = byte mask, byte value, word offset
 * Out: bits = the masked byte; the branch (Op2_Goto), else NEXT after the offset
 * Evidence: move.b $38(a5),d0 / and.b (a2)+,d0 / cmp.b (a2)+,d0 / bne.w Op2_Goto / addq.l #2,a2
 */
void Op2_BranchSprAttrNe(uint8_t *bits, uint32_t *handler, const u16 **script, const Vm2Sprite *spr)
{
    const u8 *bytes = (const u8 *)*script;
    *bits = (uint8_t)(attr_hi(spr) & bytes[0]);
    (*script)++;
    if (*bits != bytes[1]) {
        dk::jump(at_Op2_Goto);
        return;
    }
    (*script)++;
    next(handler, script);
}

/* Op2_SetSprDepth - a2 op: the sprite's sort depth (d7 of Spr_AddSorted; higher draws in front).
 * In:  script = word depth
 * Out: spr->depth; NEXT
 * Evidence: move.w (a2)+,$3A(a5); $4524 / $4570 pass $3A(a5) as d7; executed in the TAS
 */
void Op2_SetSprDepth(uint32_t *handler, const u16 **script, Vm2Sprite *spr)
{
    spr->depth = *(*script)++;
    next(handler, script);
}
