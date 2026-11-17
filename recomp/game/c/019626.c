#include "game.h"

/* the sprite attribute bit for a mirrored (H-flipped) sprite: bit 3 of the attribute's high byte */
static constexpr uint16_t HARLEY_ATTR_FLIP_H = 0x0800;

/* Boss_Harley_IsPartFlipped - script test of the Harley boss: returns 1 when its second sprite
 * block (Harley on the pole) is drawn H-flipped, else 0
 * In:  self = the boss object (a6)
 * Out: d0 = 0 / 1 (for a native switch op)
 * Evidence: movea.w $E(a6),a0 / btst #3,$38(a0) / bne / moveq #0,d0 / rts / moveq #1,d0 / rts;
 *           entered from the boss scripts ($19474-) during the pole phase, TAS f5755-f5949 and 2
 *           fuzz runs
 */
uint32_t Boss_Harley_IsPartFlipped(const HarleyObj *self)
{
    const Vm2Sprite *part = dk::handle<const Vm2Sprite>(self->part_sprite);
    return (part->attr & HARLEY_ATTR_FLIP_H) ? 1 : 0;
}
