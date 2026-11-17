#include "game.h"

/* a 68000 address held in a register, as a pointer (PTR(T): the 68000 value kept) */
template <class T> static T *ptr(uint32_t a)
{
    PTR(T) p;
    p.v = a;
    return p;
}

/* A block on the V-int object list (VIntObj_New links it before g_VIntListTail): the handler
 * the V-int calls, and the list links. Free, the block's first word links the pool's free list. */
struct VIntListNode_8BF6 {
    union {
        u32 handler;       /* +$00: the V-int handler (VIntObj_New's a0) */
        s16 free_next;     /* +$00: once freed: the next free pool block (g_ObjPoolHead's chain) */
    };
    s16 prev;              /* +$04: the previous node (g_VIntListHead for the first) */
    s16 next;              /* +$06: the next node (g_VIntListTail for the last) */
};
static_assert(offsetof(VIntListNode_8BF6, prev) == 0x04, "VIntListNode_8BF6.prev");
static_assert(offsetof(VIntListNode_8BF6, next) == 0x06, "VIntListNode_8BF6.next");

/* VIntObj_UnlinkFree - unlinks a V-int list object and returns its block to the object pool,
 * with g_VIntOff raised over the edit so a V-int can't walk the list half-linked.
 * In:  a6 = the object
 * Out: a1 = the node that followed it (movem.w: the word sign-extended); g_ObjPoolHead = the block,
 *      g_ObjPoolFree + 1
 * Evidence: addq.b #1,g_VIntOff / movem.w $4(a6),a0-a1 / move.w a1,$6(a0) / move.w a0,$4(a1) /
 *           move.w g_ObjPoolHead,(a6) / move.w a6,g_ObjPoolHead / addq.b #1,g_ObjPoolFree / subq.b;
 *           callers VM6_EndClearJob and the blit job's end hook ($74A8)
 */
uint32_t VIntObj_UnlinkFree(uint32_t a6)
{
    VIntListNode_8BF6 *self = ptr<VIntListNode_8BF6>(a6);
    g_VIntOff++;
    int16_t prev = self->prev, next = self->next;
    dk::handle<VIntListNode_8BF6>(prev)->next = next;
    dk::handle<VIntListNode_8BF6>(next)->prev = prev;
    self->free_next = (int16_t)g_ObjPoolHead;
    g_ObjPoolHead = (uint16_t)a6;
    g_ObjPoolFree++;
    g_VIntOff--;
    return (uint32_t)(int32_t)next;
}
