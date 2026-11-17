#include "game.h"

/* the title director's a2 script (ROM) */
static constexpr uint32_t Scr2_0496D8 = 0x0496D8u;

/* Title_SpawnMenu - spawns the title director (script Scr2_0496D8): a 500-frame idle timeout, then
 * Start opens the START / OPTIONS menu; START ends the scene with result 1, OPTIONS runs the
 * options page until Start.
 * In:  x, y = the spawn position (Obj_SpawnEnemy's d4/d5)
 * Out: script = the director's script; Obj_SpawnEnemy's results (a6 = the object, d0 = 0 / -1)
 * Evidence: lea Scr2_0496D8,a2 / jmp Obj_SpawnEnemy; Scr2_0496D8 = Scene_EndAfterDelay 0/$1F4,
 *           coroutine $49708 branching on Start (var 0 bit 7); TAS f404
 */
void Title_SpawnMenu(uint16_t *d1, uint16_t x, uint16_t y, uint32_t *script, uint32_t *a3, uint32_t *a4, uint32_t *a6)
{
    *script = Scr2_0496D8;
    dk::jump(at_Obj_SpawnEnemy);
}
