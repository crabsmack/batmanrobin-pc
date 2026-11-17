#include "game.h"

/* the ladder's script after this native (the bytes after its jsr $7FE.w): hung from the shaft
 * top at x $A8, y -$F0 and moved in screen space */
static constexpr uint32_t Scr2_026626 = 0x026626u;

/* Enemy_LadderThug_PickAnchor - native at the start of the rope-ladder body: while the boss hull
 * is shown (g_Stage2BossPlaneOn) the ladder hangs from the hull (Enemy_LadderThug_AnchorHull),
 * else from the top of the lift shaft
 * In:  the running ladder object's registers (a2 = its script, after this native's jsr $7FE.w)
 * Out: the script continues at Scr2_026626, or at the hull exit's (jsr $7FE.w)
 * Evidence: tst.b g_Stage2BossPlaneOn / bne.s Enemy_LadderThug_AnchorHull / jsr $7FE.w; TAS
 *           f24982-f34742 (8 frames, one per ladder); png f025250: a thug on a rope ladder
 *           hanging into the lift shaft
 */
void Enemy_LadderThug_PickAnchor(uint32_t d0, uint32_t d1, uint32_t d2, uint32_t d3, uint32_t d4, uint32_t d5, uint32_t d6,
                                 uint32_t d7, uint32_t a1, uint32_t a3, uint32_t a4, uint32_t a5, uint32_t a6)
{
    if (g_Stage2BossPlaneOn != 0) {
        dk::jump(at_Enemy_LadderThug_AnchorHull);   /* bne: the next function file */
        return;
    }
    dk::jump_script(at_VM2_Enter, Scr2_026626);
}
