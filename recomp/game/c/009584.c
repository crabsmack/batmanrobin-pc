#include "game.h"

enum {
    FLASH_FRAMES = 4,   /* the white flash's length */
};

/* Pal_FlashWhite - native callback: flashes the screen white for 4 frames (the V-int's palette
 * job $A7F0 fills CRAM with $0EEE while g_FlashReq is set, and restores the palette when the timer
 * runs out).
 * In:  none
 * Out: g_FlashReq = $FF, g_FlashTimer = 4; ccr = the move.b #4's (N Z V C clear)
 * Evidence: st.b g_FlashReq / move.b #4,g_FlashTimer / rts; 21 script uses after Op2_CallNative
 *           ($13788, $1992C...); the a4 op $6492 does the same. Prototype: &ccr added (its ccout)
 */
void Pal_FlashWhite(dk::Flags *ccr)
{
    g_FlashReq = 0xFF;
    g_FlashTimer = FLASH_FRAMES;
    *ccr = dk::Flags::test(FLASH_FRAMES, 1, ccr->x);
}
