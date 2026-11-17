#include "game.h"

/* the object's a2 script (ROM data right after this spawner's code; batmanrobin.asm) */
static constexpr uint32_t Scr2_029F0C = 0x029F0Cu;

/* Enemy_BlackDrone_SpawnWaypointsA - wave spawner: a black gun drone (Scr2_029F0C, body $27DBE:
 * energy $68, score $C8) that flies a course of 6 waypoints: relative (dx, dy) (-$96, +$64), the
 * absolute point (x $C8, y $12C), relative (+$28, +$28), (+$14, -$14), the absolute point (x $64, y
 * 0) and finally relative (0, -$258) up off the screen, firing every $168 frames, the lower gun
 * first (vars 0/1 = 1: the shot counter; Scr2_027E58 fires the upper gun on odd counts, the lower
 * on even)
 * In:  x/y = where it starts (d4/d5); a1 = the wave entry's word after the callback (timer 1)
 * Out: Z set, a6 = the new object, its timer 1 (the attack delay) from the entry and a1 past the
 *      word; a short pool returns past this to the wave loop (Obj_SpawnEnemyOrQuit)
 * Evidence: lea Scr2_029F0C,a2 / jsr Obj_SpawnEnemyOrQuit / jmp Spawn_SetTimer1FromEntry;
 *           Op2_SetVector2HereGotoArgs2 word pairs (y, x) into Op2_AimAtTargetArgOffset
 *           $27D90/$27D98 (Arg0 = dx, Arg1 = dy), except pairs 2 and 5, which go to $27DA8
 *           (Op2_AimAtArgPoint: Arg0/Arg1 = absolute x/y); 12 entries in Scr5_021D3A (+$5C2.., y
 *           $2014, drop action 1 or $2C); frames $39BC/$39C6 rendered: a black pod with a cannon
 *           pointing left; TAS 36669-38223
 */
void Enemy_BlackDrone_SpawnWaypointsA(uint16_t *d1, uint16_t x, uint16_t y, uint32_t *d7, uint32_t *a1, uint32_t *script, uint32_t *a3, uint32_t *a4, uint32_t *a6)
{
    /* Obj_Spawn sets a0 (the run list) and a5 itself: the values going in are not read */
    uint32_t d0 = 0, list = 0, sprite = 0;
    *script = Scr2_029F0C;
    Obj_SpawnEnemyOrQuit(&d0, d1, x, y, d7, &list, *script, a3, a4, &sprite, a6);
    dk::jump(at_Spawn_SetTimer1FromEntry);   /* its a1 = the entry, a4 = the new object's block */
}
