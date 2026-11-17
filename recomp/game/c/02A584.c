#include "game.h"

/* the spawned object's a2 script (ROM data right after this spawner's code; batmanrobin.asm) */
static constexpr uint32_t Scr2_02A594 = 0x02A594u;

/* Enemy_BlackDrone_SpawnWaypointsH - wave spawner: WaypointsG mirrored vertically (Scr2_02A594):
 * (-$28, 0) at cruise $2AA, (-$28, +$32), (-5, -6), then (+$A0, -$258) off the screen
 * In:  d4/d5 = x/y (the wave entry's), a1 = the entry
 * Out: script = Scr2_02A594, then Obj_SpawnEnemyOrQuit's results (a short pool returns from the
 *      caller), then the tail's
 * Evidence: spawner long(s) at $022272, $022C02, $022C0C; lea script / jsr Obj_SpawnEnemyOrQuit /
 *           jmp Spawn_SetTimer1FromEntry(pc) as its siblings; raw until crane-crash (2026-10-30):
 *           no corpus run spawns it; 02A584.syn checks it
 */
void Enemy_BlackDrone_SpawnWaypointsH(uint16_t *d1, uint16_t x, uint16_t y, uint32_t *d7, uint32_t *a1, uint32_t *script, uint32_t *a3, uint32_t *a4, uint32_t *a6)
{
    /* Obj_Spawn sets a0 (the run list) and a5 itself: the values going in are not read */
    uint32_t d0 = 0, list = 0, sprite = 0;
    *script = Scr2_02A594;
    Obj_SpawnEnemyOrQuit(&d0, d1, x, y, d7, &list, *script, a3, a4, &sprite, a6);
    dk::jump(at_Spawn_SetTimer1FromEntry);   /* its a1 = the entry, a4 = the new object's block */
}
