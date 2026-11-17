#include "game.h"

/* the pickup's a2 script (ROM data right after this spawner's code; batmanrobin.asm) */
static constexpr uint32_t Scr2_013B34 = 0x013B34u;

/* Item_SpawnPowerUpFalling - the weapon-drop director's power-up from above: falls in and raises
 * the weapon level by one when touched (its icon cycles every $60 ticks)
 * In:  x/y = where the pickup appears (world pixels: WeaponDrop_SpawnOnScreen's screen position
 *      plus the camera)
 * Out: script = Scr2_013B34, then Obj_SpawnEnemy: d0 = 0 and a6 = the pickup, d0 nonzero when
 *      the pool is short (the other registers pass through unchanged)
 * Evidence: lea Scr2_013B34,a2 / jmp Obj_SpawnEnemy; Scr2_013B34 = coroutine $1336A (Op2_SetCruiseY $640), touch coroutine
 *           $13C4E (sound $47, Item_CollectWeaponLevelUp); the dc.l in the director scripts at
 *           $13FE4/$1400C/$14072 after WeaponDrop_FromTop; scenario-continue/game-over
 */
void Item_SpawnPowerUpFalling(uint32_t *result, uint16_t *d1, uint16_t x, uint16_t y, uint32_t *d7, uint32_t a4, uint32_t a6, uint32_t *script)
{
    *script = Scr2_013B34;
    dk::jump(at_Obj_SpawnEnemy);
}
