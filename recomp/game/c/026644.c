#include "game.h"

/* the hull-hung ladder's script (the bytes after this native's jsr $7FE.w): starts at x $100,
 * y -$F0 in hull space, then the hull-tilted loop $026654 */
static constexpr uint32_t Scr2_026648 = 0x026648u;

/* Enemy_LadderThug_AnchorHull - the hull exit of Enemy_LadderThug_PickAnchor: the ladder hangs
 * from the boss hull instead of the shaft top
 * In:  the running ladder object's registers (a2 = its script)
 * Out: the script continues at Scr2_026648 (jsr $7FE.w)
 * Evidence: bne target of Enemy_LadderThug_PickAnchor; jsr $7FE.w; TAS f33179-f34742 (3 frames:
 *           ladders under the boss hull, png f033300)
 */
void Enemy_LadderThug_AnchorHull(uint32_t d0, uint32_t d1, uint32_t d2, uint32_t d3, uint32_t d4, uint32_t d5, uint32_t d6,
                                 uint32_t d7, uint32_t a1, uint32_t a3, uint32_t a4, uint32_t a5, uint32_t a6)
{
    dk::jump_script(at_VM2_Enter, Scr2_026648);
}
