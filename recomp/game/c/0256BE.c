#include "game.h"

/* the thug's a2 script (ROM data right after this spawner's code; batmanrobin.asm) */
static constexpr uint32_t Scr2_0256CE = 0x0256CEu;

/* Enemy_PoleThug_SpawnD - lift trigger callback: the pole-sliding thug variant that holds $1E0, slides,
 * then jumps off the pole at once ($02592E) wherever it is
 * In:  x/y = where the trigger entry places it, a1 = the entry's words after the callback long
 *      (script variables 0/1: hold count, jump-off y)
 * Out: Z set, a6 = the thug, a1 past the two words; a short pool returns past this to the
 *      trigger loop (Obj_SpawnEnemyOrQuit)
 * Evidence: lea Scr2_0256CE,a2 / jsr Obj_SpawnEnemyOrQuit / jmp Stage2_SetSpawnVarsFromEntry;
 *           18 dc.l uses $0214D0-$021D1E; body $02577E; TAS f21270-f29037; RAM f21300: an
 *           object at x 32, y 18 with var1 $87
 */
void Enemy_PoleThug_SpawnD(uint16_t *d1, uint16_t x, uint16_t y, uint32_t *d7, uint32_t *a1, uint32_t *script, uint32_t *a3, uint32_t *a4, uint32_t *a6)
{
    /* Obj_Spawn sets a0 (the run list) and a5 itself: the values going in are not read */
    uint32_t d0 = 0, list = 0, sprite = 0;
    *script = Scr2_0256CE;
    Obj_SpawnEnemyOrQuit(&d0, d1, x, y, d7, &list, *script, a3, a4, &sprite, a6);
    dk::jump(at_Stage2_SetSpawnVarsFromEntry);   /* its a1 = the entry, a4 = the new thug's block */
}
