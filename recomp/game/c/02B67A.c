#include "game.h"

/* the object's a2 script (ROM data right after this spawner's code; batmanrobin.asm) */
static constexpr uint32_t Scr2_02B68A = 0x02B68Au;

/* Enemy_Jet_SpawnLane - wave spawner: a rocket jet (Scr2_02B68A, body $285DE) with a random pickup
 * ($29B76) that flies 40 px further in (cruise $2AA), then follows the player's height for $5A00
 * ticks and leaves; one per screen lane
 * In:  x/y = where the wave entry places it; entry = the entry's words after the callback long (a1:
 *      the attack-delay word, timer 1)
 * Out: script = Scr2_02B68A, then Obj_SpawnEnemyOrQuit (a3/a4/a6 = the new jet's blocks, d1/d7)
 *      and Spawn_SetTimer1FromEntry: timer 1 = the word, entry past it, Z set (moveq #0,d0; d0
 *      itself is never read); a short pool returns past this to the wave loop
 * Evidence: lea Scr2_02B68A,a2 / jsr Obj_SpawnEnemyOrQuit / jmp Spawn_SetTimer1FromEntry; 5 entries
 *           Tbl_023764 +$158..+$180 (x $14A, screen y $0C/$3E/$70/$A2/$D4, drop action $2B, delay
 *           $7F80 = the attack (pairs of $27E2C shot requests every $140) practically never
 *           starts); TAS 54046-64339 (117 frames), f055200 shows rows of rocket jets
 */
void Enemy_Jet_SpawnLane(uint16_t *d1, uint16_t x, uint16_t y, uint32_t *d7, uint32_t *entry, uint32_t *script, uint32_t *a3, uint32_t *a4, uint32_t *a6)
{
    /* Obj_Spawn sets a0 (the run list) and a5 itself: the values going in are not read */
    uint32_t d0 = 0, list = 0, sprite = 0;
    *script = Scr2_02B68A;
    Obj_SpawnEnemyOrQuit(&d0, d1, x, y, d7, &list, *script, a3, a4, &sprite, a6);
    dk::jump(at_Spawn_SetTimer1FromEntry);   /* jmp: its a1 = the entry, a4 = the new jet's block */
}
