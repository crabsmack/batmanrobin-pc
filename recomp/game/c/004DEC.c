#include "game.h"

/* Hitbox_TestAttackBox - tests one attack box at the object's position: Hitbox_TestAttackBoxAt
 * (which it falls into) with d1 / d2 = the object's x / y.
 * In:  a0 = the attack box; a5 = the sprite block (flips); a6 = the owner object
 * Out: d1 / d2 low words = x / y; then Hitbox_TestAttackBoxAt's
 * Evidence: move.w $3E(a6),d1 / move.w $42(a6),d2, falls into $4DF4; called by Op2_TestAttackBoxes;
 *           22k TAS frames
 */
void Hitbox_TestAttackBox(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5,
                          uint32_t *d6, uint32_t *d7, uint32_t a0, uint32_t *a1, uint32_t a3, uint32_t *a4,
                          uint32_t a5, uint32_t *a6)
{
    (void)d0, (void)d3, (void)d4, (void)d5, (void)d6, (void)d7, (void)a0, (void)a1, (void)a3, (void)a4, (void)a5;
    PTR(const Vm2Obj) self;
    self.v = *a6;
    *d1 = (*d1 & 0xFFFF0000u) | (uint16_t)self->x;
    *d2 = (*d2 & 0xFFFF0000u) | (uint16_t)self->y;
    dk::jump(at_Hitbox_TestAttackBoxAt);
}
