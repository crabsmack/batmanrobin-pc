#include "game.h"

/* the ship's death script after this native (the bytes after its jsr $7FE.w) */
static constexpr uint32_t Scr2_029454 = 0x029454u;
/* the radius object's death script: the expanding disc */
static constexpr uint32_t Scr2_0295D4 = 0x0295D4u;

static constexpr int BEAM_COLOUR = 15;              /* palette line 1 colour 15: the beam and disc */
static constexpr uint16_t BEAM_WHITE = 0xEEE;
static constexpr int SCENE_DISC_ONLY = 0xA2;        /* g_SceneVars+$A2: the callback $33A88 draws the disc alone */

/* Boss_BeamShip_StartDeathDisc - native in the defeated ship's script: turns the beam colour white,
 * gives the radius object the death script Scr2_0295D4 (an expanding disc) and tells the scene's
 * frame callback to draw the disc alone (no beam cone)
 * In:  ship = the ship (a6); the rest its registers
 * Out: palette line 1 colour 15 = $EEE (g_Pal1Reload set); the radius object resumes at
 *      Scr2_0295D4; g_SceneVars+$A2 = $FF; the script continues at Scr2_029454 (jsr $7FE.w)
 * Evidence: move.w #$EEE,g_PalSource1+$1E / st.b g_Pal1Reload / movea.l $22(a6),a0 / movea.w
 *           $A(a0),a0 / move.l #$295D4,$2A(a0) / st.b g_SceneVars+$A2 / jsr $7FE.w; Op2_Native at
 *           $029430 after the ship's death sequence; the callback $33A88 tests g_SceneVars+$A2;
 *           TAS f79013
 */
void Boss_BeamShip_StartDeathDisc(uint32_t d0, uint32_t d1, uint32_t d2, uint32_t d3, uint32_t d4, uint32_t d5, uint32_t d6,
                                  uint32_t d7, uint32_t a1, uint32_t a3, uint32_t a4, uint32_t a5, const BeamShipObj *ship)
{
    ((u16 *)g_PalSource1)[BEAM_COLOUR] = BEAM_WHITE;
    g_Pal1Reload = 0xFF;
    dk::handle<Vm2Block>(ship->radius->vm)->resume.v = Scr2_0295D4;
    g_SceneVars[SCENE_DISC_ONLY] = 0xFF;
    dk::jump_script(at_VM2_Enter, Scr2_029454);
}
