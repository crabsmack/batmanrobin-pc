#include "game.h"

/* Obj_NewMulti_Clear4 ($008972-$008983, with its entries Clear5-Clear8), the end of Obj_NewMulti's clear run, and
 * Obj_NewMultiUnlinked ($008984-$008999), which shares Obj_NewMulti's handle and clear runs
 * (bra.s Obj_NewMulti_FillHandles; the C loops over the same writes itself, as Obj_NewMulti's
 * does). The raw insert-after variant at $00899A never runs. */

static constexpr int NEWMULTI_MAX_BLOCKS = 9;   /* the lowest handle unit is at $8948 */

/* the clear run's last `words` move.w d0,(a1)+, then the end of Obj_NewMulti */
static void clear_tail(int words, uint32_t *d0, uint32_t *a1, uint32_t a2)
{
    PTR(s16) p;
    p.v = *a1;
    s16 *slot = p;
    for (int i = 0; i < words; i++)
        *slot++ = (int16_t)(uint16_t)*d0;
    *a1 = ADDR68(slot);
    g_ObjPoolHead = (uint16_t)a2;
    *d0 = 0;
}

/* Obj_NewMulti_Clear4 - the clear run's 4-block entry: zeroes the 5 handle words left, stores the
 * new pool head and returns success.
 * In:  d0 = the clears' value (0; low word); a1 = object +$E; a2 = the first block not taken
 * Out: a1 = past the handle words, g_ObjPoolHead = a2, d0 = 0 (the condition codes are its tst.l)
 * Evidence: 5 x move.w d0,(a1)+ / move.w a2,(g_ObjPoolHead).w / moveq #0,d0 / rts; jmp
 *           $896A(pc,d1.w) with d1 = 8 lands here for Obj_Spawn's 4-block script objects (140
 *           corpus runs), and Obj_NewMulti_ClearRun falls into it
 */
void Obj_NewMulti_Clear4(uint32_t *d0, uint32_t *a1, uint32_t a2)
{
    clear_tail(5, d0, a1, a2);
}

/* Obj_NewMulti_Clear5 - the clear run's 5-block entry: 4 handle words left to zero, then the end
 * of Obj_NewMulti (as Obj_NewMulti_Clear4).
 * In:  d0 = the clears' value (0; low word); a1 = object +$10; a2 = the first block not taken
 * Out: a1 = past the handle words, g_ObjPoolHead = a2, d0 = 0 (the condition codes are its tst.l)
 * Evidence: jmp $896A(pc,d1.w) at $8968 with d1 = 10 lands at $8974 (call-site log)
 */
void Obj_NewMulti_Clear5(uint32_t *d0, uint32_t *a1, uint32_t a2)
{
    clear_tail(4, d0, a1, a2);
}

/* Obj_NewMulti_Clear6 - the clear run's 6-block entry: 3 handle words left to zero, then the end
 * of Obj_NewMulti.
 * In:  d0 = the clears' value (0; low word); a1 = object +$12; a2 = the first block not taken
 * Out: a1 = past the handle words, g_ObjPoolHead = a2, d0 = 0 (the condition codes are its tst.l)
 * Evidence: jmp $896A(pc,d1.w) at $8968 with d1 = 12 lands at $8976 (call-site log)
 */
void Obj_NewMulti_Clear6(uint32_t *d0, uint32_t *a1, uint32_t a2)
{
    clear_tail(3, d0, a1, a2);
}

/* Obj_NewMulti_Clear7 - the clear run's 7-block entry (Stage1_SpawnSignDebris): 2 handle words
 * left to zero, then the end of Obj_NewMulti.
 * In:  d0 = the clears' value (0; low word); a1 = object +$14; a2 = the first block not taken
 * Out: a1 = past the handle words, g_ObjPoolHead = a2, d0 = 0 (the condition codes are its tst.l)
 * Evidence: jmp $896A(pc,d1.w) at $8968 with d1 = 14 lands at $8978 (call-site log)
 */
void Obj_NewMulti_Clear7(uint32_t *d0, uint32_t *a1, uint32_t a2)
{
    clear_tail(2, d0, a1, a2);
}

/* Obj_NewMulti_Clear8 - the clear run's 8-block entry: 1 handle word left to zero, then the end
 * of Obj_NewMulti.
 * In:  d0 = the clears' value (0; low word); a1 = object +$16; a2 = the first block not taken
 * Out: a1 = past the handle words, g_ObjPoolHead = a2, d0 = 0 (the condition codes are its tst.l)
 * Evidence: jmp $896A(pc,d1.w) at $8968 with d1 = 16 lands at $897A (call-site log)
 */
void Obj_NewMulti_Clear8(uint32_t *d0, uint32_t *a1, uint32_t a2)
{
    clear_tail(1, d0, a1, a2);
}

/* Obj_NewMultiUnlinked - Obj_NewMulti without the list insert: pops `blocks` pool blocks, sets
 * the handler and zero prev/next links, then the same handle words and clears.
 * In:  handler (a0, kept); blocks (d7: the pool count takes its low byte, the jumps its word; 1-9)
 * Out: d0 = 0, obj = the object on no list (a6), d1 = 2 x blocks, d7 = -4 x blocks,
 *      g_ObjPoolHead = the first block not taken; the condition codes are tst.l of d0 (a1/a2
 *      clobbered). Short of blocks: Obj_NewMultiFail (d0 = -1)
 * Evidence: sub.b d7,(g_ObjPoolFree).w / bcs Obj_NewMultiFail / movea.w (g_ObjPoolHead).w,a6 /
 *           movea.w (a6),a2 / movea.l a6,a1 / moveq #0,d0 / move.l a0,(a1)+ / move.l d0,(a1)+ /
 *           bra.s Obj_NewMulti_FillHandles; caller Obj_SpawnUnlinked (the ...Unlinked spawners)
 */
void Obj_NewMultiUnlinked(uint32_t *d0, uint16_t *d1, uint16_t *blocks, uint32_t *handler, uint32_t *obj)
{
    uint8_t want = (uint8_t)*blocks;
    bool borrow = g_ObjPoolFree < want;
    g_ObjPoolFree = (uint8_t)(g_ObjPoolFree - want);
    if (borrow) {
        dk::jump(at_Obj_NewMultiFail);
        return;
    }
    uint16_t n = *blocks;
    if (n > NEWMULTI_MAX_BLOCKS)
        dk::fatal("Obj_NewMultiUnlinked: %u blocks: the jump lands before the handle run", (unsigned)n);
    ObjNode *node = dk::handle<ObjNode>((int16_t)(uint16_t)g_ObjPoolHead);
    uint32_t next = (uint32_t)(int32_t)(int16_t)((ObjFreeBlock *)node)->next_free;   /* a2 */
    node->handler = *handler;
    node->prev = 0;                                                                   /* move.l d0,(a1)+ */
    node->next = 0;
    int handles = n > 0 ? n - 1 : 0;
    for (int i = 0; i < handles; i++) {
        node->blocks[i] = (int16_t)(uint16_t)next;                                    /* move.w a2,(a1)+ */
        next = (uint32_t)(int32_t)(int16_t)dk::handle<ObjFreeBlock>((int16_t)(uint16_t)next)->next_free;
    }
    for (int i = handles; i < OBJ_HANDLE_SLOTS; i++)
        node->blocks[i] = 0;                                                          /* move.w d0,(a1)+ */
    g_ObjPoolHead = (uint16_t)next;
    *obj = ADDR68(node);
    *d1 = (uint16_t)(n * 2);
    *blocks = (uint16_t)-(uint16_t)(n * 4);
    *d0 = 0;
}
