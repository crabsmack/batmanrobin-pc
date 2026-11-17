#include "game.h"

/* the title menu's text-map command list (ROM): menu items 0/1 START, OPTIONS */
static constexpr uint32_t Tbl_0498AE = 0x0498AEu;

enum { PAD_HELD = 0 };   /* g_Pad1 +0: the buttons held */
enum { MENU_LAST_ITEM = 1, IDLE_FRAMES = 500 };

static void set_low_word(uint32_t *r, uint16_t v) { *r = (*r & 0xFFFF0000u) | v; }
static void set_low_byte(uint32_t *r, uint8_t v) { *r = (*r & 0xFFFFFF00u) | v; }

/* Title_MenuInit - opens the title menu: blanks the text map and puts the cursor on START.
 * In:  none
 * Out: g_MenuCursor = 0; TextMap_Clear's d0 / d7
 * Evidence: jsr TextMap_Clear / clr.w g_MenuCursor / rts; Op2_CallNativeRel from $4972E
 *           (Scr2_049720); TAS f404, scenario-options f404/f1449
 */
void Title_MenuInit(uint32_t *d0, uint32_t *d7)
{
    TextMap_Clear(d0, d7);
    g_MenuCursor = 0;
}

/* Title_MenuStep - title menu per-frame native: Up/Down move the cursor over START / OPTIONS, the
 * menu text map is drawn and sent to plane A, and any held button restarts the 500-frame idle
 * timeout.
 * In:  the registers the text-map routines pass through
 * Out: g_MenuCursor; g_SceneTimer = 500 when a pad 1 button is held; d1 = 1 until TextMap_Upload,
 *      then the text-map routines' registers
 * Evidence: moveq #1,d1 / jsr Menu_MoveCursor / lea Tbl_0498AE(pc),a0 / TextMap_Run / TextMap_Upload /
 *           tst.b g_Pad1 / move.w #$1F4,g_SceneTimer; Op2_CallNativeRel from $49738; runtime entry
 *           in the TAS and scenario-options
 */
void Title_MenuStep(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6,
                    uint32_t *d7, uint32_t *a1, uint32_t *a2, uint32_t *a3, uint32_t *a4, uint32_t *a5, uint32_t *a6)
{
    *d1 = MENU_LAST_ITEM;   /* moveq: the whole register */
    uint8_t b2 = (uint8_t)*d2;
    Menu_MoveCursor(d0, (uint16_t)*d1, &b2);
    set_low_byte(d2, b2);

    uint32_t a0 = Tbl_0498AE;
    uint16_t w6 = (uint16_t)*d6;
    uint16_t w7 = (uint16_t)*d7;
    TextMap_Run(d0, &w6, &w7, &a0, a1, a2, a3, a4, a5, a6);
    set_low_word(d7, w7);
    uint16_t w3 = (uint16_t)*d3, w4 = (uint16_t)*d4, w5 = (uint16_t)*d5;
    TextMap_Upload(d0, d1, d2, &w3, &w4, &w5, &w6, d7, a0);
    set_low_word(d3, w3);
    set_low_word(d4, w4);
    set_low_word(d5, w5);
    set_low_word(d6, w6);

    if (g_Pad1[PAD_HELD] != 0)
        g_SceneTimer = IDLE_FRAMES;
}
