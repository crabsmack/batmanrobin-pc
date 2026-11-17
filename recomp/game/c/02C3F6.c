#include "game.h"

/* the object's a2 script (ROM data right after this spawner's code; batmanrobin.asm) */
static constexpr uint32_t Scr2_02C406 = 0x02C406u;

/* Enemy_RedDrone_SpawnHoverLevel - wave spawner: a red gun drone (Scr2_02C406) that holds a window
 * level with the targeted player (y +$14, x +$DC..+$E6) for $F00 ticks, then leaves like
 * Enemy_RedDrone_SpawnHoverAbove (shared attack $2C4DE)
 * In:  x/y = where the wave entry places it; entry = the entry's words after the callback long (a1:
 *      the attack-delay word, timer 1)
 * Out: script = Scr2_02C406, then Obj_SpawnEnemyOrQuit (a3/a4/a6 = the new drone's blocks, d1/d7)
 *      and Spawn_SetTimer1FromEntry: timer 1 = the word, entry past it, Z set (moveq #0,d0; d0
 *      itself is never read); a short pool returns past this to the wave loop
 * Evidence: lea Scr2_02C406,a2 / jsr Obj_SpawnEnemyOrQuit / jmp Spawn_SetTimer1FromEntry; window
 *           words ($14, $14, $E6, $DC); entries next to HoverAbove/HoverBelow in Scr5_022780 and
 *           Scr5_021D3A +$7BA (drop $17); not run in the TAS (fuzz runs)
 */
void Enemy_RedDrone_SpawnHoverLevel(uint16_t *d1, uint16_t x, uint16_t y, uint32_t *d7, uint32_t *entry, uint32_t *script, uint32_t *a3, uint32_t *a4, uint32_t *a6)
{
    /* Obj_Spawn sets a0 (the run list) and a5 itself: the values going in are not read */
    uint32_t d0 = 0, list = 0, sprite = 0;
    *script = Scr2_02C406;
    Obj_SpawnEnemyOrQuit(&d0, d1, x, y, d7, &list, *script, a3, a4, &sprite, a6);
    dk::jump(at_Spawn_SetTimer1FromEntry);   /* jmp: its a1 = the entry, a4 = the new drone's block */
}
