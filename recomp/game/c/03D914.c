#include "game.h"

/* the little hat's frame loop after the native (its jsr $53C6.w: VM4_Enter), the draw op */
static constexpr uint32_t Scr4_03D932 = 0x03D932u;

enum {
    LITTLE_HAT_FALL = 0x18,   /* deleted once it has fallen this far below the owner's y */
};

/* Boss_Hatter_LittleHatFollow - a4 native in the little hat's frame loop: keeps it at its owner's
 * x, and deletes it (Op4_Kill) once it has fallen 24 px below the owner's y, else goes on to the
 * draw op
 * In:  d5 = the hat's drawn y (Op4_MoveXY); hat = the little hat (a6: owner +$12)
 * Out: a0 = the owner (sign-extended handle), d4's low word = the owner's x, d0's low word = the
 *      owner's y + 24; then Op4_Kill's, or the script's
 * Evidence: movea.w $12(a6),a0 / move.w $3E(a0),d4 / move.w $42(a0),d0 / addi.w #$18,d0 / cmp.w d0,d5 /
 *           blt / jmp Op4_Kill / jsr ($53C6).w; native after Op4_MoveXY in Scr4_03D908; TAS
 *           f113672-f113706
 */
void Boss_Hatter_LittleHatFollow(uint32_t *d0, uint32_t d1, uint32_t *d2, uint32_t d3, uint32_t *d4, uint32_t d5,
                                 uint32_t d6, uint32_t d7, uint32_t *a0, uint32_t *a1, uint32_t a2, uint32_t a3,
                                 uint32_t a5, const LittleHat *hat)
{
    const Vm2Obj *owner = dk::handle<const Vm2Obj>(hat->owner);
    *a0 = dk::addr(owner);
    *d4 = (*d4 & 0xFFFF0000u) | (uint16_t)owner->x;
    uint16_t floor = (uint16_t)(owner->y + LITTLE_HAT_FALL);
    *d0 = (*d0 & 0xFFFF0000u) | floor;
    if ((int16_t)(uint16_t)d5 >= (int16_t)floor) {
        dk::jump(at_Op4_Kill);
        return;
    }
    dk::jump_script(at_VM4_Enter, Scr4_03D932);
}
