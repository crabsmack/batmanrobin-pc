#include "game.h"

/* the object's a2 script (ROM data right after this spawner's code; batmanrobin.asm) */
static constexpr uint32_t Scr2_02B7BC = 0x02B7BCu;

/* Enemy_BlackDrone_SpawnHoverOnPlayer - wave spawner: the black-drone version of
 * Enemy_RedDrone_SpawnHoverOnPlayer (Scr2_02B7BC: body $27DBE, then vectors 0/1 jump into
 * Scr2_02B708's course $2B722 and attack $2B764)
 * In:  x/y = where the wave entry places it; entry = the entry's words after the callback long (a1:
 *      the attack-delay word, timer 1)
 * Out: script = Scr2_02B7BC, then Obj_SpawnEnemyOrQuit (a3/a4/a6 = the new drone's blocks, d1/d7)
 *      and Spawn_SetTimer1FromEntry: timer 1 = the word, entry past it, Z set (moveq #0,d0; d0
 *      itself is never read); a short pool returns past this to the wave loop
 * Evidence: lea Scr2_02B7BC,a2 / jsr Obj_SpawnEnemyOrQuit / jmp Spawn_SetTimer1FromEntry;
 *           Scr2_02B7BC is 26 bytes: SetSpeedRand, vectors to $2B722/$2B764, goto $27DBE; entries
 *           Scr5_021D3A +$36C/+$380/+$3BE/+$3C8; TAS f41698
 */
void Enemy_BlackDrone_SpawnHoverOnPlayer(uint16_t *d1, uint16_t x, uint16_t y, uint32_t *d7, uint32_t *entry, uint32_t *script, uint32_t *a3, uint32_t *a4, uint32_t *a6)
{
    /* Obj_Spawn sets a0 (the run list) and a5 itself: the values going in are not read */
    uint32_t d0 = 0, list = 0, sprite = 0;
    *script = Scr2_02B7BC;
    Obj_SpawnEnemyOrQuit(&d0, d1, x, y, d7, &list, *script, a3, a4, &sprite, a6);
    dk::jump(at_Spawn_SetTimer1FromEntry);   /* jmp: its a1 = the entry, a4 = the new drone's block */
}
