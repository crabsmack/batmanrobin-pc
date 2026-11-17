#include "game.h"

/* the boss script after the native (the bytes after its jsr $7FE.w) */
static constexpr uint32_t Scr2_025DE0 = 0x025DE0u;

/* Boss_Stage2_ShowWreck - boss defeat native (after sound 3 and Fade_FlashWhite6): plane B shows
 * the wrecked hull, the other half of its picture; then the hull effects and the end ladder
 * In:  the running object's registers (for the script)
 * Out: g_Stage2BossPlaneAlt = $FF (st.b); the script continues at Scr2_025DE0 (jsr $7FE.w)
 * Evidence: st.b g_Stage2BossPlaneAlt / jsr $7FE.w; TAS f34683 (one frame); png f034800/f035400:
 *           the wreck with flames
 */
void Boss_Stage2_ShowWreck(uint32_t d0, uint32_t d1, uint32_t d2, uint32_t d3, uint32_t d4, uint32_t d5, uint32_t d6,
                           uint32_t d7, uint32_t a1, uint32_t a3, uint32_t a4, uint32_t a5, uint32_t a6)
{
    g_Stage2BossPlaneAlt = 0xFF;
    dk::jump_script(at_VM2_Enter, Scr2_025DE0);
}
