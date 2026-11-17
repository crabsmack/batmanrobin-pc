#include "game.h"

/* the boss script after the native (the bytes after its jsr $7FE.w) */
static constexpr uint32_t Scr2_025C38 = 0x025C38u;

/* Boss_Stage2_StartFight - boss script native once the four turrets exist: allows the floor
 * tilts and shows the hull picture on plane B, then back into the script (music $10, energy
 * $D00/$F80, BossMeter_Show)
 * In:  the running object's registers (for the script)
 * Out: g_Stage2TiltAllowed = g_Stage2BossPlaneOn = $FF (st.b); the script continues at
 *      Scr2_025C38 (jsr $7FE.w)
 * Evidence: st.b g_Stage2TiltAllowed / st.b g_Stage2BossPlaneOn / jsr $7FE.w; Op2_Native at
 *           $025C2A; TAS f32140 (one frame), when the hull appears (png f032400)
 */
void Boss_Stage2_StartFight(uint32_t d0, uint32_t d1, uint32_t d2, uint32_t d3, uint32_t d4, uint32_t d5, uint32_t d6,
                            uint32_t d7, uint32_t a1, uint32_t a3, uint32_t a4, uint32_t a5, uint32_t a6)
{
    g_Stage2TiltAllowed = 0xFF;
    g_Stage2BossPlaneOn = 0xFF;
    dk::jump_script(at_VM2_Enter, Scr2_025C38);
}
