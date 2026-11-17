#include "game.h"

/* the rooftop scene object's a2 script (ROM) */
static constexpr uint32_t Scr2_048CE2 = 0x048CE2u;

/* Attract_SpawnRooftopHeroes - spawns the rooftop scene's object (Scr2_048CE2): two animated
 * sprite blocks (Batman and Robin), Attract_RooftopLightning every frame, lightning flashes
 * (Tint_FlashA2 with sprite variable 0 = $20) and Scene_EndAfterDelay $14 at the end.
 * In:  x, y = the spawn position (Obj_SpawnEnemy's d4/d5)
 * Out: script = Scr2_048CE2; Obj_SpawnEnemy's results (a6 = the object, d0 = 0 / -1)
 * Evidence: lea Scr2_048CE2,a2 / jmp Obj_SpawnEnemy; Scr2_048CE2: AllocSprBlock $10, CallNative
 *           $048E80, SetAnimDelay $5A00 holds, Tint_FlashA2 + SetSprVar0 $20; only caller
 *           Attract_Rooftop; idle 926 + 3925n
 */
void Attract_SpawnRooftopHeroes(uint32_t *d0, uint16_t *d1, uint16_t x, uint16_t y, uint32_t *d7, uint32_t *script,
                                uint32_t *a3, uint32_t *a4, uint32_t a6)
{
    *script = Scr2_048CE2;
    dk::jump(at_Obj_SpawnEnemy);
}
