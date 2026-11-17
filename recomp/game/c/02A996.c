#include "game.h"

/* the object's a2 script (ROM data right after this spawner's code; batmanrobin.asm) */
static constexpr uint32_t Scr2_02A9A6 = 0x02A9A6u;

/* Enemy_RedDrone_SpawnTrackY - wave spawner: a red gun drone (Scr2_02A9A6, body $27F70: energy $88)
 * that flies in toward a point 10 px left of its entry (cruise x $200, y $300), stops, follows the
 * player's height for $F00 frames and leaves; fires two shots left from (dx, dy) = (-$10, -$B/+$B)
 * every $140 frames
 * In:  x/y = where it starts (d4/d5); a1 = the wave entry's word after the callback (timer 1)
 * Out: Z set, a6 = the new object, its timer 1 (the attack delay) from the entry and a1 past the
 *      word; a short pool returns past this to the wave loop (Obj_SpawnEnemyOrQuit)
 * Evidence: lea Scr2_02A9A6,a2 / jsr Obj_SpawnEnemyOrQuit / jmp Spawn_SetTimer1FromEntry;
 *           Op2_SetVector2HereGotoArgs2 words are (dy, dx) = (0, -$A) -> $27D90
 *           (Op2_AimAtTargetArgOffset after Op2_TargetSelf); 7 entries in Scr5_023156 (x $154, drop
 *           actions $31/$1A/$2A/$2B/0); vector 2 $27D1A (Op2_CruiseToPlayerY until timer 2); shots
 *           via Fx_SpawnAtOfsAngle ($9010: heading $200, script $2CF92); frames $3A02-$3A16
 *           rendered: the drone body in red/orange; TAS 42282-57038
 */
void Enemy_RedDrone_SpawnTrackY(uint16_t *d1, uint16_t x, uint16_t y, uint32_t *d7, uint32_t *a1, uint32_t *script, uint32_t *a3, uint32_t *a4, uint32_t *a6)
{
    /* Obj_Spawn sets a0 (the run list) and a5 itself: the values going in are not read */
    uint32_t d0 = 0, list = 0, sprite = 0;
    *script = Scr2_02A9A6;
    Obj_SpawnEnemyOrQuit(&d0, d1, x, y, d7, &list, *script, a3, a4, &sprite, a6);
    dk::jump(at_Spawn_SetTimer1FromEntry);   /* its a1 = the entry, a4 = the new object's block */
}
