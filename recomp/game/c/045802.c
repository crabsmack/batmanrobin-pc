#include "game.h"

/* the spawned script object's a2 script (ROM data right after this spawner's code) */
static constexpr uint32_t Scr2_04580E = 0x04580Eu;

/* Enemy_LabTech_Spawn - spawns a lab technician of the stage 4 lab (white coat, red goggles) at floor height
 * $A2 with a random speed; coroutine 0 cycles its behaviours (hold a range beside the
 * targeted player, swap targets) through vector 0
 * In:  x/y (d4/d5) = the spawn position (wave entry: x -$14/-$50 left or $154/$190 right, y $70)
 * Out: script (a2) = Scr2_04580E, then Obj_SpawnEnemy's results (d0 = 0 and a6 = the new object, nonzero d0
 *      when the pool is short; its condition codes)
 * Evidence: lea Scr2_04580E,a2 / jmp Obj_SpawnEnemy; Scr2_04580E: Op2_SetSpeedRand $26/$E6, coroutine 0
 *           $45820 (Op2_SetVector0HereGoto list into $42AC8-$42B3A, Op2_SwapPlayerTarget), Op2_PlaceYOnScreen
 *           $A2, body $42A34 (population +1, score $64, energy $50 1P/2P, Op2_HoldRangeOfPlayerX $50-$5A); 15
 *           wave entries in Scr2_0417CC/Scr5_0413AC (some with spawn action $29); RAM f119460: two objects of
 *           this body at x 291/334, the goggled men in white lab coats of shot w29_c119460; TAS 119416-122363
 *           (11 spawns)
 */
void Enemy_LabTech_Spawn(uint16_t *d1, uint16_t x, uint16_t y, uint32_t *d7, uint32_t *script, uint32_t *a3,
                         uint32_t *a4, uint32_t *a6)
{
    *script = Scr2_04580E;
    dk::jump(at_Obj_SpawnEnemy);
}
