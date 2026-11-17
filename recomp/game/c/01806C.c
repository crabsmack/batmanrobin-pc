#include "game.h"

/* the boss script after the native (the bytes after its jsr $7FE.w) */
static constexpr uint32_t Scr2_018076 = 0x018076u;

/* g_Stage1FireState: neither the sky wave nor the fire: Stage1_UpdateStreetScroll and
   Stage1_VIntFacade leave plane A alone */
static constexpr uint8_t STAGE1_FX_OFF = 0xFF;

/* Boss_Harley_StopStreetFx - native fragment at the head of the Harley boss script: turns off the
 * stage 1 street's plane-A fire/sky line effects, then resumes the script
 * In:  the boss object's registers (entered through Op2_Native at the head of Scr2_018066)
 * Out: g_Stage1FireState = $FF; the script continues at Scr2_018076 (jsr $7FE.w) with every
 *      register as it came
 * Evidence: Scr2_018066 = 437A 0001 07FC (native) then move.b #$FF,$F518 / jsr $7FE; TAS f4519,
 *           right after Boss_Harley_Spawn
 */
void Boss_Harley_StopStreetFx(uint32_t d0, uint32_t d1, uint32_t d2, uint32_t d3, uint32_t d4, uint32_t d5,
                              uint32_t d6, uint32_t d7, uint32_t a1, uint32_t a3, uint32_t a4, uint32_t a5,
                              uint32_t a6)
{
    g_Stage1FireState = STAGE1_FX_OFF;
    dk::jump_script(at_VM2_Enter, Scr2_018076);
}
