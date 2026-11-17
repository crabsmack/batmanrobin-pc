#include "game.h"

/* the launcher's a2 script (ROM, right after this spawner's code) */
static constexpr uint32_t Scr2_01FFB8 = 0x01FFB8u;

/* Stage1_SpawnBalloonLauncherA - wave-script spawner of the stage 1 parade-truck section: spawns the
 * balloon-bomb launcher Scr2_01FFB8 (an unseen emitter at the right screen edge) and points it at
 * height table B
 * In:  d4/d5 = x/y; the wave director's registers (the a5-VM wave script Tbl_0174F0 calls it)
 * Out: script (a2) = Scr2_01FFB8; a6 = the launcher, then as Stage1_BalloonUseHeightsB (its
 *      heights = Tbl_02036E, d0 = 0, Z set). On a full pool Obj_SpawnEnemyOrQuit returns to this
 *      spawner's caller itself
 * Evidence: lea Scr2_01FFB8,a2 / jsr Obj_SpawnEnemyOrQuit / jmp Stage1_BalloonUseHeightsB; dc.l at
 *           $017C3C in Tbl_0174F0; TAS f13756 (scene $1FECC): the object sits at screen x 325, y 162
 *           while balloons rise from the car on the right (shot f13790)
 */
void Stage1_SpawnBalloonLauncherA(uint16_t *d1, uint16_t d4, uint16_t d5, uint32_t *d7, uint32_t *script,
                                  uint32_t *a3, uint32_t *a4, uint32_t *a6)
{
    *script = Scr2_01FFB8;
    uint32_t d0 = 0, a0 = 0, a5 = 0;  /* clobbered */
    Obj_SpawnEnemyOrQuit(&d0, d1, d4, d5, d7, &a0, *script, a3, a4, &a5, a6);
    dk::jump(at_Stage1_BalloonUseHeightsB);
}
