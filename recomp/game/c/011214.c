#include "game.h"

/* Player_GatherStats - recomputes the per-frame aggregates over the players in play: their count,
 * and the max/min/sum of weapon level, health/16 and lives (the minimums start at $7FFF).
 * In:  g_Player1, g_Player2
 * Out: d0 as Player_AddStats left it (from moveq #0); the aggregate globals
 * Evidence: moveq #0,d0 / move.w #$7FFF,d1 / ten move.w to g_PlayersInPlay..g_LivesSum / bsr
 *           Player_AddStats for g_Player1 / lea g_Player2,a0 and the fall into Player_AddStats
 *           (a jump: a0, a clobber, is an output here so the jump hands it on); first call of HUD_Update
 */
uint32_t Player_GatherStats(uint32_t *a0)
{
    g_PlayersInPlay = 0;
    g_WeaponLvlMax = 0;
    g_WeaponLvlMin = 0x7FFF;
    g_WeaponLvlSum = 0;
    g_HealthMax = 0;
    g_HealthMin = 0x7FFF;
    g_HealthSum = 0;
    g_LivesMax = 0;
    g_LivesMin = 0x7FFF;
    g_LivesSum = 0;
    uint32_t d0 = 0;
    Player_AddStats(&d0, (PlayerRec *)g_Player1);
    *a0 = dk::addr(g_Player2);
    dk::jump(at_Player_AddStats);
    return d0;
}
