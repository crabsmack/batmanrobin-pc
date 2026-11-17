#include "game.h"

/* the copyright notice's text-map command list (ROM) and the script after the native (the bytes
   after its jsr $7FE.w: Op2_Kill) */
static constexpr uint32_t Tbl_049FB8 = 0x049FB8u;
static constexpr uint32_t Scr2_049FB6 = 0x049FB6u;

static void set_low_word(uint32_t *r, uint16_t v) { *r = (*r & 0xFFFF0000u) | v; }

/* Copyright_DrawText - native of the copyright notice object: draws the notice (BATMAN AND ALL
 * RELATED ELEMENTS / ARE THE PROPERTY OF DC COMICS / TM & (C)1995 / (C)1995 SEGA) from its text map,
 * sends it to plane A, then back into the script, whose next op kills the object.
 * In:  the object's registers
 * Out: plane A's text image; the script continues at Scr2_049FB6 (Op2_Kill) with the registers the
 *      text-map routines leave
 * Evidence: lea Tbl_049FB8,a0 / jsr TextMap_Run / jsr TextMap_Upload / jsr $7FE.w; Op2_Native at
 *           $49F9E in Scr2_049F94; once per attract pass (TAS f164)
 */
void Copyright_DrawText(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5,
                        uint32_t *d6, uint32_t *d7, uint32_t *a1, uint32_t *a2, uint32_t *a3, uint32_t *a4,
                        uint32_t *a5, uint32_t *a6)
{
    uint32_t a0 = Tbl_049FB8;
    uint16_t w6 = (uint16_t)*d6, w7 = (uint16_t)*d7;
    TextMap_Run(d0, &w6, &w7, &a0, a1, a2, a3, a4, a5, a6);
    set_low_word(d7, w7);
    uint16_t w3 = (uint16_t)*d3, w4 = (uint16_t)*d4, w5 = (uint16_t)*d5;
    TextMap_Upload(d0, d1, d2, &w3, &w4, &w5, &w6, d7, a0);
    set_low_word(d3, w3);
    set_low_word(d4, w4);
    set_low_word(d5, w5);
    set_low_word(d6, w6);
    dk::jump_script(at_VM2_Enter, Scr2_049FB6);
}
