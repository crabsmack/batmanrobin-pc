#include "game.h"

/* the a2 script the spawned object runs (ROM data right after this spawner's code; batmanrobin.asm) */
static constexpr uint32_t Scr2_01B640 = 0x01B640u;

/* Enemy_Clown_SpawnWeak - spawns a striped-shirt clown with the low energy $20 (palette line 3)
 * that drops in from above
 * In:  d4/d5 = x/y (street trigger Tbl_015F5A, also Tbl_0166EA +$2B0)
 * Out: script = Scr2_01B640, then Obj_SpawnEnemy's (d0 = 0 and a6 = the new object; Z clear
 *      when the pool is short)
 * Evidence: lea Scr2_01B640,a2 / jmp Obj_SpawnEnemy; Scr2_01B640 = ModSprAttr $98/$60, score
 *           $64, SetEnergy $20, coroutine 0 $1BAD6 (op $1CDC: FallAnim without the 1-px nudge,
 *           then anim $258); street clowns (energy 32, waiting above the screen at y < 0) in
 *           RAM f2160-f2220; shot f1260 (clowns in pointed hats); TAS f622-f9231 (23 frames)
 */
void Enemy_Clown_SpawnWeak(uint16_t *d1, uint16_t d4, uint16_t d5, uint32_t *d7, uint32_t *a0,
                           uint32_t *script, uint32_t *a3, uint32_t *a4, uint32_t *a5, uint32_t *a6)
{
    *script = Scr2_01B640;
    dk::jump(at_Obj_SpawnEnemy);
}
