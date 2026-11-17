#include "game.h"

/* Obj_NewMulti ($008912-$00894B) and its shared tail Obj_NewMulti_FillHandles ($00893C).
 *
 * The original fills the object's 8 handle words with two computed jumps: `jmp $896C(pc,d7.w)`
 * with d7 = -4 x blocks lands blocks - 1 units before the end of Obj_NewMulti_HandleRun (each
 * unit `move.w a2,(a1)+ / movea.w (a2),a2`: the next free block's address, then walk the free
 * list; the unit at $8948 just before that file is the 9-block one), whose closing
 * `jmp $896A(pc,d1.w)` with d1 = 2 x blocks lands 9 - blocks words before the end of the clear
 * run (Obj_NewMulti_ClearRun, Obj_NewMulti_Clear4: `move.w d0,(a1)+`). 0 blocks jumps straight to
 * the clear run's start: no handle, 8 clears. The C loops over the same writes, in the same
 * order. */

static constexpr uint32_t NEWMULTI_JUMP_BASE = 0x00896C;  /* jmp $896C(pc,d7.w) */
static constexpr int NEWMULTI_MAX_BLOCKS = 9;             /* the lowest unit is at $8948 */

/* the handle and clear runs for `blocks`: one handle word per extra block (the next free block,
 * walking the free list), zeroes to the 8th, then the pool head = the first block not taken */
static void fill_handles(uint16_t blocks, uint16_t zero, uint32_t *a1, uint32_t *a2)
{
    if (blocks > NEWMULTI_MAX_BLOCKS)
        dk::fatal("Obj_NewMulti: %u blocks: jmp $%06X lands before the handle run", (unsigned)blocks,
                  (unsigned)(NEWMULTI_JUMP_BASE - 4u * blocks));
    int handles = blocks > 0 ? blocks - 1 : 0;
    PTR(s16) p;
    p.v = *a1;
    s16 *slot = p;
    for (int i = 0; i < handles; i++) {
        *slot++ = (int16_t)(uint16_t)*a2;                                            /* move.w a2,(a1)+ */
        *a2 = (uint32_t)(int32_t)(int16_t)dk::handle<ObjFreeBlock>((int16_t)(uint16_t)*a2)->next_free;  /* movea.w (a2),a2 */
    }
    for (int i = handles; i < OBJ_HANDLE_SLOTS; i++)
        *slot++ = (int16_t)zero;                                                     /* move.w d0,(a1)+ */
    *a1 = ADDR68(slot);
    g_ObjPoolHead = (uint16_t)*a2;
}

/* Obj_NewMulti - creates a multi-block object: pops `blocks` pool blocks, makes the first the
 * object (handler, linked before the main list's tail), stores the other blocks' addresses as
 * handles from +8 and zeroes the rest of the 8 handle words.
 * In:  handler (a0); blocks (d7: the pool count takes its low byte, the jumps its word; 1-9,
 *      4 for a script object: a3/a4/a5 handles at +8/+$A/+$C)
 * Out: d0 = 0, obj = the object (a6), last = the previous last object (a0), a1 = obj +$18,
 *      a2 = the first block not taken (= g_ObjPoolHead), d1 = 2 x blocks, d7 = -4 x blocks;
 *      the condition codes are tst.l of d0. Short of blocks: Obj_NewMultiFail (d0 = -1)
 * Evidence: sub.b d7,(g_ObjPoolFree).w / bcs Obj_NewMultiFail / movea.w (g_ObjPoolHead).w,a6 /
 *           movea.w (a6),a2 / move.l a0,(a6) / the Obj_New link / lea $8(a6),a1 / moveq #0,d0,
 *           then Obj_NewMulti_FillHandles; callers Obj_Spawn (every Obj_SpawnEnemy* / Boss*
 *           spawner), Stage1_SpawnSignDebris (7 blocks)
 */
void Obj_NewMulti(uint32_t *d0, uint16_t *d1, uint16_t *blocks, uint32_t *handler_last, uint32_t *a1,
                  uint32_t *a2, uint32_t *obj)
{
    uint8_t want = (uint8_t)*blocks;
    bool borrow = g_ObjPoolFree < want;
    g_ObjPoolFree = (uint8_t)(g_ObjPoolFree - want);
    if (borrow) {
        dk::jump(at_Obj_NewMultiFail);
        return;
    }
    ObjNode *node = dk::handle<ObjNode>((int16_t)(uint16_t)g_ObjPoolHead);
    *a2 = (uint32_t)(int32_t)(int16_t)((ObjFreeBlock *)node)->next_free;
    node->handler = *handler_last;
    ObjNode *tail = (ObjNode *)g_ObjListTail;
    ObjNode *last = dk::handle<ObjNode>(tail->prev);
    tail->prev = dk::handle_of(node);
    last->next = dk::handle_of(node);
    node->prev = dk::handle_of(last);
    node->next = dk::handle_of(tail);
    *handler_last = ADDR68(last);
    *obj = ADDR68(node);
    *a1 = ADDR68(&node->blocks[0]);
    *d0 = 0;
    *d1 = (uint16_t)(*blocks * 2);
    fill_handles(*blocks, 0, a1, a2);
    *blocks = (uint16_t)-(uint16_t)(*blocks * 4);
}

/* Obj_NewMulti_FillHandles - the shared tail of Obj_NewMulti (and Obj_NewMultiUnlinked's
 * bra.s): from the block count, the handle words and the clears, then the new pool head.
 * In:  d0 = 0 (the clears' value, low word); blocks (d7.w); a1 = the object's first handle word
 *      (+8); a2 = the next free block; a0, a6 pass through
 * Out: d1 = 2 x blocks, d7 = -4 x blocks, a1 = past the 8 handle words, a2 = the first block not
 *      taken (= g_ObjPoolHead), d0 = 0 (the condition codes are its tst.l)
 * Evidence: add.w d7,d7 / move.w d7,d1 / add.w d7,d7 / neg.w d7 / jmp $896C(pc,d7.w), the run's
 *           jmp $896A(pc,d1.w), the clear run's move.w a2,(g_ObjPoolHead).w / moveq #0,d0
 */
void Obj_NewMulti_FillHandles(uint32_t *d0, uint16_t *d1, uint16_t *blocks, uint32_t *a0, uint32_t *a1,
                              uint32_t *a2, uint32_t *a6)
{
    (void)a0; (void)a6;
    *d1 = (uint16_t)(*blocks * 2);
    fill_handles(*blocks, (uint16_t)*d0, a1, a2);
    *blocks = (uint16_t)-(uint16_t)(*blocks * 4);
    *d0 = 0;
}
