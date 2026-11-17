#include "game.h"

/* the barrel's a2 script (ROM data right after this spawner's code; batmanrobin.asm) */
static constexpr uint32_t Scr2_0253DC = 0x0253DCu;

/* Enemy_Barrel_Spawn - spawns the barrel a barrel thug throws (Scr2_0253DC) at x/y
 * In:  x/y = where the barrel starts (Enemy_BarrelThug_ThrowBarrel's offsets from the thug)
 * Out: script = Scr2_0253DC, then Obj_SpawnEnemy: d0 = 0, Z set and a6 = the barrel when it spawned,
 *      d0 nonzero and Z clear when the pool is short
 * Evidence: lea Scr2_0253DC,a2 / jmp Obj_SpawnEnemy; only caller Enemy_BarrelThug_ThrowBarrel
 *           ($02539C); RAM f28320: objects in Scr2_0253DC at x 228 y 180 (the barrel on the floor
 *           in shot f28320). The prototype gains &a2 (measured a clobber: no caller reads it) so
 *           the jump hands the script on to Obj_SpawnEnemy, and &d0 (measured a clobber) so
 *           its C caller Enemy_BarrelThug_ThrowBarrel reads the result as the asm reads Z
 */
void Enemy_Barrel_Spawn(uint32_t *result, uint16_t *d1, uint16_t x, uint16_t y, uint32_t *d7, uint32_t *script, uint32_t *a3, uint32_t a4, uint32_t *a6)
{
    *script = Scr2_0253DC;
    dk::jump(at_Obj_SpawnEnemy);   /* jmp: Obj_SpawnEnemy returns to our caller */
}
