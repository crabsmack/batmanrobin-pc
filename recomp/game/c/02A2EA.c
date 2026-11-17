#include "game.h"

/* the object's a2 script (ROM data right after this spawner's code; batmanrobin.asm) */
static constexpr uint32_t Scr2_02A2FA = 0x02A2FAu;

/* Enemy_BlackDrone_SpawnHoverLongB - wave spawner: the second drone of the long-hover trio
 * (Scr2_02A2FA): window x +$F0..+$10E right of the player and 60-80 px above it (y -$50..-$3C) for
 * $B40 frames, then off left
 * In:  x/y = where it starts (d4/d5); a1 = the wave entry's word after the callback (timer 1)
 * Out: Z set, a6 = the new object, its timer 1 (the attack delay) from the entry and a1 past the
 *      word; a short pool returns past this to the wave loop (Obj_SpawnEnemyOrQuit)
 * Evidence: lea Scr2_02A2FA,a2 / jsr Obj_SpawnEnemyOrQuit / jmp Spawn_SetTimer1FromEntry;
 *           Op2_SetVector2HereGotoArgs4 words (y hi, y lo, x hi, x lo) = (-$3C, -$50, $10E, $F0) ->
 *           $27D34; the $10E x bound wraps in Mot_StorePlayerWindow's byte packing ($10E/2 = $87 ->
 *           -$F2), so Op2_HoldPlayerWindow keeps the drone hunting round x +$F0; Scr5_021D3A
 *           +$82C/+$84A/+$868; TAS 39695-40372
 */
void Enemy_BlackDrone_SpawnHoverLongB(uint16_t *d1, uint16_t x, uint16_t y, uint32_t *d7, uint32_t *a1, uint32_t *script, uint32_t *a3, uint32_t *a4, uint32_t *a6)
{
    /* Obj_Spawn sets a0 (the run list) and a5 itself: the values going in are not read */
    uint32_t d0 = 0, list = 0, sprite = 0;
    *script = Scr2_02A2FA;
    Obj_SpawnEnemyOrQuit(&d0, d1, x, y, d7, &list, *script, a3, a4, &sprite, a6);
    dk::jump(at_Spawn_SetTimer1FromEntry);   /* its a1 = the entry, a4 = the new object's block */
}
