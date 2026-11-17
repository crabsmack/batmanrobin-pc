#include "game.h"

/* the object's a2 script (ROM data right after this spawner's code; batmanrobin.asm) */
static constexpr uint32_t Scr2_02A156 = 0x02A156u;

/* Enemy_BlackDrone_SpawnHoverB - wave spawner: the second drone of the hover trio (Scr2_02A156):
 * holds a player window nominally 240-270 px right (x +$F0..+$10E; the $10E bound wraps, see
 * Evidence) and 60-80 px above it (y -$50..-$3C), then the same x band at the player's height (y
 * -$14..+$14), $780 frames each, then off left
 * In:  x/y = where it starts (d4/d5); a1 = the wave entry's word after the callback (timer 1)
 * Out: Z set, a6 = the new object, its timer 1 (the attack delay) from the entry and a1 past the
 *      word; a short pool returns past this to the wave loop (Obj_SpawnEnemyOrQuit)
 * Evidence: lea Scr2_02A156,a2 / jsr Obj_SpawnEnemyOrQuit / jmp Spawn_SetTimer1FromEntry;
 *           Op2_SetVector2HereGotoArgs4 words (y hi, y lo, x hi, x lo) = (-$3C, -$50, $10E, $F0)
 *           and ($14, -$14, $10E, $F0) -> Op2_SetPlayerWindowFromArgs (Arg0/1 = x lo/hi, Arg2/3 = y
 *           lo/hi); spawned with HoverA/HoverC (Scr5_021D3A +$6C6, +$760..; Scr5_023B38 +$F4..);
 *           TAS 35493-37761; x hi $10E packs to byte $87 (Mot_StorePlayerWindow stores half of each
 *           bound) and reads back as -$F2, so the drone hunts round x +$F0 instead of holding the
 *           240-270 px band (NOTES)
 */
void Enemy_BlackDrone_SpawnHoverB(uint16_t *d1, uint16_t x, uint16_t y, uint32_t *d7, uint32_t *a1, uint32_t *script, uint32_t *a3, uint32_t *a4, uint32_t *a6)
{
    /* Obj_Spawn sets a0 (the run list) and a5 itself: the values going in are not read */
    uint32_t d0 = 0, list = 0, sprite = 0;
    *script = Scr2_02A156;
    Obj_SpawnEnemyOrQuit(&d0, d1, x, y, d7, &list, *script, a3, a4, &sprite, a6);
    dk::jump(at_Spawn_SetTimer1FromEntry);   /* its a1 = the entry, a4 = the new object's block */
}
