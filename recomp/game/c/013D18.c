#include "game.h"

enum { WEAPON_LEVEL_MAX = 6 };

/* Item_CollectWeaponLevelUp - touch native of the power-ups: the collecting player's weapon level
 * goes up by one, not past 6. Not a player's box: Item_FoundBoxPlayer returns past this to the
 * script op.
 * In:  g_FoundBox = the toucher's box; ccr's X (cmpi keeps it)
 * Out: player_at = the player record, owner_at = the box owner (Item_BoxPlayer's); the record's
 *      weapon_level; ccr = the cmpi.w #6 (at the cap) or the addq.w #1 that raised it
 * Evidence: bsr Item_FoundBoxPlayer / cmpi.w #6,$2A(a0) / beq / addq.w #1,$2A(a0) / rts; the native
 *           before the a2 script at $13D2A (the power-ups' touch coroutine $13C4E); NOTES Weapons:
 *           level 0-6
 */
void Item_CollectWeaponLevelUp(uint32_t *player_at, uint32_t *owner_at, dk::Flags *ccr)
{
    bool x = ccr->x;
    PlayerRec *p = nullptr;                  /* a0 / a1 stay plain registers: their inputs may be no address */
    const void *owner = nullptr;
    Item_FoundBoxPlayer(&p, &owner);
    *player_at = dk::addr(p);
    *owner_at = dk::addr(owner);
    uint16_t level = p->weapon_level;
    *ccr = dk::Flags::compare(level, WEAPON_LEVEL_MAX, 2, x);
    if (ccr->z)
        return;
    uint16_t up = (uint16_t)(level + 1);
    p->weapon_level = up;
    bool carry = level == 0xFFFF;
    *ccr = dk::Flags{ carry, (up & 0x8000) != 0, up == 0, level == 0x7FFF, carry };
}
