#include "game.h"

/* the object's a2 script (ROM data right after this spawner's code; batmanrobin.asm) */
static constexpr uint32_t Scr2_0315A0 = 0x0315A0u;

/* Enemy_MissileDome_SpawnSingles - spawns a missile dome on the hull that opens its hatch (anim
 * $2FCB0) and three times, while on screen, launches one Enemy_Missile_SpawnAgile to the left
 * (GosubArgs to $3164E: heading $200, 5 frames straight, $F homing), then closes (anim $2FD4A) and
 * waits $F00
 * In:  x/y = the trigger entry's map position; entry = the entry's words after the callback (a1:
 *      the timer-1 word)
 * Out: Obj_SpawnEnemyOrQuit with Scr2_0315A0 (d1, a0/a4-a6 = the new dome's list and blocks), then
 *      Spawn_SetTimer1FromEntry: timer 1 = the word, entry past it, d0 = 0 and Z set; a short pool
 *      returns past this to Trigger_Fire
 * Evidence: lea Scr2_0315A0,a2 / jsr Obj_SpawnEnemyOrQuit / jmp Spawn_SetTimer1FromEntry; 2 trigger
 *           entries (Tbl_0307C2 +$62C/+$63C); body $2FC56: energy $150/$300 (1P/2P), score $64;
 *           TAS f68764
 */
void Enemy_MissileDome_SpawnSingles(uint16_t *d1, uint16_t x, uint16_t y, uint32_t *a0, uint32_t *entry, uint32_t *a4, uint32_t *a5, uint32_t *a6)
{
    uint32_t d0 = 0, d7 = 0, motion = 0;   /* this prototype's clobbers */
    Obj_SpawnEnemyOrQuit(&d0, d1, x, y, &d7, a0, Scr2_0315A0, &motion, a4, a5, a6);
    dk::jump(at_Spawn_SetTimer1FromEntry);   /* jmp: its a1 = the entry, a4 = the new dome's block */
}
