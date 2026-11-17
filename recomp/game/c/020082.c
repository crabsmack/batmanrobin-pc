#include "game.h"

/* the launcher's a2 script (ROM, right after this spawner's code) */
static constexpr uint32_t Scr2_020094 = 0x020094u;

/* Stage1_SpawnBalloonLauncherB - second balloon-bomb launcher of the stage 1 parade-truck section:
 * spawns Scr2_020094 at the right screen edge and points it at height table A
 * In:  d4/d5 = x/y; the wave director's registers (wave script Tbl_0174F0)
 * Out: script (a2) = Scr2_020094; a6 = the launcher, then as Stage1_BalloonUseHeightsA (its
 *      heights = Tbl_020354, d0 = 0, Z set). On a full pool Obj_SpawnEnemyOrQuit returns to this
 *      spawner's caller itself
 * Evidence: lea Scr2_020094,a2 / jsr Obj_SpawnEnemyOrQuit / jmp Stage1_BalloonUseHeightsA; dc.l at
 *           $017DDA in Tbl_0174F0; TAS f16330 (camera x 3190: launcher at x 3515, y 170, balloons
 *           Scr2_0203E0 around it at f16380)
 */
void Stage1_SpawnBalloonLauncherB(uint16_t *d1, uint16_t d4, uint16_t d5, uint32_t *d7, uint32_t *script,
                                  uint32_t *a3, uint32_t *a4, uint32_t *a6)
{
    *script = Scr2_020094;
    uint32_t d0 = 0, a0 = 0, a5 = 0;  /* clobbered */
    Obj_SpawnEnemyOrQuit(&d0, d1, d4, d5, d7, &a0, *script, a3, a4, &a5, a6);
    dk::jump(at_Stage1_BalloonUseHeightsA);
}
