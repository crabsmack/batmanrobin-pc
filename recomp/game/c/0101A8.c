#include "game.h"

/* Player_InitRecord - resets a player record for a new game or a join: score 0, full health
 * ($100), no weapon charge and no rapid charge left, and the starting lives, weapon type and
 * weapon level (set by Main_Start).
 * In:  record = the player record, g_Player1 or g_Player2 (a0)
 * Out: its score/health/weapon_charge/rapid_charge/lives/weapon_type/weapon_level
 * Evidence: move.w #$100,$4(a0) / clr.l 0(a0) / clr.w $8(a0) / clr.w $26(a0) / the three
 *           g_Start* copies to +6/+$28/+$2A; callers $11050 (game start) and Player_JoinOrSpawn
 *           (a credit join); TAS frame 477
 */
void Player_InitRecord(uint32_t record)
{
    PTR(PlayerRec) rec;
    rec.v = record;
    PlayerRec *p = rec;
    p->health = 0x100;
    p->score = 0;
    p->weapon_charge = 0;
    p->rapid_charge = 0;
    p->lives = (uint16_t)g_StartLives;
    p->weapon_type = (uint16_t)g_StartWeaponType;
    p->weapon_level = (uint16_t)g_StartWeaponLvl;
}
