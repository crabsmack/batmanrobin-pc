#include "game.h"

/* the a2 script the spawned object runs (ROM data right after this spawner's code; batmanrobin.asm) */
static constexpr uint32_t Scr2_01AF1A = 0x01AF1Au;

/* Enemy_Brute_SpawnPhased - spawns the brute (palette line 3, energy $D0) whose behaviour
 * vector 1 is switched on timers ($F0, $F0, $5A0 frames) by its own loop at $1AF4C
 * In:  d4/d5 = x/y (trigger Tbl_0163BA)
 * Out: script = Scr2_01AF1A, then Obj_SpawnEnemy's (d0 = 0 and a6 = the new object; Z clear
 *      when the pool is short)
 * Evidence: lea Scr2_01AF1A,a2 / jmp Obj_SpawnEnemy; Scr2_01AF1A: vector-2 +8 goto = $1AF4C
 *           (SetVector1HereGoto / SetTimer0 $F0, $F0, $5A0, loops); its prologue heads the
 *           shared brute body; Gem Expo trigger table Tbl_0163BA; TAS f6766-f18529 (13 frames)
 */
void Enemy_Brute_SpawnPhased(uint16_t *d1, uint16_t d4, uint16_t d5, uint32_t *a0, uint32_t *script,
                             uint32_t *a4, uint32_t *a5, uint32_t *a6)
{
    *script = Scr2_01AF1A;
    dk::jump(at_Obj_SpawnEnemy);
}
