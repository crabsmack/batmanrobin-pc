#include "game.h"

/* the death disc's script after this native (the bytes after its jsr $7FE.w) */
static constexpr uint32_t Scr2_02961E = 0x02961Eu;

static constexpr int BEAM_COLOUR = 15;          /* palette line 1 colour 15: the beam and disc */
static constexpr uint16_t BEAM_DARK = 0x222;

/* Boss_BeamShip_DeathFlash - native in the death disc script: the disc colour goes dark ($222),
 * then the big white flash (Fade_FlashWhite7, Tint_FlashC3) after the disc's 7 flashing pulses
 * In:  the radius object's registers
 * Out: palette line 1 colour 15 = $222 (g_Pal1Reload left as it was); the
 *      script continues at Scr2_02961E (jsr $7FE.w)
 * Evidence: move.w #$222,g_PalSource1+$1E / jsr Fade_FlashWhite7 / jsr Tint_FlashC3 / jsr $7FE.w;
 *           Op2_Native at $029606 in Scr2_0295D4 after a loop of 7 Fade_FlashWhite3 +
 *           Shake_StartBig; TAS f79050
 */
void Boss_BeamShip_DeathFlash(uint32_t d0, uint32_t d1, uint32_t d2, uint32_t d3, uint32_t d4, uint32_t d5, uint32_t d6,
                              uint32_t d7, uint32_t a1, uint32_t a3, uint32_t a4, uint32_t a5, uint32_t a6)
{
    ((u16 *)g_PalSource1)[BEAM_COLOUR] = BEAM_DARK;
    dk::Flags flags{};   /* its ccr is X only (move.b/clr.b/st.b keep it): no reader here */
    Fade_FlashWhite7(&flags);
    Tint_FlashC3();
    dk::jump_script(at_VM2_Enter, Scr2_02961E);
}
