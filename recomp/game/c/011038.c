#include "game.h"

/* Player_NewGameSetup - starts a new game's player state: the boot settings become the game's
 * (g_UnkGame_F634, the credits, the enemy and boss energy scales), then Player_InitRecords.
 * In:  none
 * Out: as Player_InitRecords (a1 = g_Player2)
 * Evidence: four move.w (g_X).w,(g_Y).w and the fall into Player_InitRecords; one caller, the
 *           title flow at $9EFC
 */
uint32_t Player_NewGameSetup(void)
{
    g_UnkGame_F634 = g_UnkStart_F610;
    g_Credits = g_StartCredits;
    g_EnemyEnergyScale = g_StartEnemyScale;
    g_BossEnergyScale = g_StartBossScale;
    dk::jump(at_Player_InitRecords);
    return 0;   /* a1 is Player_InitRecords' to set */
}
