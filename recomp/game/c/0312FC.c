#include "game.h"

/* the object's a2 script (ROM data right after this spawner's code; batmanrobin.asm) */
static constexpr uint32_t Scr2_03130E = 0x03130Eu;

/* Enemy_HullTurret_SpawnDelayed - spawns an aiming hull gun turret whose start delay (timer 0) comes
 * from the trigger entry; it fires 3 aimed shots $1E0 apart, swaps target and repeats
 * In:  x/y = the trigger entry's map position; entry = the entry's words after the callback (a1:
 *      the start-delay word)
 * Out: Obj_SpawnEnemyOrQuit with Scr2_03130E (d1, a0/a4-a6 = the new turret's list and blocks),
 *      then Spawn_SetTimer0FromEntry: timer 0 = the word, entry past it, d0 = 0 and Z set; a short
 *      pool returns past this to Trigger_Fire
 * Evidence: lea Scr2_03130E,a2 / jsr Obj_SpawnEnemyOrQuit / jmp Spawn_SetTimer0FromEntry; the most
 *           common entry of Tbl_0307C2 (57 entries, 24 with drop byte $2B); body $2F98C, coroutine
 *           0 $2F9EC aims; TAS 67325-75173 (35 spawns)
 */
void Enemy_HullTurret_SpawnDelayed(uint16_t *d1, uint16_t x, uint16_t y, uint32_t *a0, uint32_t *entry, uint32_t *a4, uint32_t *a5, uint32_t *a6)
{
    uint32_t d0 = 0, d7 = 0, motion = 0;   /* this prototype's clobbers */
    Obj_SpawnEnemyOrQuit(&d0, d1, x, y, &d7, a0, Scr2_03130E, &motion, a4, a5, a6);
    dk::jump(at_Spawn_SetTimer0FromEntry);   /* jmp: its a1 = the entry, a4 = the new turret's block */
}
