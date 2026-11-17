#include "game.h"

/* the ladder's a2 script (ROM data right after this spawner's code; batmanrobin.asm) */
static constexpr uint32_t Scr2_027872 = 0x027872u;

static constexpr uint16_t LADDER_BLOCKS = 6;   /* pool blocks: the object, its script blocks and the swing block */

/* Enemy_LadderThug_SpawnC - boss phase 1's spawner: the rope-ladder thug Scr2_027872 (throw set Scr4_027C28, body
 *      $026566) hung from the boss hull
 * In:  x/y = where it starts (d4/d5)
 * Out: d0 = 0 (Z set), a6 = the ladder, its swing block started (Enemy_LadderThug_InitLadder); a
 *      short pool returns past this to the spawning script (Obj_SpawnEnemyNOrQuit)
 * Evidence: lea Scr2_027872,a2 / move.w #6,d7 / jsr Obj_SpawnEnemyNOrQuit / jmp
 *           Enemy_LadderThug_InitLadder; op $2CC4 at $026DC0 and $026EDA in the
 *           boss's phase-1 attack scripts; not in the TAS, run at f34230 in the continue/game-over
 *           scenarios. The prototype gains &a2/&a3/&a4/&a6
 *           (measured clobbers: no caller reads them) so the jump hands the new object on
 */
void Enemy_LadderThug_SpawnC(uint32_t *d0, uint16_t *d1, uint16_t x, uint16_t y, uint16_t *d7, uint32_t *script, uint32_t *a3, uint32_t *a4, uint32_t *a6)
{
    /* Obj_Spawn sets a0 (the run list) and a5 itself: the values going in are not read */
    uint32_t list = 0, sprite = 0;
    *script = Scr2_027872;
    *d7 = LADDER_BLOCKS;
    Obj_SpawnEnemyNOrQuit(d0, d1, x, y, d7, &list, *script, a3, a4, &sprite, a6);
    dk::jump(at_Enemy_LadderThug_InitLadder);   /* jmp: its a6 = the new ladder */
}
