#include "game.h"

/* the object's a2 script (ROM data right after this spawner's code; batmanrobin.asm) */
static constexpr uint32_t Scr2_029F8A = 0x029F8Au;

/* Enemy_BlackDrone_SpawnWaypointsB - wave spawner: the gun drone with WaypointsA's course mirrored
 * vertically (Scr2_029F8A): relative (dx, dy) (-$96, -$64), the absolute point (x $C8, y $60),
 * relative (+$28, -$28), (+$14, +$14), the absolute point (x $64, y $18C), then relative (0, +$258)
 * down off the screen, firing every $168 frames, the upper gun first (vars 0/1 = 0)
 * In:  x/y = where it starts (d4/d5); a1 = the wave entry's word after the callback (timer 1)
 * Out: Z set, a6 = the new object, its timer 1 (the attack delay) from the entry and a1 past the
 *      word; a short pool returns past this to the wave loop (Obj_SpawnEnemyOrQuit)
 * Evidence: lea Scr2_029F8A,a2 / jsr Obj_SpawnEnemyOrQuit / jmp Spawn_SetTimer1FromEntry;
 *           Op2_SetVector2HereGotoArgs2 words are (dy, dx); pairs 2 and 5 go to $27DA8
 *           (Op2_AimAtArgPoint, absolute; A's y $12C/0 mirrored about y $C6), the rest to
 *           $27D90/$27D98 (Op2_AimAtTargetArgOffset, relative); 12 entries in Scr5_021D3A (+$640..,
 *           y $2178), interleaved with WaypointsA; TAS 37005-37943
 */
void Enemy_BlackDrone_SpawnWaypointsB(uint16_t *d1, uint16_t x, uint16_t y, uint32_t *d7, uint32_t *a1, uint32_t *script, uint32_t *a3, uint32_t *a4, uint32_t *a6)
{
    /* Obj_Spawn sets a0 (the run list) and a5 itself: the values going in are not read */
    uint32_t d0 = 0, list = 0, sprite = 0;
    *script = Scr2_029F8A;
    Obj_SpawnEnemyOrQuit(&d0, d1, x, y, d7, &list, *script, a3, a4, &sprite, a6);
    dk::jump(at_Spawn_SetTimer1FromEntry);   /* its a1 = the entry, a4 = the new object's block */
}
