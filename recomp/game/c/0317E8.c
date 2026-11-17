#include "game.h"

/* the object's a2 script (ROM data right after this spawner's code; batmanrobin.asm) */
static constexpr uint32_t Scr2_0317FA = 0x0317FAu;

/* Enemy_BallTurret_SpawnStream - spawns a ball turret that turns slowly (rate $400) and, while on
 * screen, fires a stream of 10 aimed shots $F0 apart, then stops turning and waits $960
 * In:  x/y = the trigger entry's map position; entry = the entry's words after the callback (a1:
 *      the start-delay word, timer 0)
 * Out: Obj_SpawnEnemyOrQuit with Scr2_0317FA (d1, a0/a4-a6 = the new turret's list and blocks),
 *      then Spawn_SetTimer0FromEntry: timer 0 = the word, entry past it, d0 = 0 and Z set; a short
 *      pool returns past this to Trigger_Fire
 * Evidence: lea Scr2_0317FA,a2 / jsr Obj_SpawnEnemyOrQuit / jmp Spawn_SetTimer0FromEntry; 4 trigger
 *           entries in Tbl_0307C2 (+$678-$6B4); body $2FDD4; TAS 71513-74279 (2 spawns)
 */
void Enemy_BallTurret_SpawnStream(uint16_t *d1, uint16_t x, uint16_t y, uint32_t *a0, uint32_t *entry, uint32_t *a4, uint32_t *a5, uint32_t *a6)
{
    uint32_t d0 = 0, d7 = 0, motion = 0;   /* this prototype's clobbers */
    Obj_SpawnEnemyOrQuit(&d0, d1, x, y, &d7, a0, Scr2_0317FA, &motion, a4, a5, a6);
    dk::jump(at_Spawn_SetTimer0FromEntry);   /* jmp: its a1 = the entry, a4 = the new turret's block */
}
