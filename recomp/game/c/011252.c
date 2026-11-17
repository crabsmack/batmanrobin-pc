#include "game.h"

/* one value into a max/min/sum triple (signed word compares: ble / bge) */
static void fold(int16_t v, u16 &max, u16 &min, u16 &sum)
{
    sum += (uint16_t)v;
    if (v > (int16_t)(uint16_t)max)
        max = (uint16_t)v;
    if (v < (int16_t)(uint16_t)min)
        min = (uint16_t)v;
}

/* Player_AddStats - adds one player record to the per-frame aggregates when it is in play: the
 * count, weapon level, health / 16 (a negative health counts as 0) and lives; a player out of play
 * has its +$36/+$38/+$3A latch fields cleared instead.
 * In:  rec = the record (a0); d0 = the caller's (its upper word survives the word moves)
 * Out: d0 = the lives (low word) when in play, else unchanged
 * Evidence: btst #0,$25(a0) / beq / addq.w #1,(g_PlayersInPlay).w / move.w $2A(a0),d0 / add, cmp,
 *           ble, cmp, bge for each triple / move.w $4(a0),d0 / bpl / moveq #0,d0 / lsr.w #4,d0 /
 *           move.w $6(a0),d0; out of play clr.b $3A / clr.w $38 / clr.w $36
 */
void Player_AddStats(uint32_t *d0, PlayerRec *rec)
{
    if (!(rec->flags & PLAYER_IN_PLAY)) {
        rec->targetable = 0;
        rec->attackers = 0;
        rec->attackers_last = 0;
        return;
    }
    g_PlayersInPlay += 1;
    uint16_t level = rec->weapon_level;
    fold((int16_t)level, g_WeaponLvlMax, g_WeaponLvlMin, g_WeaponLvlSum);
    uint32_t h = (*d0 & 0xFFFF0000u) | (uint16_t)rec->health;
    if ((int16_t)(uint16_t)rec->health < 0)
        h = 0;
    uint16_t health16 = (uint16_t)((uint16_t)h >> 4);
    fold((int16_t)health16, g_HealthMax, g_HealthMin, g_HealthSum);
    *d0 = (h & 0xFFFF0000u) | (uint16_t)rec->lives;
    fold((int16_t)(uint16_t)rec->lives, g_LivesMax, g_LivesMin, g_LivesSum);
}
