#include "game.h"

/* the object's a2 script (ROM data right after this spawner's code; batmanrobin.asm) */
static constexpr uint32_t Scr2_032AB6 = 0x032AB6u;

/* Boss_DomeCannon_SpawnDomeLeft - spawns one of the dome cannon's left-facing missile domes as an
 * unlinked child: spawn action $2D, speed $E6, the shared dome body $2FC56 (H-flipped, energy
 * $150/$300); the parent starts its launch script (coroutine 1) from $32ADA-$32D46
 * In:  x/y = the parent's position (Op2_SpawnChildOrWait)
 * Out: script = Scr2_032AB6, then Obj_SpawnEnemyUnlinked: Z set and a6 = the new dome when it
 *      spawned, Z clear when the pool is short
 * Evidence: lea Scr2_032AB6,a2 / jmp Obj_SpawnEnemyUnlinked; dc.l of Op2_SpawnChildOrWait (fields
 *           $2C/$2E) in Scr2_031B4E, Scr2_031D3C, Scr2_0320EA; RAM f70080: two dome bodies (pc
 *           $2FC86) beside the core; TAS 70036-75008. The prototype gains &a2 (measured a clobber:
 *           Op2_SpawnChildOrWait saves and restores a2 around its jsr (a0)) so the jump hands the
 *           script on
 */
void Boss_DomeCannon_SpawnDomeLeft(uint16_t x, uint16_t y, uint32_t *d7, uint32_t *script, uint32_t a4, uint32_t *a6)
{
    *script = Scr2_032AB6;
    dk::jump(at_Obj_SpawnEnemyUnlinked);   /* jmp: it returns to Op2_SpawnChildOrWait */
}
