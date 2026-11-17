#include "game.h"

/* the continue screen's text-map command list (ROM: 'CREDITS ' + the g_Credits number, the
   blinking PRESS START TO CONTINUE) and the script after the native (Op2_Return) */
static constexpr uint32_t Tbl_049428 = 0x049428u;
static constexpr uint32_t Scr2_049232 = 0x049232u;

enum {
    TEXT_ROWS_MINUS1 = 2,      /* TextMap_UploadRows' d7: 3 rows */
    TEXT_ROW_OFFSET = 0xC00,   /* row 24 of plane B (64 cells x 2 bytes a row) */
};

static void set_low_word(uint32_t *r, uint16_t v) { *r = (*r & 0xFFFF0000u) | v; }

/* Continue_DrawCreditsText - per-frame text of the continue screen: runs the text map ('CREDITS
 * n', the blinking PRESS START TO CONTINUE) and DMAs 3 rows of the image to plane B row 24, then
 * back into the script (Op2_Return: the coroutine restarts here next frame).
 * In:  the controller's registers (coroutine 1 of Scr2_0491A0)
 * Out: plane B's text rows; the script continues at Scr2_049232 with the registers the text-map
 *      routines leave
 * Evidence: lea Tbl_049428,a0 / TextMap_Run / moveq #2,d7 / move.w g_PlaneBVRAM,d2 / addi.w #$C00 /
 *           TextMap_UploadRows / jsr $7FE.w; Op2_Native at $49210; png scenario-game-over f42600
 *           ('CREDITS 6'); f41999-f42598
 */
void Continue_DrawCreditsText(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5,
                              uint32_t *d6, uint32_t *d7, uint32_t *a1, uint32_t *a2, uint32_t *a3, uint32_t *a4,
                              uint32_t *a5, uint32_t *a6)
{
    uint32_t a0 = Tbl_049428;
    uint16_t w6 = (uint16_t)*d6, w7 = (uint16_t)*d7;
    TextMap_Run(d0, &w6, &w7, &a0, a1, a2, a3, a4, a5, a6);
    *d7 = TEXT_ROWS_MINUS1;
    set_low_word(d2, (uint16_t)(g_PlaneBVRAM + TEXT_ROW_OFFSET));
    uint16_t w3 = (uint16_t)*d3, w4 = (uint16_t)*d4, w5 = (uint16_t)*d5;
    w7 = TEXT_ROWS_MINUS1;
    TextMap_UploadRows(d0, d1, d2, &w3, &w4, &w5, &w6, &w7, a0);
    set_low_word(d3, w3);
    set_low_word(d4, w4);
    set_low_word(d5, w5);
    set_low_word(d6, w6);
    set_low_word(d7, w7);
    dk::jump_script(at_VM2_Enter, Scr2_049232);
}
