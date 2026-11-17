#include "game.h"

/* the skyscraper scene's second object's a2 script (ROM) */
static constexpr uint32_t Scr2_048B9A = 0x048B9Au;

/* Attract_SpawnPlaneBDriver - spawns the skyscraper scene's second object (Scr2_048B9A): it enters
 * at screen (30, 110) with a hover oscillation, drifts, then cruises left, drawing two sprites;
 * its native Attract_PlaneBFollowObj makes plane B follow it; at the end it starts an a6 loader
 * script and dies.
 * In:  x, y = the spawn position (Obj_SpawnEnemy's d4/d5)
 * Out: script = Scr2_048B9A; Obj_SpawnEnemy's results
 * Evidence: lea Scr2_048B9A,a2 / jmp Obj_SpawnEnemy; Scr2_048B9A: PlaceOnScreen $1E $6E,
 *           AllocOsc / SetOscY, CruiseLeft, CallNativeRelSaved $048CA6, Op2_StartA6Script $048A62,
 *           Kill; only caller Attract_Skyscraper; TAS f391
 */
void Attract_SpawnPlaneBDriver(uint16_t x, uint16_t y, uint32_t *d7, uint32_t *script, uint32_t *a3, uint32_t *a4,
                               uint32_t a6)
{
    *script = Scr2_048B9A;
    dk::jump(at_Obj_SpawnEnemy);
}
