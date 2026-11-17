#include "game.h"

/* the object's a2 script (ROM data right after this spawner's code; batmanrobin.asm) */
static constexpr uint32_t Scr2_02ABD2 = 0x02ABD2u;

/* Enemy_RedDrone_SpawnCircuit - wave spawner: a red gun drone (Scr2_02ABD2) that flies 30 px left
 * of its entry, then loops twice round a circuit through the absolute points (x $3C, y $13C), (x
 * $78, y $50), (x $50, y $13C), (x $3C, y $50), each followed by two short relative legs (e.g. (dx,
 * dy) (+$28, +$14), (+$28, -$14)), then moves by one of 4 random relative offsets down-left ((-$50,
 * +$3C), (-$28, +$28), (-$50, +$50), (-$28, +$78)), 40 px further left, follows the player's height
 * $F00 ticks and leaves
 * In:  x/y = where the wave entry places it; entry = the entry's words after the callback long (a1:
 *      the attack-delay word, timer 1)
 * Out: script = Scr2_02ABD2, then Obj_SpawnEnemyOrQuit (a3/a4/a6 = the new drone's blocks, d1/d7)
 *      and Spawn_SetTimer1FromEntry: timer 1 = the word, entry past it, Z set (moveq #0,d0; d0
 *      itself is never read); a short pool returns past this to the wave loop
 * Evidence: lea Scr2_02ABD2,a2 / jsr Obj_SpawnEnemyOrQuit / jmp Spawn_SetTimer1FromEntry;
 *           Op2_SetVector2HereGotoArgs2 words are (dy, dx); the corner pairs go to $27DA8
 *           (Op2_AimAtArgPoint, absolute x/y), the other legs to $27D90/$27D98
 *           (Op2_AimAtTargetArgOffset after Op2_TargetSelf), the last leg to vector 2 $27D1A
 *           (Op2_CruiseToPlayerY); Scr5_023156 +$168/+$172 (x $B4, y $1FC4, delays $F0/$780);
 *           Op2_SetCounter1 2 / Op2_LoopCounter1 round 10 waypoints, Op2_SwitchRandom over 4 exits;
 *           body $27F70; TAS 57398-57538
 */
void Enemy_RedDrone_SpawnCircuit(uint16_t *d1, uint16_t x, uint16_t y, uint32_t *d7, uint32_t *entry, uint32_t *script, uint32_t *a3, uint32_t *a4, uint32_t *a6)
{
    /* Obj_Spawn sets a0 (the run list) and a5 itself: the values going in are not read */
    uint32_t d0 = 0, list = 0, sprite = 0;
    *script = Scr2_02ABD2;
    Obj_SpawnEnemyOrQuit(&d0, d1, x, y, d7, &list, *script, a3, a4, &sprite, a6);
    dk::jump(at_Spawn_SetTimer1FromEntry);   /* jmp: its a1 = the entry, a4 = the new drone's block */
}
