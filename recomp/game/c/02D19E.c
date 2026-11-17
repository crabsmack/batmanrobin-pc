#include "game.h"

/* the object's a2 script (ROM data right after this spawner's code; batmanrobin.asm) */
static constexpr uint32_t Scr2_02D1AA = 0x02D1AAu;

/* Enemy_DomeCraft_SpawnA - wave spawner: a green-domed craft (Scr2_02D1AA, body $280EC) that rises
 * from below the screen and holds a sequence of player-relative windows, then cruises off left;
 * fires bursts of 3 shot requests ($28156) 6 times, then launches 2 x 6 spinning mines downward
 * ($2999E)
 * In:  x/y = where the trigger or wave entry places it
 * Out: script = Scr2_02D1AA, then Obj_SpawnEnemy: Z set and a6 = the new object when it spawned, Z
 *      clear when the pool is short (d0 is clobbered: no caller reads it)
 * Evidence: lea Scr2_02D1AA,a2 / jmp Obj_SpawnEnemy; Scr5_021D3A +$2EC/+$310 (x $118, screen y
 *           $110, drop actions 0/$2D); TAS f41668
 */
void Enemy_DomeCraft_SpawnA(uint16_t *d1, uint16_t x, uint16_t y, uint32_t *d7, uint32_t *script, uint32_t *a3, uint32_t *a4, uint32_t *a6)
{
    *script = Scr2_02D1AA;
    dk::jump(at_Obj_SpawnEnemy);   /* jmp: Obj_SpawnEnemy returns to our caller */
}
