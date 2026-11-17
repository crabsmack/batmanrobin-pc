#include "game.h"

/* the spawned object's a2 script (ROM data right after this spawner's code; batmanrobin.asm) */
static constexpr uint32_t Scr2_02788E = 0x02788Eu;
static constexpr uint16_t LADDER_BLOCKS = 6;   /* pool blocks: the object, its script blocks and the swing block */

/* Enemy_LadderThug_SpawnD - boss-phase-1 spawner: a rope-ladder thug (Scr2_02788E: throw set
 * $027C40, body $026566) hung from the boss hull
 * In:  d4/d5 = x/y (the boss's anchor)
 * Out: script = Scr2_02788E, then Obj_SpawnEnemyNOrQuit's results, then
 *      Enemy_LadderThug_InitLadder's
 * Evidence: spawner long(s) at Op2_CallNativeRelSaved at $026E72, $026EE4, $026F3C ... (7); lea
 *           script / move.w #6,d7 / jsr Obj_SpawnEnemyNOrQuit / jmp Enemy_LadderThug_InitLadder(pc)
 *           as Enemy_LadderThug_SpawnC; raw until crane-crash (2026-10-30): no corpus run spawns
 *           it; 02787A.syn checks it
 */
void Enemy_LadderThug_SpawnD(uint32_t *d0, uint16_t *d1, uint16_t x, uint16_t y, uint16_t *d7, uint32_t *script, uint32_t *a3, uint32_t *a4, uint32_t *a6)
{
    /* Obj_Spawn sets a0 (the run list) and a5 itself: the values going in are not read */
    uint32_t list = 0, sprite = 0;
    *script = Scr2_02788E;
    *d7 = LADDER_BLOCKS;
    Obj_SpawnEnemyNOrQuit(d0, d1, x, y, d7, &list, *script, a3, a4, &sprite, a6);
    dk::jump(at_Enemy_LadderThug_InitLadder);   /* jmp: its a6 = the new ladder */
}
