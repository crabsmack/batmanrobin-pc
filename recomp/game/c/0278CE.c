#include "game.h"

/* the ladder's a2 script (ROM data right after this spawner's code; batmanrobin.asm) */
static constexpr uint32_t Scr4_0278E2 = 0x0278E2u;

static constexpr uint16_t LADDER_BLOCKS = 6;   /* pool blocks: the object, its script blocks and the swing block */

/* Enemy_LadderThug_SpawnB - spawner: the rope-ladder thug Scr4_0278E2 (an a2 script: throw set $0278EA:
 *      one of 6 bomb arcs, body $02653E)
 * In:  x/y = where it starts (d4/d5)
 * Out: d0 = 0 (Z set), a6 = the ladder, its swing block started (Enemy_LadderThug_InitLadder); a
 *      short pool returns past this to the spawning script (Obj_SpawnEnemyNOrQuit)
 * Evidence: lea Scr4_0278E2,a2 / move.w #6,d7 / jsr Obj_SpawnEnemyNOrQuit / jmp
 *           Enemy_LadderThug_InitLadder; dc.l at $021BD0 / $021CB2 and op $2CC4 at
 *           $0273E6 (boss phase 2); TAS f33599 (one spawn)
 */
void Enemy_LadderThug_SpawnB(uint32_t *d0, uint16_t *d1, uint16_t x, uint16_t y, uint16_t *d7, uint32_t *script, uint32_t *a3, uint32_t *a4, uint32_t *a6)
{
    /* Obj_Spawn sets a0 (the run list) and a5 itself: the values going in are not read */
    uint32_t list = 0, sprite = 0;
    *script = Scr4_0278E2;
    *d7 = LADDER_BLOCKS;
    Obj_SpawnEnemyNOrQuit(d0, d1, x, y, d7, &list, *script, a3, a4, &sprite, a6);
    dk::jump(at_Enemy_LadderThug_InitLadder);   /* jmp: its a6 = the new ladder */
}
