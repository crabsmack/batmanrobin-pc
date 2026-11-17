#include "game.h"

/* the GAME OVER text object's a2 script (ROM) */
static constexpr uint32_t Scr2_04963C = 0x04963Cu;

/* GameOver_SpawnText - spawns the GAME OVER sprite text (Scr2_04963C): palette line 3, position
 * ($5A, $FF), a jump-animated drop into place, then each frame it draws 'GAME OVER' and keeps
 * pace with the scrolling camera (GameOver_TextKeepPace).
 * In:  x, y = the spawn position (Obj_SpawnEnemy's d4/d5)
 * Out: script = Scr2_04963C; Obj_SpawnEnemy's results (a6 = the object, d0 = 0 / -1)
 * Evidence: lea Scr2_04963C,a2 / jmp Obj_SpawnEnemy; Scr2_04963C: Op2_LoadPalette3, Op2_SetPos
 *           $5A/$FF, Op2_JumpAnim, $4965A, Op2_ApplyMove, CallNative Text_DrawAtObject $14088;
 *           only caller GameOver_Run; png scenario-game-over f43200
 */
void GameOver_SpawnText(uint32_t *d0, uint16_t *d1, uint16_t x, uint16_t y, uint32_t *d7, uint32_t *script,
                        uint32_t *a3, uint32_t *a4, uint32_t a6)
{
    *script = Scr2_04963C;
    dk::jump(at_Obj_SpawnEnemy);
}
