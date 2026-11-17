#include "game.h"

/* the object's a2 script (ROM data right after this spawner's code; batmanrobin.asm) */
static constexpr uint32_t Scr2_02C0DA = 0x02C0DAu;

/* Enemy_RedDrone_SpawnDropB - wave spawner: the second drop-in red drone (Scr2_02C0DA): drops 100
 * px from above the screen, then holds a window below-right of the player (y +$32..+$64, x
 * +$B4..+$D2) and then above-right (y -$64..-$32) for $780 ticks each, tracks the player's height
 * and leaves; shares the attack $2C174
 * In:  x/y = where the wave entry places it; entry = the entry's words after the callback long (a1:
 *      the attack-delay word, timer 1)
 * Out: script = Scr2_02C0DA, then Obj_SpawnEnemyOrQuit (a3/a4/a6 = the new drone's blocks, d1/d7)
 *      and Spawn_SetTimer1FromEntry: timer 1 = the word, entry past it, Z set (moveq #0,d0; d0
 *      itself is never read); a short pool returns past this to the wave loop
 * Evidence: lea Scr2_02C0DA,a2 / jsr Obj_SpawnEnemyOrQuit / jmp Spawn_SetTimer1FromEntry; entries y
 *           $DFF8 at x $C8/$12C in Tbl_0234DC / Scr5_0235B4 (next to SpawnDropA); TAS 46591-46638
 */
void Enemy_RedDrone_SpawnDropB(uint16_t *d1, uint16_t x, uint16_t y, uint32_t *d7, uint32_t *entry, uint32_t *script, uint32_t *a3, uint32_t *a4, uint32_t *a6)
{
    /* Obj_Spawn sets a0 (the run list) and a5 itself: the values going in are not read */
    uint32_t d0 = 0, list = 0, sprite = 0;
    *script = Scr2_02C0DA;
    Obj_SpawnEnemyOrQuit(&d0, d1, x, y, d7, &list, *script, a3, a4, &sprite, a6);
    dk::jump(at_Spawn_SetTimer1FromEntry);   /* jmp: its a1 = the entry, a4 = the new drone's block */
}
