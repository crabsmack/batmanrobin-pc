#include "game.h"

/* Spr_ChangeFrame ($007EF6-$007F5D), which branches into Spr_SwapFrameArt (the next function
 * file) when the new frame uses other art.
 *
 * The original parks the caller's a1 in usp (move.l a1,usp) and every exit, its own and
 * Spr_SwapFrameArt's, restores it (move.l usp,a1): a1 comes back unchanged. The C does the same
 * (dk::set_usp / dk::usp, kit 0.9.5) and jumps into Spr_SwapFrameArt with the old entry in a1. */

static uint32_t sext16(uint16_t w) { return (uint32_t)(int32_t)(int16_t)w; }

template <class T> static T *at(uint32_t a)
{
    PTR(T) p;
    p.v = a;
    return p;
}

static constexpr uint32_t SPR_RAM_FRAMES = 0xFFFFA764u;   /* frames < 0: back from g_ArtRelRingWr */

/* a frame number's entry: >= 0 in the ROM table, < 0 in the RAM one */
static uint32_t frame_entry(uint16_t frame)
{
    return ((int16_t)frame >= 0 ? SPR_ROM_FRAMES : SPR_RAM_FRAMES) + sext16(frame);
}

/* Spr_ChangeFrame - switches a sprite from frame `old` to frame `frame`, loading the new frame's
 * art into VRAM (Spr_SwapFrameArt) when its art handle differs from the old one's.
 * In:  frame (d1.w); old (d2.w); byte offsets: >= 0 into Tbl_1DDB60, < 0 into the RAM table
 *      below $A764
 * Out: entry = the frame-table entry to draw (a0); frame = the frame now shown (d1: old when the
 *      art could not be loaded this frame); d0 = the new entry's handle address word (and what
 *      Spr_SwapFrameArt leaves in d0/d2/d3); a1 unchanged
 * Evidence: move.l a1,usp / tst.w d1 / bmi / tst.w d2 / bmi, the four lea (Tbl_1DDB60).l /
 *           (g_ArtRelRingWr).w + adda.w combinations, move.w (a0),d0 / cmp.w (a1),d0 /
 *           bne.s Spr_SwapFrameArt / move.l usp,a1; 37 call sites (the a2 VM sprite ops)
 */
void Spr_ChangeFrame(uint32_t *d0, uint32_t *frame, uint32_t *old, uint32_t *d3, uint32_t *entry, uint32_t *a1)
{
    dk::set_usp(*a1);                                 /* move.l a1,usp */
    *entry = frame_entry((uint16_t)*frame);
    uint32_t old_entry = frame_entry((uint16_t)*old);
    *d0 = (*d0 & 0xFFFF0000u) | (uint16_t)at<const SprFrame>(*entry)->art_handle;
    if ((uint16_t)*d0 != (uint16_t)at<const SprFrame>(old_entry)->art_handle) {
        *a1 = old_entry;                              /* bne.s Spr_SwapFrameArt, a1 = the old entry */
        dk::jump(at_Spr_SwapFrameArt);
        return;
    }
    *a1 = dk::usp();                                  /* move.l usp,a1 */
}
