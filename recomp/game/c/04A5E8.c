#include "game.h"

/* the narrator's script after the native (the bytes after its jsr $7FE.w) */
static constexpr uint32_t Scr2_04A5FE = 0x04A5FEu;

static void set_low_word(uint32_t *r, uint16_t v) { *r = (*r & 0xFFFF0000u) | v; }

/* Ending_EpilogueTextStep - native of the epilogue narrator's loop: while a typewriter text is
 * running, continues it (TextMap_Run), then sends the text map to plane A and re-enters the script.
 * In:  the narrator's registers (TextMap_Run continues from its own state)
 * Out: plane A's text image; the script continues at Scr2_04A5FE with the registers the text-map
 *      routines leave
 * Evidence: tst.b g_TextMapTyping / beq / jsr TextMap_Run / jsr TextMap_Upload / jsr $7FE.w; every
 *           frame f19910-21790 in scenario-level-skip
 */
void Ending_EpilogueTextStep(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5,
                             uint32_t *d6, uint32_t *d7, uint32_t *a0, uint32_t *a1, uint32_t *a2, uint32_t *a3,
                             uint32_t *a4, uint32_t *a5, uint32_t *a6)
{
    uint16_t w6 = (uint16_t)*d6;
    if (g_TextMapTyping) {
        uint16_t w7 = (uint16_t)*d7;
        TextMap_Run(d0, &w6, &w7, a0, a1, a2, a3, a4, a5, a6);
        set_low_word(d7, w7);
    }
    uint16_t w3 = (uint16_t)*d3, w4 = (uint16_t)*d4, w5 = (uint16_t)*d5;
    TextMap_Upload(d0, d1, d2, &w3, &w4, &w5, &w6, d7, *a0);
    set_low_word(d3, w3);
    set_low_word(d4, w4);
    set_low_word(d5, w5);
    set_low_word(d6, w6);
    dk::jump_script(at_VM2_Enter, Scr2_04A5FE);
}
