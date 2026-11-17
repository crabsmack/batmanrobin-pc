#include "game.h"

/* Boss_Hatter_LittleHatSetOwner - a4 native ending the little hat's spawn script: stores its owner
 * (the boss left in g_ArcTargetX by Boss_Hatter_PopLittleHat) in the child just forked
 * In:  hat = the child (a5)
 * Out: hat->owner; the condition codes of the move.w (N = a RAM handle's sign: set)
 * Evidence: move.w (g_ArcTargetX).w,$12(a5) / rts at the end of Scr4_03D8EC (op $53C4); TAS f113672
 */
void Boss_Hatter_LittleHatSetOwner(LittleHat *hat, dk::Flags *ccr)
{
    uint16_t owner = g_ArcTargetX;
    hat->owner = (int16_t)owner;
    *ccr = dk::Flags::test(owner, 2, ccr->x);
}
