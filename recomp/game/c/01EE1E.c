#include "game.h"

/* jetpack Harley's script for her second appearance (ROM, right after this file's code) */
static constexpr uint32_t Scr2_01EE34 = 0x01EE34u;
static constexpr uint16_t HARLEYJET_FLOOR_Y = 0xD2;   /* the flat floor she fights over */

/* Boss_HarleyJet_SpawnSecond - claw-script native: puts a flat floor at y $D2 and spawns jetpack
 * Harley again (Scr2_01EE34: the same energy, timer $5A0, no wait for the population to drop)
 * In:  d4/d5 = the position Spawn_AtCameraOfs gives (op $2CBC at $1E956, camera offset $15E/$46)
 * Out: d0 low word = $D2 going in; script (a2) = Scr2_01EE34, then as Obj_SpawnBoss
 * Evidence: move.w #$D2,d0 / jsr Coll_SetFlatFloor / lea Scr2_01EE34,a2 / jmp Obj_SpawnBoss; called
 *           from Scr2_01E87E +$DE; TAS f10670, the object at $B6D2 runs Scr2_01EE64 from f10740
 */
void Boss_HarleyJet_SpawnSecond(uint32_t *d0, uint16_t *d1, uint16_t d4, uint16_t d5, uint32_t *d7, uint32_t a4,
                                uint32_t a6, uint32_t *script)
{
    *d0 = (*d0 & 0xFFFF0000u) | HARLEYJET_FLOOR_Y;
    dk::Flags ccr{};                  /* Obj_SpawnBoss sets its own */
    Coll_SetFlatFloor(HARLEYJET_FLOOR_Y, &ccr);
    *script = Scr2_01EE34;
    dk::jump(at_Obj_SpawnBoss);
}
