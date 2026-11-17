#include "game.h"

/* Spr_ResetList - empties the per-frame sprite display list: 80 free entries, the key-node pool
 * from its top, the front list from its start, the head linked to the tail sentinel (its entry
 * link $FFFF = the end), the SAT buffer's write pointer at its start, no sprite counted.
 * In:  -
 * Out: g_SprNodesFree = 80, g_SprNodeAlloc = $A3A4, g_SprFrontEnd = $9E16; head next / its entry
 *      link = the tail, head last = its own entry; tail key 0, entry link $FFFF, last = the AddBack
 *      slot; g_SATPtr = g_SATBuffer, g_SprCount = 0; a0 = the head, a1 = the tail
 * Evidence: move.b #$50,(g_SprNodesFree).w / move.w #$A3A4 / #$9E16 / lea g_SprSortHead,a0 / lea
 *           $9DF2,a1 / the links / move.w #$9B56,(g_SATPtr).w / clr.w (g_SprCount).w; called at every
 *           frame start ($9B86, $9BAC)
 */
void Spr_ResetList(uint32_t *a0, uint32_t *a1)
{
    g_SprNodesFree = SPR_NODES;
    g_SprNodeAlloc = SPR_NODE_POOL_TOP;
    SprKeyNode *head = (SprKeyNode *)g_SprSortHead;
    SprKeyNode *tail = (SprKeyNode *)(g_SprSortHead + SPR_TAIL_OFFSET);
    uint16_t tail_h = (uint16_t)ADDR68(tail);
    g_SprFrontEnd = (uint16_t)ADDR68(g_SprSortHead + SPR_FRONT_OFFSET);
    *a0 = ADDR68(head);
    *a1 = ADDR68(tail);
    head->next = tail_h;
    head->entry.link = tail_h;
    head->last = (uint16_t)ADDR68(&head->entry);
    tail->key = 0;
    tail->entry.link = 0xFFFF;
    tail->last = (uint16_t)ADDR68(tail + 1);   /* where Spr_AddBack puts its first entry (below this) */
    g_SATPtr = (uint16_t)ADDR68(g_SATBuffer);
    g_SprCount = 0;
#if DK_MODE_NATIVE
    spr_host_list_reset();   /* phase 6 (R10): the host's nodes past the cap start empty too */
    ws_draw_list_reset();    /* phase 6 (R11): and the culled draws the wide view adds */
#endif
}
