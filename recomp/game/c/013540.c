#include "game.h"

/* Item_CollectSmallHealth - touch native of the small health pickup: the collecting player gets $10
 * health (1/16 of the bar), capped at $100. A word over $7FFF after the add counts as negative in the
 * signed compare and is kept (the original's ble), as Item_CollectBigHealth's.
 * In:  g_FoundBox = the box that touched the pickup
 * Out: the player's health; the box owner (a1, Item_BoxPlayer's); not a player's box:
 *      Item_FoundBoxPlayer returns past this to the script op
 * Evidence: bsr Item_FoundBoxPlayer / addi.w #$10,$4(a0) / cmpi.w #$100 / ble / move.w #$100; op
 *           $2CB2 at $01353A in the small pickup's body $1351E right after sound $12; no corpus run
 *           collects one (crane-crash 2026-10-30); 013540.syn checks it
 */
const void *Item_CollectSmallHealth(void)
{
    PlayerRec *p = 0;
    const void *owner = 0;
    Item_FoundBoxPlayer(&p, &owner);
    p->health += 0x10;
    if ((int16_t)p->health > 0x100)
        p->health = 0x100;
    return owner;
}
