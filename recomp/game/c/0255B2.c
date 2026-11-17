#include "game.h"

/* the thug's a2 script (ROM data right after this spawner's code; batmanrobin.asm) */
static constexpr uint32_t Scr2_0255C2 = 0x0255C2u;

/* Enemy_PoleThug_SpawnA - lift trigger callback: the pole-sliding thug variant that holds, slides and
 * shoots from the pole in a 3x3 counter loop
 * In:  x/y = where the trigger entry places it, a1 = the entry's words after the callback long
 *      (script variables 0/1: hold count, jump-off y)
 * Out: Z set, a6 = the thug, a1 past the two words; a short pool returns past this to the
 *      trigger loop (Obj_SpawnEnemyOrQuit)
 * Evidence: lea Scr2_0255C2,a2 / jsr Obj_SpawnEnemyOrQuit / jmp Stage2_SetSpawnVarsFromEntry;
 *           dc.l at $0214B8, $0214C4, $02156E, $02157A, $021810, $02181C in the lift's
 *           y-trigger table (x $28 / $118 = the left / right side pipe, var0 $7FFF, var1
 *           $AA/$64 = jump-off y); TAS f21360-f22492; png f21310: a thug on the left pipe
 */
void Enemy_PoleThug_SpawnA(uint16_t *d1, uint16_t x, uint16_t y, uint32_t *d7, uint32_t *a1, uint32_t *script, uint32_t *a3, uint32_t *a4, uint32_t *a6)
{
    /* Obj_Spawn sets a0 (the run list) and a5 itself: the values going in are not read */
    uint32_t d0 = 0, list = 0, sprite = 0;
    *script = Scr2_0255C2;
    Obj_SpawnEnemyOrQuit(&d0, d1, x, y, d7, &list, *script, a3, a4, &sprite, a6);
    dk::jump(at_Stage2_SetSpawnVarsFromEntry);   /* its a1 = the entry, a4 = the new thug's block */
}
