#include "game.h"

/* jetpack Harley's defeat script after this native (the bytes after its jsr $7FE.w) */
static constexpr uint32_t Scr2_01EFCE = 0x01EFCEu;

/* Boss_HarleyJet_SetBeatenFlag - native fragment at the start of jetpack Harley's defeat: sets the
 * beaten flag (g_SceneVars byte 0, read by Boss_Claw_BranchHarleyBeaten), then back into the script
 * at Scr2_01EFCE (BossMeter_Defeated, she flies off)
 * In:  the running object's registers (for the script); entered by Op2_Native at $1EFC4
 * Out: g_SceneVars[0] = $FF; the script continues at Scr2_01EFCE (jsr $7FE.w)
 * Evidence: st.b g_SceneVars / jsr $7FE.w; TAS f12637 when her energy reached 0 (RAM f12660)
 */
void Boss_HarleyJet_SetBeatenFlag(uint32_t d0, uint32_t d1, uint32_t d2, uint32_t d3, uint32_t d4, uint32_t d5,
                                  uint32_t d6, uint32_t d7, uint32_t a1, uint32_t a3, uint32_t a4, uint32_t a5,
                                  uint32_t a6)
{
    g_SceneVars[0] = 0xFF;            /* st.b */
    dk::jump_script(at_VM2_Enter, Scr2_01EFCE);
}
