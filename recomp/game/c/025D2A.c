#include "game.h"

/* the boss coroutine after the native (the bytes after its jsr $7FE.w) */
static constexpr uint32_t Scr2_025D32 = 0x025D32u;

/* Boss_Stage2_EndPlayerTilts - boss phase-2 coroutine native: the players may no longer tilt the
 * floor (Stage2_ArenaRequestTilt refuses); from here the coroutine forces a tilt itself every
 * $690 frames (op $2CB2 into $0241DE, past that check)
 * In:  the running object's registers (for the script)
 * Out: g_Stage2TiltAllowed = 0; the script continues at Scr2_025D32 (jsr $7FE.w)
 * Evidence: clr.b g_Stage2TiltAllowed / jsr $7FE.w; Scr2_025D32: Op2_SetTimer3 $690, tick loop,
 *           op $2CB2 offset $E49C = $0241DE; TAS f33526 (one frame)
 */
void Boss_Stage2_EndPlayerTilts(uint32_t d0, uint32_t d1, uint32_t d2, uint32_t d3, uint32_t d4, uint32_t d5, uint32_t d6,
                                uint32_t d7, uint32_t a1, uint32_t a3, uint32_t a4, uint32_t a5, uint32_t a6)
{
    g_Stage2TiltAllowed = 0;
    dk::jump_script(at_VM2_Enter, Scr2_025D32);
}
