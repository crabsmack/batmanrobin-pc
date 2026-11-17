#include "game.h"

/* the spawned object's a2 script (ROM data right after this spawner's code; batmanrobin.asm) */
static constexpr uint32_t Scr2_02A728 = 0x02A728u;

/* Enemy_BlackDrone_SpawnHoverRandomA - wave spawner: a black gun drone (Scr2_02A728, cruise $300)
 * that holds for $F00 frames one of three windows right of the targeted player at random (x
 * +$D2..+$DC: level y -$14..+$14, above y -$50..-$3C or below y +$3C..+$50), then leaves
 * In:  d4/d5 = x/y (the wave entry's), a1 = the entry
 * Out: script = Scr2_02A728, then Obj_SpawnEnemyOrQuit's results (a short pool returns from the
 *      caller), then the tail's
 * Evidence: spawner long(s) at $0230D8, $023100, $02311E, $02313C; lea script / jsr
 *           Obj_SpawnEnemyOrQuit / jmp Spawn_SetTimer1FromEntry(pc) as its siblings; raw until
 *           crane-crash (2026-10-30): no corpus run spawns it; 02A718.syn checks it
 */
void Enemy_BlackDrone_SpawnHoverRandomA(uint16_t *d1, uint16_t x, uint16_t y, uint32_t *d7, uint32_t *a1, uint32_t *script, uint32_t *a3, uint32_t *a4, uint32_t *a6)
{
    /* Obj_Spawn sets a0 (the run list) and a5 itself: the values going in are not read */
    uint32_t d0 = 0, list = 0, sprite = 0;
    *script = Scr2_02A728;
    Obj_SpawnEnemyOrQuit(&d0, d1, x, y, d7, &list, *script, a3, a4, &sprite, a6);
    dk::jump(at_Spawn_SetTimer1FromEntry);   /* its a1 = the entry, a4 = the new object's block */
}
