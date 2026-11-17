#include "game.h"

/* the options page's text-map command lists and its settings table (ROM) */
static constexpr uint32_t Tbl_0498EA = 0x0498EAu;   /* static texts: OPTIONS, CREDITS:, MUSIC TEST: ... */
static constexpr uint32_t Tbl_049948 = 0x049948u;   /* the values: credits, music and sound test numbers */
static constexpr uint32_t Tbl_049994 = 0x049994u;   /* Menu_Update's settings: g_StartCredits, g_OptMusicTest, g_OptSoundTest */

enum { PAD_HELD = 0, PAD_PRESSED = 2 };
enum { BTNS_ABC = 0x70 };                             /* bits 6 A, 5 C, 4 B */
enum { ITEM_MUSIC_TEST = 1, ITEM_SOUND_TEST = 2 };
enum { IDLE_FRAMES = 0x7080 };

static void set_low_word(uint32_t *r, uint16_t v) { *r = (*r & 0xFFFF0000u) | v; }
static void set_low_byte(uint32_t *r, uint8_t v) { *r = (*r & 0xFFFFFF00u) | v; }

/* Options_Init - opens the options page from the title menu: blanks the text map, puts the cursor
 * on CREDITS and draws the static texts (OPTIONS, CREDITS:, MUSIC TEST:, SOUND TEST:, PRESS START
 * TO EXIT) from Tbl_0498EA.
 * In:  the registers TextMap_Run passes through
 * Out: g_MenuCursor = 0; a0 = Tbl_0498EA, then TextMap_Run's results and condition codes
 * Evidence: jsr TextMap_Clear / clr.w g_MenuCursor / lea Tbl_0498EA(pc),a0 / jmp TextMap_Run;
 *           Op2_CallNativeRel from $49756 (OPTIONS chosen); scenario-options f468
 */
void Options_Init(uint32_t *d0, uint16_t *d6, uint32_t *d7, uint32_t *a0, uint32_t *a1, uint32_t *a2, uint32_t *a3,
                  uint32_t *a4, uint32_t *a5, uint32_t *a6)
{
    TextMap_Clear(d0, d7);
    g_MenuCursor = 0;
    *a0 = Tbl_0498EA;
    dk::jump(at_TextMap_Run);
}

/* Options_Step - options page per-frame native: the cursor and Left/Right edit the settings
 * (credits 0-2, music test 0-$F, sound test 0-$39), the values are drawn and sent to plane A, a
 * held button restarts the $7080-frame idle timeout, and A/B/C plays the music test song or the
 * sound test effect (number + 1).
 * In:  the registers the menu and text-map routines pass through
 * Out: the settings, g_MenuCursor, g_SceneTimer; d0 = the pressed A/B/C bits, or the song / effect
 *      played (Sound_RestartMusic / Snd_PlaySfx, jumped to)
 * Evidence: lea Tbl_049994(pc),a0 / Menu_Update / Tbl_049948 TextMap_Run / TextMap_Upload / tst.b
 *           g_Pad1 / g_Pad1+2 & $70, cursor 1 -> jmp Sound_RestartMusic, 2 -> jmp Snd_PlaySfx;
 *           Op2_CallNativeRel from $49760; scenario-options / x-options f468-f1449
 */
void Options_Step(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6,
                  uint32_t *d7, uint32_t *a1, uint32_t *a2, uint32_t *a3, uint32_t *a4, uint32_t *a5, uint32_t *a6)
{
    uint16_t w1 = (uint16_t)*d1;
    uint8_t b2 = (uint8_t)*d2;
    Menu_Update(d0, &w1, &b2, Tbl_049994, a1);
    set_low_word(d1, w1);
    set_low_byte(d2, b2);

    uint32_t a0 = Tbl_049948;
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

    set_low_byte(d0, (uint8_t)(g_Pad1[PAD_PRESSED] & BTNS_ABC));
    if ((uint8_t)*d0 == 0)
        return;
    uint16_t item = g_MenuCursor;
    set_low_word(d0, item);
    if (item == ITEM_MUSIC_TEST) {
        set_low_word(d0, (uint16_t)(g_OptMusicTest + 1));
        dk::jump(at_Sound_RestartMusic);
        return;
    }
    if (item == ITEM_SOUND_TEST) {
        set_low_word(d0, (uint16_t)(g_OptSoundTest + 1));
        dk::jump(at_Snd_PlaySfx);
    }
}
