#include "game.h"

#if DK_MODE_NATIVE
extern "C" void hf_spr_add(uint16_t entry);   /* platform/native/present.h: high_fps's sprite owners (phase 6 R3) */
#endif

/* Spr_AddFront - appends a sprite to the unsorted front list, which Spr_BuildSAT emits before the
 * sorted list (in front of every sorted sprite): an entry below g_SprFrontEnd linked to it, then
 * g_SprFrontEnd moves up by a node.
 * In:  a0 = frame-table entry, d4 = x, d5 = y, d6 = attributes
 * Out: d0 = the frame entry, d1 = the old g_SprFrontEnd (the entry's link), a0 = the entry;
 *      g_SprFrontEnd + $12 and the condition codes of that addi.w. With the 80 entries used up:
 *      nothing
 * Evidence: subq.b #1,(g_SprNodesFree).w / bmi / move.l a0,d0 / movea.w (g_SprFrontEnd).w,a0 /
 *           move.w a0,d1 / move.l d0,-(a0) / movem.w d1/d4-d6,-(a0) / addi.w #$12,(g_SprFrontEnd).w;
 *           callers $46CC, $6776, $1412E, $37708, $41F0E
 */
void Spr_AddFront(uint32_t *d0, uint16_t *d1, uint16_t d4, uint16_t d5, uint16_t d6, uint32_t *a0, dk::Flags *ccr)
{
    g_SprNodesFree = (uint8_t)(g_SprNodesFree - 1);
    if ((int8_t)g_SprNodesFree < 0) {   /* bmi: none left */
        g_SprNodesFree = 0;
#if DK_MODE_NATIVE
        /* phase 6 (R10): counted, and with unlimited_sprites kept on the host (Spr_AddSorted) */
        spr_host_list_refused(SPR_HOST_FRONT, *a0, (int16_t)d4, (int16_t)d5, d6, 0);
#endif
        *ccr = dk::Flags{ ccr->x, false, true, false, false };
        return;
    }
    *d0 = *a0;
    *d1 = g_SprFrontEnd;
    SprEntry *e = spr_entry((uint16_t)(*d1 - sizeof(SprEntry)));
    e->frame.v = *d0;
    e->link = *d1;
    e->x = (int16_t)d4;
    e->y = (int16_t)d5;
    e->attr = d6;
#if DK_MODE_NATIVE
    hf_spr_add((uint16_t)ADDR68(e));   /* high_fps: the running object's entry */
#endif
    *a0 = ADDR68(e);
    uint16_t end = g_SprFrontEnd;
    uint16_t moved = (uint16_t)(end + sizeof(SprKeyNode));
    g_SprFrontEnd = moved;
    bool carry = moved < end;
    *ccr = dk::Flags{ carry, (moved & 0x8000) != 0, moved == 0,
                      ((~(end ^ 0x12u) & (end ^ moved)) & 0x8000u) != 0, carry };
}
