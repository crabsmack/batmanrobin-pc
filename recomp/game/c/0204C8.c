#include "game.h"

/* the object's a2 script (ROM, right after this spawner's code) */
static constexpr uint32_t Scr2_0204D4 = 0x0204D4u;

/* Stage1_SpawnSectionExit - wave-script spawner: spawns the section exit Scr2_0204D4, which shows the GO! sign
 * (keeping the camera lock) and fades the scene out when a player touches its box
 * In:  d4/d5 = x/y; the wave director's registers (wave script Tbl_0174F0)
 * Out: script (a2) = Scr2_0204D4, then as Obj_SpawnEnemy (d0 = 0 and a6 = the object, Z set; d0
 *      nonzero when the pool is short)
 * Evidence: lea Scr2_0204D4,a2 / jmp Obj_SpawnEnemy; dc.l at $017862 in Tbl_0174F0; the script: Op2_CallNative
 *           HUD_StartGoSignKeepLock, an Op2_BranchTouchBox (-15,-25,15,25) loop, Op2_CallNative
 *           Scene_FadeOutExit, Op2_Kill; TAS f9596, 19 frames before the next scene $1FD76 (f9615)
 */
void Stage1_SpawnSectionExit(uint16_t *d1, uint16_t d4, uint16_t d5, uint32_t *d7, uint32_t *script, uint32_t *a3, uint32_t *a4,
                             uint32_t *a6)
{
    *script = Scr2_0204D4;
    dk::jump(at_Obj_SpawnEnemy);
}
