#include "game.h"

/* the thug's a2 script (ROM data right after this spawner's code; batmanrobin.asm) */
static constexpr uint32_t Scr2_025630 = 0x025630u;

/* Enemy_PoleThug_SpawnB - lift trigger callback: the pole-sliding thug variant with a random first hold
 * ($78 + [0,$1E0)), then slide, shoot ($0258E2) and hold in $1E0-frame steps
 * In:  x/y = where the trigger entry places it, a1 = the entry's words after the callback long
 *      (script variables 0/1: hold count, jump-off y)
 * Out: Z set, a6 = the thug, a1 past the two words; a short pool returns past this to the
 *      trigger loop (Obj_SpawnEnemyOrQuit)
 * Evidence: lea Scr2_025630,a2 / jsr Obj_SpawnEnemyOrQuit / jmp Stage2_SetSpawnVarsFromEntry;
 *           dc.l at $021586, $021592, $0217A4, $0217E0, $021A96, $021AD2; body $02577E;
 *           TAS f22020-f22092
 */
void Enemy_PoleThug_SpawnB(uint16_t *d1, uint16_t x, uint16_t y, uint32_t *d7, uint32_t *a1, uint32_t *script, uint32_t *a3, uint32_t *a4, uint32_t *a6)
{
    /* Obj_Spawn sets a0 (the run list) and a5 itself: the values going in are not read */
    uint32_t d0 = 0, list = 0, sprite = 0;
    *script = Scr2_025630;
    Obj_SpawnEnemyOrQuit(&d0, d1, x, y, d7, &list, *script, a3, a4, &sprite, a6);
    dk::jump(at_Stage2_SetSpawnVarsFromEntry);   /* its a1 = the entry, a4 = the new thug's block */
}
