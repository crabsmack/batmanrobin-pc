#include "game.h"

/* The white flashes ($00AA9C-$00AB23): CRAM goes white for 2 frames (Pal_VIntUpload, while
 * g_FlashTimer runs), then the palette fades in from full white (+$E) at the given speed. */

/* one flash: g_FadeDelay = frames per fade step - 1; the ccr of the last move.b (#2: all clear) */
static void flash_white(uint8_t delay, dk::Flags *ccr)
{
    g_FadeDelayCount = delay;
    g_FadeDelay = delay;
    g_FadeLevel = 0x0E;
    g_FadeTarget = 0;
    g_FlashReq = 0xFF;
    g_FlashTimer = 2;
    *ccr = dk::Flags::test(2, 1, ccr->x);
}

/* Fade_FlashWhite6 - white flash, then the fade in from white at one step per 6 frames.
 * In:  none
 * Out: g_FadeDelay = g_FadeDelayCount = 5, g_FadeLevel = $E, g_FadeTarget = 0, g_FlashReq set,
 *      g_FlashTimer = 2; ccr N/Z/V/C clear
 * Evidence: move.b #5,g_FadeDelayCount / move.b #5,g_FadeDelay / move.b #$E,g_FadeLevel / clr.b
 *           g_FadeTarget / st.b g_FlashReq / move.b #2,g_FlashTimer; pointers in 8 stage/boss
 *           scripts (a2 natives); 19 corpus runs, TAS 7 frames from 34683
 */
void Fade_FlashWhite6(dk::Flags *ccr)
{
    flash_white(5, ccr);
}

/* Fade_FlashWhite7 - white flash, then the fade in from white at one step per 7 frames.
 * In:  none
 * Out: as Fade_FlashWhite6 with delay 6
 * Evidence: the same six stores with #6; jsr from the native $029608 (with Tint_FlashC3) and a
 *           pointer at $03CC60; not run in the corpus
 */
void Fade_FlashWhite7(dk::Flags *ccr)
{
    flash_white(6, ccr);
}

/* Fade_FlashWhite3 - white flash, then the fade in from white at one step per 3 frames.
 * In:  none
 * Out: as Fade_FlashWhite6 with delay 2
 * Evidence: the same six stores with #2; pointers at $01D8A6, $026B4C, $0295F8 ... (script
 *           natives); not run in the corpus
 */
void Fade_FlashWhite3(dk::Flags *ccr)
{
    flash_white(2, ccr);
}
