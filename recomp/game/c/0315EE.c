#include "game.h"

/* the object's a2 script (ROM data right after this spawner's code; batmanrobin.asm) */
static constexpr uint32_t Scr2_031600 = 0x031600u;

/* Enemy_MissileDome_SpawnFan - spawns a missile dome that waits $1E0, opens, then launches a fan of
 * three Enemy_Missile_SpawnSlow (headings $250/$200/$1B0, $1E frames straight, $78/$A0/$C8
 * homing), closes after $5A0 and repeats after $780
 * In:  x/y = the trigger entry's map position; entry = the entry's words after the callback (a1:
 *      the timer-1 word)
 * Out: Obj_SpawnEnemyOrQuit with Scr2_031600 (d1, a0/a4-a6 = the new dome's list and blocks), then
 *      Spawn_SetTimer1FromEntry: timer 1 = the word, entry past it, d0 = 0 and Z set; a short pool
 *      returns past this to Trigger_Fire
 * Evidence: lea Scr2_031600,a2 / jsr Obj_SpawnEnemyOrQuit / jmp Spawn_SetTimer1FromEntry; trigger
 *           entry Tbl_0307C2 +$64C; body $2FC42 (energy $100, score $64); three GosubArgs ops at
 *           $3161C-$31638 to the launch subroutine $316CE; TAS f71417
 */
void Enemy_MissileDome_SpawnFan(uint16_t *d1, uint16_t x, uint16_t y, uint32_t *a0, uint32_t *entry, uint32_t *a4, uint32_t *a5, uint32_t *a6)
{
    uint32_t d0 = 0, d7 = 0, motion = 0;   /* this prototype's clobbers */
    Obj_SpawnEnemyOrQuit(&d0, d1, x, y, &d7, a0, Scr2_031600, &motion, a4, a5, a6);
    dk::jump(at_Spawn_SetTimer1FromEntry);   /* jmp: its a1 = the entry, a4 = the new dome's block */
}
