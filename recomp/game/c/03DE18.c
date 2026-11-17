#include "game.h"

/* the road hazard's body script after the native's jsr $7FE.w (VM2_Enter) */
static constexpr uint32_t Scr2_03DE22 = 0x03DE22u;

/* Boss_Hatter_RoadHazardInit - a2 native fragment that starts the road hazard's body: one sprite
 * frame slot in use, then the body script (the place/attack/kill loop) goes on
 * In:  sprite = the hazard's sprite block (a5)
 * Out: sprite->frame_count = 1, then the script at Scr2_03DE22
 * Evidence: move.w #1,$14(a5) / jsr ($7FE).w; Op2_Goto from Scr2_03FB1E to $03DE16; TAS
 *           f114753/f114833
 */
void Boss_Hatter_RoadHazardInit(uint32_t d0, uint32_t d1, uint32_t d2, uint32_t d3, uint32_t d4, uint32_t d5,
                                uint32_t d6, uint32_t d7, uint32_t a1, uint32_t a3, uint32_t a4, Vm2Sprite *sprite,
                                uint32_t a6)
{
    sprite->frame_count = 1;
    dk::jump_script(at_VM2_Enter, Scr2_03DE22);
}
