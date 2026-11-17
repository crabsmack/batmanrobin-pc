#include "game.h"

/* the object's a2 script (ROM data right after this spawner's code; batmanrobin.asm) */
static constexpr uint32_t Scr2_02A008 = 0x02A008u;

/* Enemy_BlackDrone_SpawnHoverA - wave spawner: the first of a trio of gun drones (Scr2_02A008) that
 * each hold two windows beside the targeted player for $780 frames apiece (this one: x +$B4..+$D2
 * right of the player at its height (y -$14..+$14), then x +$F0..+$10E and 60-80 px below it (y
 * +$3C..+$50)), then fly off left
 * In:  x/y = where it starts (d4/d5); a1 = the wave entry's word after the callback (timer 1)
 * Out: Z set, a6 = the new object, its timer 1 (the attack delay) from the entry and a1 past the
 *      word; a short pool returns past this to the wave loop (Obj_SpawnEnemyOrQuit)
 * Evidence: lea Scr2_02A008,a2 / jsr Obj_SpawnEnemyOrQuit / jmp Spawn_SetTimer1FromEntry; entries
 *           in Scr5_021D3A (+$6BC, +$756..) and Scr5_023B38 (+$EA..), always with HoverB/HoverC at
 *           x $154 and delays $1E0/$4B0/$780; window op $27D34 (Op2_SetPlayerWindowFromArgs +
 *           Op2_HoldPlayerWindow until timer 2), Op2_SetVector2HereGotoArgs4 words (y hi, y lo, x
 *           hi, x lo) = ($14, -$14, $D2, $B4) and ($50, $3C, $10E, $F0); the $10E x bound wraps in
 *           Mot_StorePlayerWindow's byte packing ($10E/2 = $87 -> -$F2), so Op2_HoldPlayerWindow
 *           keeps the drone hunting round x +$F0; attack loop $2A23C (two shots, $F0/$168 apart,
 *           then $2D0); TAS 35434-36156
 */
void Enemy_BlackDrone_SpawnHoverA(uint16_t *d1, uint16_t x, uint16_t y, uint32_t *d7, uint32_t *a1, uint32_t *script, uint32_t *a3, uint32_t *a4, uint32_t *a6)
{
    /* Obj_Spawn sets a0 (the run list) and a5 itself: the values going in are not read */
    uint32_t d0 = 0, list = 0, sprite = 0;
    *script = Scr2_02A008;
    Obj_SpawnEnemyOrQuit(&d0, d1, x, y, d7, &list, *script, a3, a4, &sprite, a6);
    dk::jump(at_Spawn_SetTimer1FromEntry);   /* its a1 = the entry, a4 = the new object's block */
}
