#include "game.h"

/* the spawned object's a2 script (ROM data right after this spawner's code; batmanrobin.asm) */
static constexpr uint32_t Scr2_02B7E6 = 0x02B7E6u;

/* Enemy_RedDrone_SpawnHoverRight - wave spawner: Enemy_RedDrone_SpawnHoverOnPlayer's red drone
 * (Scr2_02B7E6, random speed) holding its $1E00-tick window right of the targeted player (x
 * +$B4..+$C8, y -$A..+$A), not on it
 * In:  d4/d5 = x/y (the wave entry's), a1 = the entry
 * Out: script = Scr2_02B7E6, then Obj_SpawnEnemyOrQuit's results (a short pool returns from the
 *      caller), then the tail's
 * Evidence: spawner long(s) at $0220B0, $02225E; lea script / jsr Obj_SpawnEnemyOrQuit / jmp
 *           Spawn_SetTimer1FromEntry(pc) as its siblings; raw until crane-crash (2026-10-30): no
 *           corpus run spawns it; 02B7D6.syn checks it
 */
void Enemy_RedDrone_SpawnHoverRight(uint16_t *d1, uint16_t x, uint16_t y, uint32_t *d7, uint32_t *a1, uint32_t *script, uint32_t *a3, uint32_t *a4, uint32_t *a6)
{
    /* Obj_Spawn sets a0 (the run list) and a5 itself: the values going in are not read */
    uint32_t d0 = 0, list = 0, sprite = 0;
    *script = Scr2_02B7E6;
    Obj_SpawnEnemyOrQuit(&d0, d1, x, y, d7, &list, *script, a3, a4, &sprite, a6);
    dk::jump(at_Spawn_SetTimer1FromEntry);   /* its a1 = the entry, a4 = the new object's block */
}
