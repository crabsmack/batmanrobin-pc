#include "game.h"

/* the road driver's a2 script (ROM data after this spawner) */
static constexpr uint32_t Scr2_04061A = 0x04061Au;

/* Stage3_SpawnRoadDriver - spawns the Hatter road's driver object Scr2_04061A (boss class): every
 * frame it runs Stage3_RoadCameraFollow and Stage3_DrawRoad
 * In:  d4/d5 = position (unused by the driver)
 * Out: script (a2) = Scr2_04061A, then Obj_SpawnBoss's (a6 = the driver, a3/a4 its blocks)
 * Evidence: lea Scr2_04061A,a2 / jmp Obj_SpawnBoss; Scr2_04061A: Op2_SetSpeed $100,
 *           Op2_CallNativeRelSaved $04062A and $0406CC, Op2_YieldGoto back; bsr from
 *           Stage3_InitHatterRoad; TAS f113311. a2 an output (measured as a clobber):
 *           Obj_SpawnBoss, jumped to, takes the script in it
 */
void Stage3_SpawnRoadDriver(uint16_t d4, uint16_t d5, uint32_t *script, uint32_t *a3, uint32_t *a4, uint32_t *a6)
{
    *script = Scr2_04061A;
    dk::jump(at_Obj_SpawnBoss);
}
