#include "game.h"

/* Item_CollectBomb - touch native of the bomb pickup: starts the 15-frame screen-clear pause and
 * deals $200 damage to every enemy box, credited to the collecting player.
 * In:  g_FoundBox = the toucher's box
 * Out: g_ScreenClearTimer = $F; damage = $200, attacker = the player's pad number, owner = the box
 *      owner (Item_BoxPlayer's), then Hitbox_DamageAllEnemies; not a player's box:
 *      Item_FoundBoxPlayer returns past this to the script op
 * Evidence: bsr Item_FoundBoxPlayer / move.w $3E(a0),d1 / move.w #$F,(g_ScreenClearTimer).w /
 *           move.w #$200,d0 / jmp Hitbox_DamageAllEnemies; op $2CB2 in the bomb's touch coroutine
 */
void Item_CollectBomb(uint16_t *damage, uint16_t *attacker, const void **owner)
{
    PlayerRec *p = 0;
    Item_FoundBoxPlayer(&p, owner);
    *attacker = p->pad;
    g_ScreenClearTimer = 0xF;
    *damage = 0x200;
    dk::jump(at_Hitbox_DamageAllEnemies);
}
