#include "game.h"

/* the object's a2 script (ROM data right after this spawner's code; batmanrobin.asm) */
static constexpr uint32_t Scr2_02A286 = 0x02A286u;

/* Enemy_BlackDrone_SpawnHoverLongA - wave spawner: the first of a faster trio of gun drones
 * (Scr2_02A286, entry cruise x $355) that hold one window beside the player, x +$B4..+$D2 right of
 * it and level with it (y -$14..+$14), for $B40 frames, then fly off left
 * In:  x/y = where it starts (d4/d5); a1 = the wave entry's word after the callback (timer 1)
 * Out: Z set, a6 = the new object, its timer 1 (the attack delay) from the entry and a1 past the
 *      word; a short pool returns past this to the wave loop (Obj_SpawnEnemyOrQuit)
 * Evidence: lea Scr2_02A286,a2 / jsr Obj_SpawnEnemyOrQuit / jmp Spawn_SetTimer1FromEntry;
 *           Op2_SetVector2HereGotoArgs4 words (y hi, y lo, x hi, x lo) = ($14, -$14, $D2, $B4) ->
 *           $27D34; Scr5_021D3A +$822/+$840/+$85E with HoverLongB/C (x $14A, delays
 *           $1E0/$3C0/$5A0); TAS 39651-40326
 */
void Enemy_BlackDrone_SpawnHoverLongA(uint16_t *d1, uint16_t x, uint16_t y, uint32_t *d7, uint32_t *a1, uint32_t *script, uint32_t *a3, uint32_t *a4, uint32_t *a6)
{
    /* Obj_Spawn sets a0 (the run list) and a5 itself: the values going in are not read */
    uint32_t d0 = 0, list = 0, sprite = 0;
    *script = Scr2_02A286;
    Obj_SpawnEnemyOrQuit(&d0, d1, x, y, d7, &list, *script, a3, a4, &sprite, a6);
    dk::jump(at_Spawn_SetTimer1FromEntry);   /* its a1 = the entry, a4 = the new object's block */
}
