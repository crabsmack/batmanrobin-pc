#include "game.h"

/* the skyscraper camera's a2 script (ROM) */
static constexpr uint32_t Scr2_048B2A = 0x048B2Au;

/* Attract_SpawnSkyscraperCamera - spawns the skyscraper scene's camera object (Scr2_048B2A): it
 * climbs, rises faster, eases off and ends the scene after $C8 frames, flashing the line tint
 * (the lightning) until then; its native Attract_SkyscraperScroll scrolls plane A's layer to it.
 * In:  x, y = the spawn position (Obj_SpawnEnemy's d4/d5)
 * Out: Obj_SpawnEnemy's results (a6 = the object, a3/a4 its blocks)
 * Evidence: lea Scr2_048B2A,a2 / jmp Obj_SpawnEnemy; Scr2_048B2A: SetCruiseY $A55, CruiseUp,
 *           CallNativeRelSaved Attract_SkyscraperScroll, Scene_EndAfterDelay $C8, Tint_FlashA2 /
 *           Tint_FlashE6 loop; only caller Attract_Skyscraper; TAS f391
 */
void Attract_SpawnSkyscraperCamera(uint16_t x, uint16_t y, uint32_t *a3, uint32_t *a4, uint32_t *a6)
{
    /* a jmp, called: a2 (the script) is not this prototype's to hand on; Obj_SpawnEnemy plays no
       stack tricks */
    uint32_t d0 = 0, d7 = 0, a0 = 0, a5 = 0;
    uint16_t w1 = 0;
    Obj_SpawnEnemy(&d0, &w1, x, y, &d7, &a0, Scr2_048B2A, a3, a4, &a5, a6);
}
