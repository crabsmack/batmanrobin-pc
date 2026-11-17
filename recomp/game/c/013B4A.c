#include "game.h"

/* the pickup's a2 script (ROM data right after this spawner's code; batmanrobin.asm) */
static constexpr uint32_t Scr2_013B56 = 0x013B56u;

/* Item_SpawnPowerUpDrifting - the weapon-drop director's power-up from the right edge: drifts in
 * sideways and raises the weapon level by one when touched
 * In:  x/y = where the pickup appears (world pixels: WeaponDrop_SpawnOnScreen's screen position
 *      plus the camera)
 * Out: script = Scr2_013B56, then Obj_SpawnEnemy: d0 = 0 and a6 = the pickup, d0 nonzero when
 *      the pool is short (the other registers pass through unchanged)
 * Evidence: lea Scr2_013B56,a2 / jmp Obj_SpawnEnemy; Scr2_013B56 = coroutine $133CA, else as Item_SpawnPowerUpFalling; the
 *           dc.l in the director scripts at $1402E/$14050 after WeaponDrop_FromRight;
 *           scenario-level-skip
 */
void Item_SpawnPowerUpDrifting(uint32_t *result, uint16_t *d1, uint16_t x, uint16_t y, uint32_t *d7, uint32_t a4, uint32_t a6, uint32_t *script)
{
    *script = Scr2_013B56;
    dk::jump(at_Obj_SpawnEnemy);
}
