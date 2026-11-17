#include "game.h"

/* Obj_NewMultiFail ($00890A-$008911). */

/* Obj_NewMultiFail - the failure exit of Obj_NewMulti / Obj_NewMultiUnlinked when the pool holds
 * fewer than the blocks asked for: gives them back to g_ObjPoolFree (the sub.b borrowed) and
 * returns -1. Unlike ObjPool_Alloc it restores the count instead of clearing it.
 * In:  blocks = the blocks asked for (d7.b, already subtracted from g_ObjPoolFree)
 * Out: -1 (d0; moveq: the condition codes are its tst.l); g_ObjPoolFree restored
 * Evidence: add.b d7,(g_ObjPoolFree).w / moveq #-1,d0; bcs from Obj_NewMulti ($8916),
 *           Obj_NewMultiUnlinked ($8988) and the raw insert-after variant at $899E
 */
uint32_t Obj_NewMultiFail(uint8_t blocks)
{
    g_ObjPoolFree = (uint8_t)(g_ObjPoolFree + blocks);
    return 0xFFFFFFFFu;
}
