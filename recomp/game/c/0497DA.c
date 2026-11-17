#include "game.h"

enum { PAD_PRESSED = 2, BTN_START = 0x80 };   /* g_Pad1 +2: newly pressed; bit 7 Start */

/* Title_PaletteFadeRun - handler of the title palette fade-in: Start (pad 1, newly pressed) loads
 * palette lines 0-2 at full brightness from the title palette and frees the object; otherwise each
 * of the 7 colour-range channels steps (Title_PaletteFadeChannel).
 * In:  self = the fade object
 * Out: Start: lines 0-2 loaded (a0 past them), the object unlinked and freed (Obj_UnlinkFree's
 *      results); else the channels' registers (Pal_CopyBright's), d7 = $FFFF
 * Evidence: btst #7,g_Pad1+2 / movea.l $A(a6),a0 / Pal_LoadLine0-2 / jmp Obj_UnlinkFree; else
 *           lea $E(a6),a5 / moveq #6,d7 / move.w d7,-(a7) / bsr Title_PaletteFadeChannel /
 *           move.w (a7)+,d7 / dbra; TAS f404-f405 (Start at once), idle 4680 frames
 */
void Title_PaletteFadeRun(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d6, uint32_t *d7, uint32_t *a0,
                          uint32_t *a1, uint32_t *a2, uint32_t *a3, uint32_t *a4, TitleFadeObj *self)
{
    if (g_Pad1[PAD_PRESSED] & BTN_START) {
        *a0 = self->palette.raw();
        dk::Flags cc{};   /* the condition codes nothing here reads */
        Pal_LoadLine0(a0, a1, &cc);   /* each copies one line, a0 moving on to the next */
        Pal_LoadLine1(a0, a1, &cc);
        Pal_LoadLine2(a0, a1, &cc);
        dk::jump(at_Obj_UnlinkFree);
        return;
    }
    /* moveq #6,d7 sets the whole register; the channel's Pal_CopyBright counts only its low word,
       which the move.w (a7)+ restores, so the loop leaves d7 = $0000FFFF */
    for (int i = 0; i < 7; i++)
        Title_PaletteFadeChannel(d0, d1, d2, d6, a0, a1, a2, a3, a4, &self->ch[i], self);
    *d7 = 0x0000FFFFu;
}
