#include "game.h"

/* the spawn action table: 50 code longs (ROM, right after SpawnAction_Run; batmanrobin.asm) */
static constexpr uint32_t Tbl_012EF0 = 0x012EF0u;

/* SpawnAction_Run - runs spawn action d0 (its low byte) of Tbl_012EF0 at the object's position:
 * item and health drops, group counters. a2-a6 are kept across the action (saved on the 68000's
 * stack, as the original's movem).
 * In:  d0 = action number (low byte), d1 = its argument, a6 = the object (its x/y become d4/d5)
 * Out: d0 = action x 4 going in, then whatever the action leaves in d0-d7 and a1; d4/d5 = the
 *      object's x/y going in (low words)
 * Evidence: movem.l a2-a6,-(a7) / move.w $3E(a6),d4 / move.w $42(a6),d5 / andi.w #$FF,d0 / add.w
 *           d0,d0 twice / movea.l Tbl_012EF0(pc,d0.w),a0 / jsr (a0) / movem.l (a7)+,a2-a6; callers
 *           Op2_RunSpawnAction ($ACC), $6F56 and $03D9AE; 623 TAS frames
 */
void SpawnAction_Run(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6,
                     uint32_t *d7, uint32_t *a1, uint32_t a2, uint32_t a3, uint32_t a4, uint32_t a5, uint32_t a6)
{
    PTR(const Vm2Obj) self;
    self.v = a6;
    dk::push_long(a6);                      /* movem.l a2-a6,-(a7) */
    dk::push_long(a5);
    dk::push_long(a4);
    dk::push_long(a3);
    dk::push_long(a2);
    *d4 = (*d4 & 0xFFFF0000u) | (uint16_t)self->x;
    *d5 = (*d5 & 0xFFFF0000u) | (uint16_t)self->y;
    uint16_t index = (uint16_t)((*d0 & 0xFF) * 4);
    *d0 = (*d0 & 0xFFFF0000u) | index;
    PTR(const u32) table;
    table.v = Tbl_012EF0;
    uint32_t action = table.get()[index / 4];
    SpawnActionFn(action, d0, d1, d2, d3, d4, d5, d6, d7, &action, a1, &a2, &a3, &a4, &a5, &a6);
    for (int i = 0; i < 5; i++)
        dk::pop_long();                      /* movem.l (a7)+,a2-a6: the entry values stand */
}

/* SpawnAction_None - spawn action 0: nothing (the rts closing SpawnAction_Run, which Tbl_012EF0's
 * first long names).
 * In:  none
 * Out: none
 * Evidence: rts at $012EEE; Tbl_012EF0 long 0; objects with no death action ($19(a6) = 0)
 */
void SpawnAction_None(void)
{
}
