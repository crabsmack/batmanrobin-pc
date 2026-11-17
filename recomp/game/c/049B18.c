#include "game.h"

/* the player-select chooser's a2 script (ROM) */
static constexpr uint32_t Scr2_049B24 = 0x049B24u;

/* PlayerSelect_SpawnChooser - spawns the player-select chooser (Scr2_049B24): positions both halves
 * off screen, reads the pad and slides the halves every frame; A/B/C/Up/Down toggle the hero and
 * restart the 300-frame timer; Start or the timer confirms (4 tint flashes, the scene ends 10
 * frames later).
 * In:  x, y = the spawn position (Obj_SpawnEnemy's d4/d5)
 * Out: Obj_SpawnEnemy's results (a6 = the object, a3/a4 its blocks)
 * Evidence: lea Scr2_049B24,a2 / jmp Obj_SpawnEnemy; Scr2_049B24: Op2_PlaceOnScreen $FF01/$FEE8,
 *           Op2_SetTimer0 $12C0, natives $49B04/$49BA0/$49BEE/$49AD4; TAS f431
 */
void PlayerSelect_SpawnChooser(uint16_t *d1, uint16_t x, uint16_t y, uint32_t *a3, uint32_t *a4, uint32_t *a6)
{
    /* a jmp, called: a2 (the script) is not this prototype's to hand on; Obj_SpawnEnemy plays no
       stack tricks */
    uint32_t d0 = 0, d7 = 0, a0 = 0, a5 = 0;
    Obj_SpawnEnemy(&d0, d1, x, y, &d7, &a0, Scr2_049B24, a3, a4, &a5, a6);
}
