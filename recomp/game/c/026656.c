#include "game.h"

/* the hull-hung ladder's motion ops after this native (the bytes after its jsr $7FE.w) */
static constexpr uint32_t Scr2_026666 = 0x026666u;

/* Enemy_LadderThug_LoadHullPos - native at the top of the hull-hung ladder's frame: restores its
 * untilted hull-space position from script variables 1/2 before the motion ops move it
 * In:  vm = its VM block (a4: var 1/2 = x/y), self = the ladder (a6); the rest its registers
 * Out: self x/y = var 1/2; the script continues at Scr2_026666 (jsr $7FE.w)
 * Evidence: move.w $24(a4),$3E(a6) / move.w $26(a4),$42(a6) / jsr $7FE.w; pairs with
 *           Enemy_LadderThug_PlaceOnHull in the loop $026654-$026694; TAS f33180-f35226 (1212
 *           frames)
 */
void Enemy_LadderThug_LoadHullPos(uint32_t d0, uint32_t d1, uint32_t d2, uint32_t d3, uint32_t d4, uint32_t d5, uint32_t d6,
                                  uint32_t d7, uint32_t a1, uint32_t a3, const Vm2Block *vm, uint32_t a5, Vm2Obj *self)
{
    self->x = (s16)vm->var[1];
    self->y = (s16)vm->var[2];
    dk::jump_script(at_VM2_Enter, Scr2_026666);
}
