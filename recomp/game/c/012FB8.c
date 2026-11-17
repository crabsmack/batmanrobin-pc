#include "game.h"

/* Item_FoundBoxPlayer - pickup helper: the player record of whoever owns g_FoundBox (the box the
 * pickup touched); not a player's box: returns to the caller's caller (Item_BoxPlayer).
 * In:  g_FoundBox
 * Out: box = g_FoundBox, then Item_BoxPlayer: player = g_Player1 or g_Player2, box = its owner
 * Evidence: movea.l (g_FoundBox).w,a1, then falls into Item_BoxPlayer (the next function); the first
 *           call of the item-collect natives $013440, $0135F4, $01375E, $0138D8, $013C8A
 */
void Item_FoundBoxPlayer(PlayerRec **player, const void **box)
{
    PTR(const void) found;
    found.v = (uint32_t)g_FoundBox;
    *box = found;
    dk::jump(at_Item_BoxPlayer);
}
