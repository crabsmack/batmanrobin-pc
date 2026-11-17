#include "game.h"

/* the object's a2 script (ROM data right after this spawner's code; batmanrobin.asm) */
static constexpr uint32_t Scr2_02A5FE = 0x02A5FEu;

/* Enemy_BlackDrone_SpawnWaypointsC - wave spawner: a gun drone (Scr2_02A5FE) on a course of
 * relative waypoints (dx, dy) (-$28, 0), (-$28, -$32), (-5, +6), twice (+$7C, +$94), then (+$1F4,
 * +$258) off the screen down-right
 * In:  x/y = where it starts (d4/d5); a1 = the wave entry's word after the callback (timer 1)
 * Out: Z set, a6 = the new object, its timer 1 (the attack delay) from the entry and a1 past the
 *      word; a short pool returns past this to the wave loop (Obj_SpawnEnemyOrQuit)
 * Evidence: lea Scr2_02A5FE,a2 / jsr Obj_SpawnEnemyOrQuit / jmp Spawn_SetTimer1FromEntry;
 *           Op2_SetVector2HereGotoArgs2 words are (dy, dx), into Op2_AimAtTargetArgOffset
 *           ($27D90/$27D98); Scr5_022ADE +$17C/+$186 and Scr5_022C7E +$8E/+$98 (x $AA, y $2000),
 *           paired with WaypointsD; TAS 40732-41548
 */
void Enemy_BlackDrone_SpawnWaypointsC(uint16_t *d1, uint16_t x, uint16_t y, uint32_t *d7, uint32_t *a1, uint32_t *script, uint32_t *a3, uint32_t *a4, uint32_t *a6)
{
    /* Obj_Spawn sets a0 (the run list) and a5 itself: the values going in are not read */
    uint32_t d0 = 0, list = 0, sprite = 0;
    *script = Scr2_02A5FE;
    Obj_SpawnEnemyOrQuit(&d0, d1, x, y, d7, &list, *script, a3, a4, &sprite, a6);
    dk::jump(at_Spawn_SetTimer1FromEntry);   /* its a1 = the entry, a4 = the new object's block */
}
