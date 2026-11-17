#include "game.h"

/* Item_CollectBigHealth - touch native of the large health pickup: the collecting player gets $80
 * health (half the bar), capped at $100. A word over $7FFF after the add counts as negative in the
 * signed compare and is kept (the original's ble).
 * In:  g_FoundBox = the box that touched the pickup
 * Out: the player's health; the box owner (a1, Item_BoxPlayer's); not a player's box:
 *      Item_FoundBoxPlayer returns past this to the script op
 * Evidence: bsr Item_FoundBoxPlayer / addi.w #$80,$4(a0) / cmpi.w #$100 / ble / move.w #$100; op
 *           $2CB2 at $1343A right after sound $41; 7 TAS frames (7565-71935)
 */
const void *Item_CollectBigHealth(void)
{
    PlayerRec *p = 0;
    const void *owner = 0;
    Item_FoundBoxPlayer(&p, &owner);
    p->health += 0x80;
    if ((int16_t)p->health > 0x100)
        p->health = 0x100;
    return owner;
}
