#include "game.h"

/* the object's a2 script (ROM data right after this spawner's code; batmanrobin.asm) */
static constexpr uint32_t Scr2_02AF6A = 0x02AF6Au;

/* Enemy_BlackDrone_SpawnCircuitB - wave spawner: a black gun drone (Scr2_02AF6A, body $27DBE) on
 * the second circuit (as Enemy_RedDrone_SpawnCircuitB: 30 px left, 2 laps through the absolute
 * points (x $3C, y $50), (x $78, y $13C), (x $50, y $50), (x $3C, y $13C), 4 random up-left exits,
 * 40 px left, track the player's height $F00); its attack $2B078 fires 6 pairs of shot requests $F0
 * apart, then pauses $2D0
 * In:  x/y = where the wave entry places it; entry = the entry's words after the callback long (a1:
 *      the attack-delay word, timer 1)
 * Out: script = Scr2_02AF6A, then Obj_SpawnEnemyOrQuit (a3/a4/a6 = the new drone's blocks, d1/d7)
 *      and Spawn_SetTimer1FromEntry: timer 1 = the word, entry past it, Z set (moveq #0,d0; d0
 *      itself is never read); a short pool returns past this to the wave loop
 * Evidence: lea Scr2_02AF6A,a2 / jsr Obj_SpawnEnemyOrQuit / jmp Spawn_SetTimer1FromEntry;
 *           Scr5_023156 +$1FE/+$208 (x $B4, y $203C, delays $F0/$870); same course words as
 *           Scr2_02ACF0 with cruise $3AA; TAS 49847-49998
 */
void Enemy_BlackDrone_SpawnCircuitB(uint16_t *d1, uint16_t x, uint16_t y, uint32_t *d7, uint32_t *entry, uint32_t *script, uint32_t *a3, uint32_t *a4, uint32_t *a6)
{
    /* Obj_Spawn sets a0 (the run list) and a5 itself: the values going in are not read */
    uint32_t d0 = 0, list = 0, sprite = 0;
    *script = Scr2_02AF6A;
    Obj_SpawnEnemyOrQuit(&d0, d1, x, y, d7, &list, *script, a3, a4, &sprite, a6);
    dk::jump(at_Spawn_SetTimer1FromEntry);   /* jmp: its a1 = the entry, a4 = the new drone's block */
}
