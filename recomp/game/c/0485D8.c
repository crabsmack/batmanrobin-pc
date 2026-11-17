#include "game.h"

/* the searchlight scene object's a2 script (ROM) */
static constexpr uint32_t Scr2_0485E4 = 0x0485E4u;

/* Story_SpawnSearchlightCamera - spawns the searchlight scene's object (Scr2_0485E4): ends the
 * scene after $168 frames, swings in along an x arc from -400, shakes the screen now and then and
 * draws the lamp's three sprites; its native Story_SearchlightScroll scrolls the planes by its x.
 * In:  x, y = the spawn position (Obj_SpawnEnemy's d4/d5)
 * Out: script = Scr2_0485E4; Obj_SpawnEnemy's results (a6 = the object, d0 = 0 / -1)
 * Evidence: lea Scr2_0485E4,a2 / jmp Obj_SpawnEnemy; Scr2_0485E4: Scene_EndAfterDelay $168, SetX /
 *           ArcX $FE70 $12C0, SetShakeTimer $14 every $280, PushPos / MovePos $10C $DF / Draw /
 *           PopPos; only caller Story_Searchlight; idle 2774 + 3925n
 */
void Story_SpawnSearchlightCamera(uint32_t *d0, uint16_t *d1, uint16_t x, uint16_t y, uint32_t *d7, uint32_t *script,
                                  uint32_t *a3, uint32_t *a4, uint32_t a6)
{
    *script = Scr2_0485E4;
    dk::jump(at_Obj_SpawnEnemy);
}
