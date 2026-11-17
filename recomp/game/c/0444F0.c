#include "game.h"

/* the spawned script object's a2 script (ROM data right after this spawner's code) */
static constexpr uint32_t Scr2_0444FC = 0x0444FCu;

/* Enemy_HoverPod_Spawn1A - spawns a stage 4 hall hover pod on course 1 ($44836: two random screen-area
 * waypoints, x cruise $2AA then $22A/$200) with attack A ($44D68: three rounds of
 * six aimed shots $A0 ticks apart)
 * In:  x/y (d4/d5) = the spawn position (wave entry: screen x $A0, y $70)
 * Out: script (a2) = Scr2_0444FC, then Obj_SpawnEnemy's results (d0 = 0 and a6 = the new object, nonzero d0
 *      when the pool is short; its condition codes)
 * Evidence: lea Scr2_0444FC,a2 / jmp Obj_SpawnEnemy; Scr2_0444FC: speed $100, drop init $43A04, score $1F4,
 *           energy $1C0 (1P and 2P), x accel/cruise $2AA, y $155, coroutine 0 $44836, coroutine 1 $44D68,
 *           Op2_JitterPos $12C/$28 and op $1640 -$96 (starts above the screen), body $42152 (oscillator
 *           hover, heading-switched frames $48F8-$4970); wave entries in Tbl_041138 (+$6E, +$A2, +$166,
 *           +$1DC: hall waves started from Tbl_040F4A); RAM f117300: the pod (slots $4483E/$44D78 = course 1,
 *           attack A) at (151, 38) = the purple striped sphere with a thruster jet in shot w29_s117300; TAS
 *           116492-117218
 */
void Enemy_HoverPod_Spawn1A(uint16_t *d1, uint16_t x, uint16_t y, uint32_t *d7, uint32_t *script, uint32_t *a3,
                            uint32_t *a4, uint32_t *a6)
{
    *script = Scr2_0444FC;
    dk::jump(at_Obj_SpawnEnemy);
}
