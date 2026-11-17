#include "game.h"

/* Trigger_Fire - fires one trigger: its callback runs with d4/d5 = the entry's x/y; when the
 * callback succeeds (Z set) and the callback long's top byte is nonzero, that byte becomes the
 * spawned object's spawn action (run when it dies).
 * In:  d0 = the g_TriggerList element (key, offset) swapped: the entry's offset in the high word;
 *      table = g_TriggerTable (a3); everything else the callback reads
 * Out: d0 = the element swapped back with the action byte in its low byte (0 after a stored
 *      action); x/y (d4/d5 low words), a0 = the callback, a1 = past the entry; Z set = fired, clear
 *      = the callback asked to be retried; the registers the callback hands back
 * Evidence: swap d0 / lea $6(a3,d0.w),a1 / move.w (a1)+,d4 / move.w (a1)+,d5 / move.b (a1),d0 /
 *           movea.l (a1)+,a0 / movem.l d0/d6-d7/a2-a3 around jsr (a0) / bne / tst.b d0 / move.b
 *           d0,$19(a6) / moveq #0,d0; called by Trigger_RunX/RunY and $02F58C
 */
void Trigger_Fire(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t d3, uint32_t *d4, uint32_t *d5, uint32_t d6,
                  uint32_t d7, uint32_t *a0, uint32_t *a1, uint32_t a2, const u8 *table, uint32_t *a4, uint32_t *a5,
                  uint32_t *a6, dk::Flags *ccr)
{
    uint32_t v = (*d0 << 16) | (*d0 >> 16);                 /* swap: the offset into the low word */
    const TriggerEntry *e = (const TriggerEntry *)(table + (int16_t)(uint16_t)v);
    *d4 = (*d4 & 0xFFFF0000u) | (uint16_t)e->x;
    *d5 = (*d5 & 0xFFFF0000u) | (uint16_t)e->y;
    uint8_t action = (uint8_t)(e->callback >> 24);
    v = (v & 0xFFFFFF00u) | action;
    *a0 = e->callback;
    *a1 = ADDR68(&e->callback + 1);
    dk::Flags f = dk::Flags::test(action, 1, ccr->x);      /* as move.b (a1),d0 left them */
    /* movem.l d0/d6-d7/a2-a3,-(a7): on the 68000's stack, so the callback's frames sit as deep as
       the original's */
    dk::push_long(ADDR68(table));
    dk::push_long(a2);
    dk::push_long(d7);
    dk::push_long(d6);
    dk::push_long(v);
    TriggerCallback(*a0, v, d1, d2, d3, d4, d5, d6, d7, a0, a1, a2, ADDR68(table), a4, a5, a6, &f);
    v = dk::pop_long();                                     /* the saved d0: the callback's is lost */
    for (int i = 0; i < 4; i++)
        dk::pop_long();                                     /* d6/d7/a2/a3: this C's copies stand */
    *d0 = v;
    if (!f.z) {
        *ccr = f;
        return;
    }
    *ccr = dk::Flags::test(action, 1, f.x);
    if (action == 0)
        return;
    PTR(Vm2Obj) spawned;
    spawned.v = *a6;
    spawned->spawn_action = action;
    *d0 = 0;
    *ccr = dk::Flags::test(0, 4, f.x);
}
