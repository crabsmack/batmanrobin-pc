#include "game.h"

/* the sprite attribute bit for a mirrored (H-flipped) sprite: bit 3 of the attribute's high byte */
static constexpr uint16_t HARLEYJET_ATTR_FLIP_H = 0x0800;

/* Boss_HarleyJet_IsFlipped - jetpack Harley script test: 1 when her sprite is drawn H-flipped, else 0
 * In:  spr = her sprite block (a5)
 * Out: d0 = 0 / 1 (for a native switch op)
 * Evidence: btst #3,$38(a5) / bne / moveq #0,d0 / rts / moveq #1,d0 / rts; op $2CB2/$2D30 operands at
 *           $1F09C/$1F0A4 in Scr2_01EFCE; TAS f12385-f12627 (6 frames); like Boss_Harley_IsPartFlipped
 */
uint32_t Boss_HarleyJet_IsFlipped(const Vm2Sprite *spr)
{
    return (spr->attr & HARLEYJET_ATTR_FLIP_H) ? 1 : 0;
}
