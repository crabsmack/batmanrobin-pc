#include "game.h"

#if DK_MODE_NATIVE
extern "C" void hf_spr_add(uint16_t entry);   /* platform/native/present.h: high_fps's sprite owners (phase 6 R3) */
#endif

/* Spr_AddBack - appends a sprite after the last entry of the sorted display list (behind
 * everything added so far): the entry goes into the end slot below g_SprSortTail and a new end
 * node (entry link $FFFF) is carved from the pool for the next one.
 * In:  a0 = frame-table entry, d4 = x, d5 = y, d6 = attributes; a1 kept
 * Out: d1 = a1 (saved through it); d0 = the new end node = g_SprNodeAlloc, g_SprSortTail = the old
 *      g_SprNodeAlloc; the condition codes of that move.w. With the 80 entries used up: nothing
 * Evidence: subq.b #1,(g_SprNodesFree).w / bmi / move.l a1,d1 / movea.w (g_SprSortTail).w,a1 /
 *           move.w (g_SprNodeAlloc).w,d0 / move.w d0,(g_SprSortTail).w / subi.w #$12,d0 / move.l
 *           a0,-(a1) / movem.w d0/d4-d6,-(a1) / move.w #$FFFF,6(d0) / move.w d0,(g_SprNodeAlloc).w;
 *           callers $478C, $4AB4, $6776, $20A48, $36CD4
 */
void Spr_AddBack(uint16_t *d0, uint32_t *d1, uint16_t d4, uint16_t d5, uint16_t d6, uint32_t a0, uint32_t a1,
                 dk::Flags *ccr)
{
    g_SprNodesFree = (uint8_t)(g_SprNodesFree - 1);
    if ((int8_t)g_SprNodesFree < 0) {   /* bmi: none left */
        g_SprNodesFree = 0;
#if DK_MODE_NATIVE
        /* phase 6 (R10): counted, and with unlimited_sprites kept on the host (Spr_AddSorted) */
        spr_host_list_refused(SPR_HOST_BACK, a0, (int16_t)d4, (int16_t)d5, d6, 0);
#endif
        *ccr = dk::Flags{ ccr->x, false, true, false, false };
        return;
    }
    *d1 = a1;
    SprEntry *e = spr_entry((uint16_t)(g_SprSortTail - sizeof(SprEntry)));
    *d0 = g_SprNodeAlloc;
    g_SprSortTail = *d0;
    *d0 = (uint16_t)(*d0 - sizeof(SprKeyNode));
    e->frame.v = a0;
    e->link = *d0;
    e->x = (int16_t)d4;
    e->y = (int16_t)d5;
    e->attr = d6;
#if DK_MODE_NATIVE
    hf_spr_add((uint16_t)ADDR68(e));   /* high_fps: the running object's entry */
#endif
    spr_node(*d0)->entry.link = 0xFFFF;   /* the new end */
    g_SprNodeAlloc = *d0;
    *ccr = dk::Flags::test(*d0, 2, ccr->x);
}
