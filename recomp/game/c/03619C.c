#include "game.h"

/* the bomb's a2 script (ROM data right after this spawner's code; batmanrobin.asm) */
static constexpr uint32_t Scr2_0361A8 = 0x0361A8u;

/* Enemy_BouncingBomb_Spawn - spawns a bouncing bomb (boss-class script Scr2_0361A8: a small hop, an
 * 8x8 body box, bounces with a sound and dust, explodes after a random delay; a player's hit bats
 * it back)
 * In:  x/y = position (d4/d5); a6 = the dropper (passed on)
 * Out: script = Scr2_0361A8, then Obj_SpawnBoss's: d0 = 0 (Z set) and a3/a4 = the new bomb's
 *      motion / VM blocks, or d0 = -1 (Z clear) when the pool is short
 * Evidence: lea Scr2_0361A8,a2 / jmp Obj_SpawnBoss; called by Enemy_BombDropper_DropBomb (its only
 *           caller), TAS f79427-f86692. The prototype hands on a2 (measured as a clobber:
 *           dk::jump passes only the prototype's registers) and gives d0 (measured as a clobber):
 *           Obj_SpawnBoss's Z is d0 == 0, which the C caller branches on
 */
void Enemy_BouncingBomb_Spawn(uint32_t *d0, uint16_t *d1, uint16_t x, uint16_t y, uint32_t *d7, uint32_t *script,
                              uint32_t *a3, uint32_t *a4, uint32_t a6)
{
    *script = Scr2_0361A8;
    dk::jump(at_Obj_SpawnBoss);
}
