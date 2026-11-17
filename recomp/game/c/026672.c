#include "game.h"

/* the loop's script after this native (the bytes after its jsr $7FE.w) */
static constexpr uint32_t Scr2_02668C = 0x02668Cu;

static constexpr int16_t LADDER_HANG_DROP = 0x50;   /* the ladder hangs this far below the hull point */

/* low word of a data register replaced, the upper word kept (move.w into a data register) */
static inline void set_low_word(uint32_t *reg, uint16_t w) { *reg = (*reg & 0xFFFF0000u) | w; }

/* Enemy_LadderThug_PlaceOnHull - native after the hull-hung ladder's motion ops: saves the moved
 * position in script variables 1/2 (its untilted hull-space point), places it at that point
 * tilted with the arena floor (Boss_Stage2_PlaceTurret) and hangs it $50 px lower
 * In:  vm = its VM block (a4), self = the ladder (a6); the rest its registers
 * Out: var 1/2 = the moved position; self x/y = the screen position; d0-d5 as
 *      Boss_Stage2_PlaceTurret left them; the script continues at Scr2_02668C (jsr $7FE.w)
 * Evidence: move.w $3E(a6),$24(a4) / move.w $42(a6),$26(a4) / bsr.w Boss_Stage2_PlaceTurret /
 *           addi.w #$50,$42(a6) / jsr $7FE.w; TAS f33179-f35225 (1212 frames). The prototype
 *           takes d0-d5 by address (measured d0-d2 inputs, d3-d5 nothing: the contract ends at
 *           the jsr) so the script gets the registers PlaceTurret left, as on the 68000
 */
void Enemy_LadderThug_PlaceOnHull(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5,
                                  uint32_t d6, uint32_t d7, uint32_t a1, uint32_t a3, Vm2Block *vm, uint32_t a5,
                                  Vm2Obj *self)
{
    vm->var[1] = (u16)self->x;
    vm->var[2] = (u16)self->y;
    uint16_t x = (uint16_t)*d0, y = (uint16_t)*d1, tilt = (uint16_t)*d2;
    Boss_Stage2_PlaceTurret(&x, &y, &tilt, d3, d4, d5, vm, self);
    set_low_word(d0, x);
    set_low_word(d1, y);
    set_low_word(d2, tilt);
    self->y = (s16)(self->y + LADDER_HANG_DROP);
    dk::jump_script(at_VM2_Enter, Scr2_02668C);
}
