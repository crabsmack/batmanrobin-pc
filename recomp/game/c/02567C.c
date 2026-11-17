#include "game.h"

/* the thug's a2 script (ROM data right after this spawner's code; batmanrobin.asm) */
static constexpr uint32_t Scr2_02568C = 0x02568Cu;

/* Enemy_PoleThug_SpawnC - lift trigger callback: the pole-sliding thug variant that never shoots from the
 * pole: holds $780 frames, slides, repeats until it reaches its jump-off y
 * In:  x/y = where the trigger entry places it, a1 = the entry's words after the callback long
 *      (script variables 0/1: hold count, jump-off y)
 * Out: Z set, a6 = the thug, a1 past the two words; a short pool returns past this to the
 *      trigger loop (Obj_SpawnEnemyOrQuit)
 * Evidence: lea Scr2_02568C,a2 / jsr Obj_SpawnEnemyOrQuit / jmp Stage2_SetSpawnVarsFromEntry;
 *           20 dc.l uses $02159E-$021BFC in the lift trigger table (the most common
 *           variant); body $02577E; TAS f21822-f29613 (21 frames)
 */
void Enemy_PoleThug_SpawnC(uint16_t *d1, uint16_t x, uint16_t y, uint32_t *d7, uint32_t *a1, uint32_t *script, uint32_t *a3, uint32_t *a4, uint32_t *a6)
{
    /* Obj_Spawn sets a0 (the run list) and a5 itself: the values going in are not read */
    uint32_t d0 = 0, list = 0, sprite = 0;
    *script = Scr2_02568C;
    Obj_SpawnEnemyOrQuit(&d0, d1, x, y, d7, &list, *script, a3, a4, &sprite, a6);
    dk::jump(at_Stage2_SetSpawnVarsFromEntry);   /* its a1 = the entry, a4 = the new thug's block */
}
