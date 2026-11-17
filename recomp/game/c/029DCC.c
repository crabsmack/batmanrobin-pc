#include "game.h"

/* the object's a2 script (ROM data right after this spawner's code; batmanrobin.asm) */
static constexpr uint32_t Scr2_029DDC = 0x029DDCu;

/* Enemy_Missile_SpawnFromJet - spawns a homing missile (Scr2_029DDC: cruise $2AA, turn rate $700,
 * body Scr2_0287B0) and sets its heading, straight time and homing time from the launcher's
 * g_VM2Arg0-2
 * In:  x/y = where it starts (d4/d5)
 * Out: the missile's heading and times set (Enemy_Missile_InitFromArgs: d0 = 0); a short pool
 *      returns past this to the launcher's script (Obj_SpawnEnemyOrQuit)
 * Evidence: lea Scr2_029DDC,a2 / jsr Obj_SpawnEnemyOrQuit / jmp Enemy_Missile_InitFromArgs; dc.l in
 *           the jet script at $29DC2 (Spawn_AtOwnOfs, offset $14,0); jmp
 *           Enemy_Missile_InitFromArgs; TAS 43185-61879 (16 frames). The prototype gains
 *           &a2/&a3/&a4/&a6 (measured clobbers: no caller reads them) so the jump hands the new
 *           missile's blocks on
 */
void Enemy_Missile_SpawnFromJet(uint32_t *d0, uint16_t *d1, uint16_t x, uint16_t y, uint32_t *d7, uint32_t *script, uint32_t *a3, uint32_t *a4, uint32_t *a6)
{
    /* Obj_Spawn sets a0 (the run list) and a5 itself: the values going in are not read */
    uint32_t list = 0, sprite = 0;
    *script = Scr2_029DDC;
    Obj_SpawnEnemyOrQuit(d0, d1, x, y, d7, &list, *script, a3, a4, &sprite, a6);
    dk::jump(at_Enemy_Missile_InitFromArgs);   /* its a3/a4 = the new missile's blocks */
}
