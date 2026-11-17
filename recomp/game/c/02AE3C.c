#include "game.h"

/* the object's a2 script (ROM data right after this spawner's code; batmanrobin.asm) */
static constexpr uint32_t Scr2_02AE4C = 0x02AE4Cu;

/* Enemy_BlackDrone_SpawnCircuitA - wave spawner: a black gun drone (Scr2_02AE4C, body $27DBE) on
 * the same circuit as Enemy_RedDrone_SpawnCircuit (30 px left, 2 laps through the absolute points
 * (x $3C, y $13C), (x $78, y $50), (x $50, y $13C), (x $3C, y $50), 4 random down-left exits, 40 px
 * left, track the player's height $F00) but cruising faster ($3AA, exit $400); fires through the
 * shared attack $2B078
 * In:  x/y = where the wave entry places it; entry = the entry's words after the callback long (a1:
 *      the attack-delay word, timer 1)
 * Out: script = Scr2_02AE4C, then Obj_SpawnEnemyOrQuit (a3/a4/a6 = the new drone's blocks, d1/d7)
 *      and Spawn_SetTimer1FromEntry: timer 1 = the word, entry past it, Z set (moveq #0,d0; d0
 *      itself is never read); a short pool returns past this to the wave loop
 * Evidence: lea Scr2_02AE4C,a2 / jsr Obj_SpawnEnemyOrQuit / jmp Spawn_SetTimer1FromEntry;
 *           Scr5_023156 +$1EA/+$1F4 (x $B4, y $1FC4, delays $F0/$870, group action 1); attack
 *           $2B078: 6 x two shot requests (gosub $27E2C = Op2_IncVar0 unless a player is in front)
 *           $F0 apart, then $2D0 pause; TAS 49667-49818
 */
void Enemy_BlackDrone_SpawnCircuitA(uint16_t *d1, uint16_t x, uint16_t y, uint32_t *d7, uint32_t *entry, uint32_t *script, uint32_t *a3, uint32_t *a4, uint32_t *a6)
{
    /* Obj_Spawn sets a0 (the run list) and a5 itself: the values going in are not read */
    uint32_t d0 = 0, list = 0, sprite = 0;
    *script = Scr2_02AE4C;
    Obj_SpawnEnemyOrQuit(&d0, d1, x, y, d7, &list, *script, a3, a4, &sprite, a6);
    dk::jump(at_Spawn_SetTimer1FromEntry);   /* jmp: its a1 = the entry, a4 = the new drone's block */
}
