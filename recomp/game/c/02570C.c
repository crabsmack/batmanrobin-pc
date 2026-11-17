#include "game.h"

/* the spawned object's a2 script (ROM data right after this spawner's code; batmanrobin.asm) */
static constexpr uint32_t Scr2_02571C = 0x02571Cu;

/* Enemy_PoleThug_SpawnE - lift trigger callback: spawns a pole-sliding thug variant (Scr2_02571C:
 * energy $30/$C0, timers $1E0, $140, $280 x2 loops, $258, $50) on the shared body, then script vars
 * 0/1 from the entry
 * In:  d4/d5 = x/y (the wave entry's), a1 = the entry
 * Out: script = Scr2_02571C, then Obj_SpawnEnemyOrQuit's results (a short pool returns from the
 *      caller), then the tail's
 * Evidence: spawner long(s) at $0217C8, $021804, $021ABA, $021AF6; lea script / jsr
 *           Obj_SpawnEnemyOrQuit / jmp Stage2_SetSpawnVarsFromEntry(pc) as its siblings; raw until
 *           crane-crash (2026-10-30): no corpus run spawns it; 02570C.syn checks it
 */
void Enemy_PoleThug_SpawnE(uint16_t *d1, uint16_t x, uint16_t y, uint32_t *d7, uint32_t *a1, uint32_t *script, uint32_t *a3, uint32_t *a4, uint32_t *a6)
{
    /* Obj_Spawn sets a0 (the run list) and a5 itself: the values going in are not read */
    uint32_t d0 = 0, list = 0, sprite = 0;
    *script = Scr2_02571C;
    Obj_SpawnEnemyOrQuit(&d0, d1, x, y, d7, &list, *script, a3, a4, &sprite, a6);
    dk::jump(at_Stage2_SetSpawnVarsFromEntry);   /* its a1 = the entry, a4 = the new thug's block */
}
