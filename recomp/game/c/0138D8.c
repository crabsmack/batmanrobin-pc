#include "game.h"

/* Item_CollectRapidCharge - touch native of the rapid-charge pickup: the collecting player's weapon
 * charges fast for the next $2D0 frames (12 s; restarted, not added, by a second pickup).
 * In:  g_FoundBox = the toucher's box
 * Out: the player's rapid_charge = $2D0; the box owner (a1, Item_BoxPlayer's); not a player's box:
 *      Item_FoundBoxPlayer returns past this to the script op
 * Evidence: bsr Item_FoundBoxPlayer / move.w #$2D0,$26(a0); $103A6 counts +$26 down a frame and
 *           charges by $280 while it runs (NOTES Pickups)
 */
const void *Item_CollectRapidCharge(void)
{
    PlayerRec *p = 0;
    const void *owner = 0;
    Item_FoundBoxPlayer(&p, &owner);
    p->rapid_charge = 0x2D0;
    return owner;
}
