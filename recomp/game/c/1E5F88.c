#include "game.h"

/* the effect definitions (ROM), effect 1 first */
static constexpr uint32_t Tbl_1E50D6 = 0x1E50D6u;

/* Snd_PlaySfx - queues sound effect d0 (1-based) in its request slot unless a higher-priority
 * effect holds the slot; the V-int's Snd_SendSfx hands it to the Z80
 * In:  sound (d0) low word = the effect number (0 = none: returns at once)
 * Out: the slot's priority, hold and data from the effect's definition; d0 = the low word - 1 for
 *      effect 0, else the definition's priority byte (zero-extended); a0/a1 saved on the stack
 * Evidence: subq.w #1,d0 / bmi rts / movem.l a0-a1,-(a7) / lsl.w #3,d0 / lea Tbl_1E50D6(pc),a0 /
 *           adda.w d0,a0 / lea g_SndSfxSlots,a1 / moveq #0,d0 / move.b (a0)+,d0 / adda.w d0,a1 /
 *           move.b (a0)+,d0 / cmp.b (a1),d0 / bcs (keep the slot) / move.b (a0)+,(a1) /
 *           move.b (a0)+,1(a1) / move.l (a0)+,2(a1) / movem.l (a7)+,a0-a1 / rts; callers
 *           Op2_PlaySound, Op4_PlaySound, player and weapon code; 133 corpus runs
 */
void Snd_PlaySfx(uint32_t *sound)
{
    uint16_t index = (uint16_t)(*sound - 1u);
    *sound = (*sound & 0xFFFF0000u) | index;
    if ((int16_t)index < 0)
        return;
    /* lsl.w / adda.w: the word offset, sign-extended */
    const SndSfxDef *def = snd_rom<SndSfxDef>(Tbl_1E50D6 + (uint32_t)(int32_t)(int16_t)(uint16_t)(index << 3));
    /* the slot by its byte offset in the slot array, as the table gives it */
    SndSfxSlot *slot = (SndSfxSlot *)(g_SndSfxSlots + def->slot_offset);
    *sound = def->priority;
    if (def->priority < slot->priority)
        return;   /* a higher-priority effect is still in the slot */
    slot->priority = def->keep_priority;
    slot->hold = def->hold;
    slot->data = def->data;
}
