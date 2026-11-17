#include "game.h"

/* Obj_NewMulti_HandleRun ($00894C-$00896B): the unrolled handle run of Obj_NewMulti.
 *
 * 7 units of `move.w a2,(a1)+ / movea.w (a2),a2` (the next free block's address into the next
 * handle word, then walk the free list), entered by Obj_NewMulti's `jmp $896C(pc,d7.w)`
 * blocks - 1 units before its closing `jmp $896A(pc,d1.w)` into the clear run (d1 = 2 x blocks:
 * 9 - blocks `move.w d0,(a1)+`, then g_ObjPoolHead = a2 and d0 = 0). Obj_NewMulti's C loops over
 * the units itself; this entry is the 8-block one (all 7 units). */

static constexpr int HANDLERUN_UNITS = (0x00896C - 0x00894C) / 4 - 1;   /* 7; the jmp is the 8th */
static constexpr int CLEARRUN_WORDS = (0x00897C - 0x00896C) / 2;        /* 8 */

/* Obj_NewMulti_HandleRun - all 7 handle units, then the clear run's tail chosen by d1, and the
 * end of Obj_NewMulti.
 * In:  d0 = the clears' value (0; low word); d1 = 2 x blocks (the second jump's displacement:
 *      2-18); a1 = the next handle word; a2 = the next free block
 * Out: a1 = past the last word written, a2 = the first block not taken (= g_ObjPoolHead),
 *      d0 = 0 (the condition codes are its tst.l)
 * Evidence: 7 x (move.w a2,(a1)+ / movea.w (a2),a2), jmp $896A(pc,d1.w) into
 *           Obj_NewMulti_ClearRun / Obj_NewMulti_Clear4 (move.w a2,(g_ObjPoolHead).w /
 *           moveq #0,d0 / rts); entered by the computed jump at $008944 (dkit.toml
 *           computed_entry_runs), so its start is that jump's largest count but one (8 blocks;
 *           $8948 is the 9-block unit)
 */
void Obj_NewMulti_HandleRun(uint32_t *d0, uint16_t d1, uint32_t *a1, uint32_t *a2)
{
    int16_t disp = (int16_t)d1;
    if (disp < 2 || disp > 2 * (CLEARRUN_WORDS + 1) || (disp & 1))
        dk::fatal("Obj_NewMulti_HandleRun: jmp $896A(pc,$%04X) lands outside the clear run", (unsigned)d1);
    int clears = CLEARRUN_WORDS + 1 - disp / 2;
    PTR(s16) p;
    p.v = *a1;
    s16 *slot = p;
    for (int i = 0; i < HANDLERUN_UNITS; i++) {
        *slot++ = (int16_t)(uint16_t)*a2;                                            /* move.w a2,(a1)+ */
        *a2 = (uint32_t)(int32_t)(int16_t)dk::handle<ObjFreeBlock>((int16_t)(uint16_t)*a2)->next_free;  /* movea.w (a2),a2 */
    }
    for (int i = 0; i < clears; i++)
        *slot++ = (int16_t)(uint16_t)*d0;                                            /* move.w d0,(a1)+ */
    *a1 = ADDR68(slot);
    g_ObjPoolHead = (uint16_t)*a2;
    *d0 = 0;
}
