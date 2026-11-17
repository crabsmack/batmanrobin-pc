#include "game.h"

/* the object's a2 script (ROM data right after this spawner's code; batmanrobin.asm) */
static constexpr uint32_t Scr2_02ACF0 = 0x02ACF0u;

/* Enemy_RedDrone_SpawnCircuitB - wave spawner: a red gun drone (Scr2_02ACF0, body $27F70) on the
 * second circuit course: Enemy_RedDrone_SpawnCircuit mirrored vertically: 30 px left, then 2 laps
 * through the absolute points (x $3C, y $50), (x $78, y $13C), (x $50, y $50), (x $3C, y $13C) (the
 * circuit's y $13C/$50 swapped, mirrored about y $C6) with the relative legs' dy negated, then one
 * of 4 random relative offsets up-left ((dx, dy) (-$50, -$3C), (-$28, -$28), (-$50, -$50), (-$28,
 * -$78)), 40 px left, tracks the player's height $F00 ticks and leaves; fires two shots left every
 * $F0 ticks, 5 times, then pauses $870
 * In:  x/y = where the wave entry places it; entry = the entry's words after the callback long (a1:
 *      the attack-delay word, timer 1)
 * Out: script = Scr2_02ACF0, then Obj_SpawnEnemyOrQuit (a3/a4/a6 = the new drone's blocks, d1/d7)
 *      and Spawn_SetTimer1FromEntry: timer 1 = the word, entry past it, Z set (moveq #0,d0; d0
 *      itself is never read); a short pool returns past this to the wave loop
 * Evidence: lea Scr2_02ACF0,a2 / jsr Obj_SpawnEnemyOrQuit / jmp Spawn_SetTimer1FromEntry;
 *           Op2_SetVector2HereGotoArgs2 words are (dy, dx), the corner pairs into $27DA8
 *           (Op2_AimAtArgPoint, absolute); Scr5_023156 +$17C/+$186 (x $B4, y $203C, delays
 *           $F0/$780, group action 1), right after the two SpawnCircuit entries (y $1FC4); attack
 *           vector 1 = $2ADFE (Fx_SpawnAtOfsAngle at (-$10, -$B/+$B), heading $200), shared with
 *           Scr2_02ABD2; TAS 52007-52147
 */
void Enemy_RedDrone_SpawnCircuitB(uint16_t *d1, uint16_t x, uint16_t y, uint32_t *d7, uint32_t *entry, uint32_t *script, uint32_t *a3, uint32_t *a4, uint32_t *a6)
{
    /* Obj_Spawn sets a0 (the run list) and a5 itself: the values going in are not read */
    uint32_t d0 = 0, list = 0, sprite = 0;
    *script = Scr2_02ACF0;
    Obj_SpawnEnemyOrQuit(&d0, d1, x, y, d7, &list, *script, a3, a4, &sprite, a6);
    dk::jump(at_Spawn_SetTimer1FromEntry);   /* jmp: its a1 = the entry, a4 = the new drone's block */
}
