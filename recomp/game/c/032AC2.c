#include "game.h"

/* the object's a2 script (ROM data right after this spawner's code; batmanrobin.asm) */
static constexpr uint32_t Scr2_032ACE = 0x032ACEu;

/* Boss_DomeCannon_SpawnDomeRight - spawns a right-facing missile dome of the dome cannon's third
 * form as an unlinked child: spawn action $2D, dome body $2FC6C (palette 1 without the H flip); the
 * script that follows ($32ADA-$32D4D) holds the domes' launch patterns: fans of
 * Enemy_Missile_SpawnSlow to the left ($250/$200/$1B0) or right ($FF80/0/$50) and
 * Enemy_Missile_SpawnFast
 * In:  x/y = the parent's position (Op2_SpawnChildOrWait)
 * Out: script = Scr2_032ACE, then Obj_SpawnEnemyUnlinked: Z set and a6 = the new dome when it
 *      spawned, Z clear when the pool is short
 * Evidence: lea Scr2_032ACE,a2 / jmp Obj_SpawnEnemyUnlinked; dc.l of Op2_SpawnChildOrWait (fields
 *           $30/$26) in Scr2_0320EA only; TAS f75008. The prototype gains &a2 (measured a clobber:
 *           Op2_SpawnChildOrWait saves and restores a2 around its jsr (a0)) so the jump hands the
 *           script on
 */
void Boss_DomeCannon_SpawnDomeRight(uint16_t x, uint16_t y, uint32_t *d7, uint32_t *script, uint32_t a4, uint32_t *a6)
{
    *script = Scr2_032ACE;
    dk::jump(at_Obj_SpawnEnemyUnlinked);   /* jmp: it returns to Op2_SpawnChildOrWait */
}
