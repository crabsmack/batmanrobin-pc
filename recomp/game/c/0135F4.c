#include "game.h"

/* Item_CollectExtraLife - touch native of the extra-life pickup: one more life for the collecting
 * player (no cap).
 * In:  g_FoundBox = the toucher's box
 * Out: the player's lives + 1; the box owner (a1, Item_BoxPlayer's); not a player's box:
 *      Item_FoundBoxPlayer returns past this to the script op
 * Evidence: bsr Item_FoundBoxPlayer / addq.w #1,$6(a0); op $2CB2 at $135EA after sound $42;
 *           4 TAS frames (31540-115798)
 */
const void *Item_CollectExtraLife(void)
{
    PlayerRec *p = 0;
    const void *owner = 0;
    Item_FoundBoxPlayer(&p, &owner);
    p->lives += 1;
    return owner;
}
