#include "game.h"

/* the snow globe's a2 script (ROM) */
static constexpr uint32_t Scr2_04A658 = 0x04A658u;

/* Ending_SpawnSnowGlobe - spawns the Freeze-cell ending's snow globe (Scr2_04A658): it starts below
 * the screen at (190, 310), jumps up 140 px, then hovers on an oscillator, animating six RAM-bank
 * frames
 * In:  x, y = the spawn position (Obj_SpawnEnemy's d4/d5; the script sets its own)
 * Out: script (a2) = Scr2_04A658; Obj_SpawnEnemy's results
 * Evidence: lea Scr2_04A658,a2 / jmp Obj_SpawnEnemy; entered by the narrator script's
 *           CallNativeRelSaved $FEAC at $4A79E after the second text block; f16099. The prototype
 *           hands a2 on (measured as clobbered: the callers don't read it, but the jump's target
 *           does)
 */
void Ending_SpawnSnowGlobe(uint32_t *d0, uint16_t *d1, uint16_t x, uint16_t y, uint32_t *d7, uint32_t *script,
                           uint32_t a4, uint32_t a6)
{
    *script = Scr2_04A658;
    dk::jump(at_Obj_SpawnEnemy);
}
