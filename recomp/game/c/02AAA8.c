#include "game.h"

/* the object's a2 script (ROM data right after this spawner's code; batmanrobin.asm) */
static constexpr uint32_t Scr2_02AAB8 = 0x02AAB8u;

/* Enemy_BlackDrone_SpawnTrackYB - wave spawner: a black gun drone (Scr2_02AAB8) on the TrackY
 * course aimed 40 px left of its entry; fires 3 rounds of two shots ($78 then $F0 apart), pause
 * $1E0, repeat
 * In:  x/y = where it starts (d4/d5); a1 = the wave entry's word after the callback (timer 1)
 * Out: Z set, a6 = the new object, its timer 1 (the attack delay) from the entry and a1 past the
 *      word; a short pool returns past this to the wave loop (Obj_SpawnEnemyOrQuit)
 * Evidence: lea Scr2_02AAB8,a2 / jsr Obj_SpawnEnemyOrQuit / jmp Spawn_SetTimer1FromEntry;
 *           Op2_SetVector2HereGotoArgs2 words are (dy, dx) = (0, -$28) -> $27D90; Scr5_023156 +$BC
 *           (x $190, drop action $2B); a second attack loop at $2AB22 (4 rounds, then $5A00) is not
 *           reached from this spawner's script head; TAS 51018-57127
 */
void Enemy_BlackDrone_SpawnTrackYB(uint16_t *d1, uint16_t x, uint16_t y, uint32_t *d7, uint32_t *a1, uint32_t *script, uint32_t *a3, uint32_t *a4, uint32_t *a6)
{
    /* Obj_Spawn sets a0 (the run list) and a5 itself: the values going in are not read */
    uint32_t d0 = 0, list = 0, sprite = 0;
    *script = Scr2_02AAB8;
    Obj_SpawnEnemyOrQuit(&d0, d1, x, y, d7, &list, *script, a3, a4, &sprite, a6);
    dk::jump(at_Spawn_SetTimer1FromEntry);   /* its a1 = the entry, a4 = the new object's block */
}
