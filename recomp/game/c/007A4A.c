#include "game.h"

#if DK_MODE_NATIVE
extern "C" void hf_spr_add(uint16_t entry);   /* platform/native/present.h: high_fps's sprite owners (phase 6 R3) */
#endif

/* Spr_AddSorted - adds a sprite to the display list in descending order of its depth key (earlier
 * = higher SAT priority = in front): a new key node before the first node whose key is at most
 * the new one, or a 12-byte entry chained to a node with the same key.
 * In:  a0 = frame-table entry, d4 = x, d5 = y, d6 = attributes, d7 = depth key; a1 / a2 kept
 * Out: d1 / d2 = a1 / a2 (saved through them); d0 = the node inserted before, or the new chained
 *      entry's base; g_SprNodeAlloc = the new node; the condition codes of that move.w. With the 80
 *      entries used up: nothing (g_SprNodesFree stays 0)
 * Evidence: subq.b #1,(g_SprNodesFree).w / bcs / move.l a1,d1 / move.l a2,d2 / lea $9DE2,a1 / the
 *           walk (4x unrolled: movea.w (a1),a2 / cmp.w (a2)+,d7 / bcc ...) / beq: the chained entry
 *           (move.l a0,-(a2) / movem.w d0/d4-d6,-(a2) ...) / else the key node (movem.w d0/a2,-(a2):
 *           the 68000 stores a2's value before the decrement) / move.w a2,(g_SprNodeAlloc).w; the
 *           a2 VM sprite ops at $4524-$4630 call it with d7 = $3A(a5)
 */
void Spr_AddSorted(uint16_t *d0, uint32_t *d1, uint32_t *d2, uint16_t d4, uint16_t d5, uint16_t d6, uint16_t d7,
                   uint32_t a0, uint32_t a1, uint32_t a2, dk::Flags *ccr)
{
    uint8_t free_nodes = g_SprNodesFree;
    g_SprNodesFree = (uint8_t)(free_nodes - 1);
    if (free_nodes == 0) {   /* bcs: none left */
        g_SprNodesFree = 0;
#if DK_MODE_NATIVE
        /* phase 6 (R10): counted, and with unlimited_sprites kept on the host for Spr_BuildSAT;
           everything the game sees is the refusal's */
        spr_host_list_refused(SPR_HOST_SORTED, a0, (int16_t)d4, (int16_t)d5, d6, d7);
#endif
        *ccr = dk::Flags{ ccr->x, false, true, false, false };
        return;
    }
    *d1 = a1;
    *d2 = a2;
    /* the walk: link = the link word that points at the node being compared */
    uint16_t link = (uint16_t)ADDR68(&((SprKeyNode *)g_SprSortHead)->next);
    SprKeyNode *node;
    for (;;) {
        node = spr_node(*spr_word(link));
        if (d7 >= (uint16_t)node->key)   /* cmp.w / bcc: unsigned */
            break;
        link = (uint16_t)ADDR68(&node->next);
    }
    uint16_t at = (uint16_t)ADDR68(node);
    uint16_t alloc = g_SprNodeAlloc;
    if (d7 != (uint16_t)node->key) {
        /* a new key node before it, carved from the pool's top */
        SprKeyNode *k = spr_node((uint16_t)(alloc - sizeof(SprKeyNode)));
        k->entry.frame.v = a0;
        k->entry.link = at;
        k->entry.x = (int16_t)d4;
        k->entry.y = (int16_t)d5;
        k->entry.attr = d6;
        k->next = at;
        k->last = (uint16_t)ADDR68(&k->entry);
        k->key = d7;
#if DK_MODE_NATIVE
        hf_spr_add((uint16_t)ADDR68(&k->entry));   /* high_fps: the running object's entry */
#endif
        uint16_t kh = (uint16_t)ADDR68(k);
        *spr_word(link) = kh;                          /* the previous node's next */
        *spr_word(*spr_word((uint16_t)(link + 2))) = kh;   /* and its chain's last entry's link */
        g_SprNodeAlloc = kh;
        *d0 = at;
        *ccr = dk::Flags::test(kh, 2, ccr->x);
        return;
    }
    /* the same key: a sprite entry chained after the node's last one */
    SprEntry *e = spr_entry((uint16_t)(alloc - sizeof(SprEntry)));
    e->frame.v = a0;
    e->link = node->next;
    e->x = (int16_t)d4;
    e->y = (int16_t)d5;
    e->attr = d6;
#if DK_MODE_NATIVE
    hf_spr_add((uint16_t)ADDR68(e));   /* high_fps: the running object's entry */
#endif
    uint16_t eh = (uint16_t)ADDR68(e);
    uint16_t last = node->last;
    node->last = eh;
    *d0 = (uint16_t)(eh - 6);   /* linked as a node base: its entry at +6 */
    *spr_word(last) = *d0;
    g_SprNodeAlloc = *d0;
    *ccr = dk::Flags::test(*d0, 2, ccr->x);
}
