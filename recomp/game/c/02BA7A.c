#include "game.h"

/* the object's a2 script (ROM data right after this spawner's code; batmanrobin.asm) */
static constexpr uint32_t Scr2_02BA8A = 0x02BA8Au;

/* Enemy_RedDrone_SpawnLoops - wave spawner: a red gun drone (Scr2_02BA8A, body $27F70, cruise $355)
 * that runs 4 times round a 6-leg course (relative (dx, dy) (-$32, 0), the absolute point (x $D2, y
 * $C6), relative (0, +$90), (+$32, 0), (0, -$9E), (0, -$9E)), then flies 600 px up off the screen;
 * its attack Scr2_02BA3C fires 3 pairs of shots left $168 ticks apart, then pauses $5A0
 * In:  x/y = where the wave entry places it; entry = the entry's words after the callback long (a1:
 *      the attack-delay word, timer 1)
 * Out: script = Scr2_02BA8A, then Obj_SpawnEnemyOrQuit (a3/a4/a6 = the new drone's blocks, d1/d7)
 *      and Spawn_SetTimer1FromEntry: timer 1 = the word, entry past it, Z set (moveq #0,d0; d0
 *      itself is never read); a short pool returns past this to the wave loop
 * Evidence: lea Scr2_02BA8A,a2 / jsr Obj_SpawnEnemyOrQuit / jmp Spawn_SetTimer1FromEntry;
 *           Op2_SetCounter1 4 / Op2_LoopCounter1, final leg (dx, dy) = (0, -$258)
 *           (Op2_SetVector2HereGotoArgs2 words are (dy, dx): FDA8 0000); the 2nd leg goes to $27DA8
 *           (Op2_AimAtArgPoint, absolute); 18 entries in Scr5_021D3A +$A2C.., Scr5_022780,
 *           Scr5_022A5E (drop actions 0/$31/8); TAS 38295-48250
 */
void Enemy_RedDrone_SpawnLoops(uint16_t *d1, uint16_t x, uint16_t y, uint32_t *d7, uint32_t *entry, uint32_t *script, uint32_t *a3, uint32_t *a4, uint32_t *a6)
{
    /* Obj_Spawn sets a0 (the run list) and a5 itself: the values going in are not read */
    uint32_t d0 = 0, list = 0, sprite = 0;
    *script = Scr2_02BA8A;
    Obj_SpawnEnemyOrQuit(&d0, d1, x, y, d7, &list, *script, a3, a4, &sprite, a6);
    dk::jump(at_Spawn_SetTimer1FromEntry);   /* jmp: its a1 = the entry, a4 = the new drone's block */
}
