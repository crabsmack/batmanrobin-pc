#include "game.h"

/* the object's a2 script (ROM data right after this spawner's code; batmanrobin.asm) */
static constexpr uint32_t Scr2_02AA38 = 0x02AA38u;

/* Enemy_BlackDrone_SpawnTrackYA - wave spawner: a black gun drone (Scr2_02AA38) on the TrackY
 * course (cruise $300 both axes to 10 px left of its entry, follow the player's height $F00 frames,
 * leave); fires 4 rounds $F0 apart once
 * In:  x/y = where it starts (d4/d5); a1 = the wave entry's word after the callback (timer 1)
 * Out: Z set, a6 = the new object, its timer 1 (the attack delay) from the entry and a1 past the
 *      word; a short pool returns past this to the wave loop (Obj_SpawnEnemyOrQuit)
 * Evidence: lea Scr2_02AA38,a2 / jsr Obj_SpawnEnemyOrQuit / jmp Spawn_SetTimer1FromEntry;
 *           Op2_TargetSelf, then Op2_SetVector2HereGotoArgs2 `0000 FFF6` (words y, x ->
 *           Op2_AimAtTargetArgOffset $27D90: dx -$A, dy 0), then $27D1A (Op2_CruiseToPlayerY) for
 *           timer 2 $F00; Scr5_023156 +$5E/+$68/+$72 (drop action $31); its final wait is $4B00 (no
 *           second volley); TAS 42373-43016
 */
void Enemy_BlackDrone_SpawnTrackYA(uint16_t *d1, uint16_t x, uint16_t y, uint32_t *d7, uint32_t *a1, uint32_t *script, uint32_t *a3, uint32_t *a4, uint32_t *a6)
{
    /* Obj_Spawn sets a0 (the run list) and a5 itself: the values going in are not read */
    uint32_t d0 = 0, list = 0, sprite = 0;
    *script = Scr2_02AA38;
    Obj_SpawnEnemyOrQuit(&d0, d1, x, y, d7, &list, *script, a3, a4, &sprite, a6);
    dk::jump(at_Spawn_SetTimer1FromEntry);   /* its a1 = the entry, a4 = the new object's block */
}
