#include "game.h"

/* the tower scene object's a2 script (ROM) */
static constexpr uint32_t Scr2_0486DE = 0x0486DEu;

/* Story_SpawnTowerCamera - spawns the guard tower scene's object (Scr2_0486DE): starts 340 px up
 * and drops in a $960-tick arc, draws a 10-frame animation, then a shake, another animation and
 * the white fade-out end (Scene_EndAfterDelayWhite); its native Story_TowerScroll moves plane B.
 * In:  x, y = the spawn position (Obj_SpawnEnemy's d4/d5)
 * Out: script = Scr2_0486DE; Obj_SpawnEnemy's results (a6 = the object, d0 = 0 / -1)
 * Evidence: lea Scr2_0486DE,a2 / jmp Obj_SpawnEnemy; Scr2_0486DE: SetPos 0 $FEAC, JumpAnim $FEAC
 *           $960, MovePos $D9 $4B, SetTimer0 $5A0, SetShakeTimer 2, CallNative
 *           Scene_EndAfterDelayWhite; only caller Story_GuardTower; idle 3165 + 3925n
 */
void Story_SpawnTowerCamera(uint32_t *d0, uint16_t *d1, uint16_t x, uint16_t y, uint32_t *d7, uint32_t *script,
                            uint32_t *a3, uint32_t *a4, uint32_t a6)
{
    *script = Scr2_0486DE;
    dk::jump(at_Obj_SpawnEnemy);
}
