#include "game.h"

/* the object's a2 script (ROM data right after this spawner's code; batmanrobin.asm) */
static constexpr uint32_t Scr2_02A67C = 0x02A67Cu;

/* Enemy_BlackDrone_SpawnWaypointsD - wave spawner: WaypointsC mirrored vertically (Scr2_02A67C):
 * relative waypoints (dx, dy) (-$28, 0), (-$28, +$32), (-5, -6), twice (+$7C, -$94), then (+$1F4,
 * -$258) off the screen, upper gun first (vars 0/1 = 0); fires 3 rounds $F0 apart, then waits $480
 * In:  x/y = where it starts (d4/d5); a1 = the wave entry's word after the callback (timer 1)
 * Out: Z set, a6 = the new object, its timer 1 (the attack delay) from the entry and a1 past the
 *      word; a short pool returns past this to the wave loop (Obj_SpawnEnemyOrQuit)
 * Evidence: lea Scr2_02A67C,a2 / jsr Obj_SpawnEnemyOrQuit / jmp Spawn_SetTimer1FromEntry;
 *           Op2_SetVector2HereGotoArgs2 word pairs (y, x) into Op2_AimAtTargetArgOffset (Arg0 = dx,
 *           Arg1 = dy); C's pairs differ only in the y sign; Scr5_022ADE +$190/+$19A and
 *           Scr5_022C7E +$A2/+$AC; TAS 40941-41032
 */
void Enemy_BlackDrone_SpawnWaypointsD(uint16_t *d1, uint16_t x, uint16_t y, uint32_t *d7, uint32_t *a1, uint32_t *script, uint32_t *a3, uint32_t *a4, uint32_t *a6)
{
    /* Obj_Spawn sets a0 (the run list) and a5 itself: the values going in are not read */
    uint32_t d0 = 0, list = 0, sprite = 0;
    *script = Scr2_02A67C;
    Obj_SpawnEnemyOrQuit(&d0, d1, x, y, d7, &list, *script, a3, a4, &sprite, a6);
    dk::jump(at_Spawn_SetTimer1FromEntry);   /* its a1 = the entry, a4 = the new object's block */
}
