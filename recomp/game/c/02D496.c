#include "game.h"

/* the object's a2 script (ROM data right after this spawner's code; batmanrobin.asm) */
static constexpr uint32_t Scr2_02D4A2 = 0x02D4A2u;

/* Enemy_Biplane_SpawnZigzagA - wave spawner: a red biplane (Scr2_02D4A2, body $28286, no attack)
 * whose first leg has the (y, x) words ($94, $B4) and that then zigzags with legs that speed up
 * from $4AA to $655
 * In:  x/y = where the trigger or wave entry places it
 * Out: script = Scr2_02D4A2, then Obj_SpawnEnemy: Z set and a6 = the new object when it spawned, Z
 *      clear when the pool is short (d0 is clobbered: no caller reads it)
 * Evidence: lea Scr2_02D4A2,a2 / jmp Obj_SpawnEnemy; Scr5_023C78 +$A0 (origin entry 0/$2000,
 *           kill-group member action 1); the rest of the course ($2D54A-$2D660) is never read; fuzz
 *           runs only
 */
void Enemy_Biplane_SpawnZigzagA(uint16_t *d1, uint16_t x, uint16_t y, uint32_t *d7, uint32_t *script, uint32_t *a3, uint32_t *a4, uint32_t *a6)
{
    *script = Scr2_02D4A2;
    dk::jump(at_Obj_SpawnEnemy);   /* jmp: Obj_SpawnEnemy returns to our caller */
}
