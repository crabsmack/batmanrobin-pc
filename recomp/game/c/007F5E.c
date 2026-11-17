#include "game.h"

/* Spr_SwapFrameArt ($007F5E-$007FFF), the tail of Spr_ChangeFrame. */

static uint32_t sext16(uint16_t w) { return (uint32_t)(int32_t)(int16_t)w; }
static void set_lo(uint32_t *r, uint16_t v) { *r = (*r & 0xFFFF0000u) | v; }

template <class T> static T *at(uint32_t a)
{
    PTR(T) p;
    p.v = a;
    return p;
}

/* the swap itself: every exit of it then restores a1 from usp */
static void swap_art(uint32_t *d0, uint32_t *frame, uint32_t *old, uint32_t *d3, uint32_t *entry, uint32_t old_entry)
{
    uint16_t new_frame = (uint16_t)*frame;            /* move.w d1,-(a7) */
    if (new_frame != 0) {
        uint32_t art = at<const SprFrame>(*entry)->art;   /* a4 */
        uint32_t handle = sext16((uint16_t)*d0);
        set_lo(d0, *at<u16>(handle));
        if ((uint16_t)*d0 != 0) {
            /* resident: one more user */
            ArtDesc *desc = dk::handle<ArtDesc>((int16_t)(uint16_t)*d0);
            int8_t was = desc->refcount;
            desc->refcount = (int8_t)(was - 1);
            if (was == 0) {                           /* released: in use again */
                *d0 = (*d0 & ~0xFFu) | desc->blocks;
                g_ArtBlocksFree = (uint8_t)(g_ArtBlocksFree - desc->blocks);
                g_ArtDescsFree = (uint8_t)(g_ArtDescsFree - 1);
            } else if ((int8_t)(was - 1) >= 0) {
                desc->blocks = (uint8_t)(desc->blocks + 1);
            }
        } else {
            const u16 *header = at<const u16>(art);   /* word DMA cost, word chunks (= blocks) */
            *d0 = sext16(header[0]);
            *frame = sext16(header[1]);
            uint16_t chunks = (uint16_t)*frame;
            set_lo(d0, (uint16_t)((uint16_t)*d0 + chunks));
            bool short_of = (int16_t)(uint16_t)*d0 > (int16_t)(uint16_t)g_DMABudgetLeft
                            || (int16_t)chunks > (int16_t)(uint16_t)g_DMAQueueFree;
            if (!short_of) {
                if (g_ArtBlocksFree < (uint8_t)chunks) {
                    short_of = true;                  /* the sub.b borrowed: given back */
                } else if (g_ArtDescsFree == 0) {
                    short_of = true;                  /* clamped at 0, the blocks given back */
                } else {
                    g_ArtBlocksFree = (uint8_t)(g_ArtBlocksFree - (uint8_t)chunks);
                    g_ArtDescsFree = (uint8_t)(g_ArtDescsFree - 1);
                    uint16_t blocks = chunks;
                    uint32_t d4 = 0, d5 = 0, d6 = 0, d7 = 0, a1 = old_entry, a3 = 0, a4 = art;
                    ArtCache_Alloc(&blocks, old, d3, &d4, &d5, &d6, &d7, handle, &a1, &a3);
                    Spr_QueueArtDMA(&blocks, old, d3, &d7, &a1, &a3, &a4);
                }
            }
            if (short_of) {
                *entry = old_entry;                   /* keep the old frame */
                set_lo(frame, (uint16_t)*old);
                return;
            }
        }
        set_lo(frame, new_frame);                     /* move.w (a7)+,d1 */
    }

    /* release the old frame's art */
    uint32_t old_handle = sext16(at<const SprFrame>(old_entry)->art_handle);
    set_lo(d0, *at<u16>(old_handle));
    if ((uint16_t)*d0 == 0)
        return;
    ArtDesc *desc = dk::handle<ArtDesc>((int16_t)(uint16_t)*d0);
    int8_t users = (int8_t)(desc->refcount + 1);
    desc->refcount = users;
    if (users != 0) {
        if (users > 0)
            desc->refcount = 0;                       /* was not in use */
        return;
    }
    g_ArtDescsFree = (uint8_t)(g_ArtDescsFree + 1);
    *old = (*old & ~0xFFu) | desc->blocks;
    g_ArtBlocksFree = (uint8_t)(g_ArtBlocksFree + desc->blocks);
    if (desc->ring_slot == 0) {                       /* onto the release ring */
        set_lo(old, g_ArtRelRingWr);
        g_ArtRelRingWrLo = (uint8_t)(g_ArtRelRingWrLo + 2);
        desc->ring_slot = (uint16_t)*old;
        *dk::handle<u16>((int16_t)(uint16_t)*old) = (uint16_t)*d0;
    }
}

/* Spr_SwapFrameArt - acquires VRAM art for a sprite's new frame and releases the old frame's: a
 * resident handle just takes a reference (revived from the cache when released), otherwise the
 * art gets blocks (ArtCache_Alloc) and its DMA is queued (Spr_QueueArtDMA). When the DMA budget,
 * the DMA queue, the free blocks or the free descriptors are short, the sprite keeps its old
 * frame and nothing is released.
 * In:  entry = the new frame's entry (a0); d0.w = its handle address; frame = the new frame (d1.w;
 *      0 = none: only the release); old = the old frame (d2.w); a1 = the old frame's entry; usp =
 *      the a1 Spr_ChangeFrame parked there
 * Out: entry and frame kept, or the old entry and frame = old (d1.w = d2.w) on a failure; d0, d2,
 *      d3 as the paths leave them (the art header's cost + chunks, ArtCache_Alloc's and
 *      Spr_QueueArtDMA's registers, the release's block count / ring slot); a1 = usp (every exit:
 *      Spr_ChangeFrame's caller gets its a1 back). d4-d7, a2-a4 are saved and restored (movem.l)
 * Evidence: movem.l d4-d7/a0-a4,-(a7) / move.w d1,-(a7) / beq, movea.l $6(a0),a4, movem.w
 *           (a4),d0-d1 / add.w d1,d0 / cmp.w (g_DMABudgetLeft).w,d0 / bgt / cmp.w
 *           (g_DMAQueueFree).w,d1 / bgt, sub.b d1,(g_ArtBlocksFree).w / subq.b
 *           #1,(g_ArtDescsFree).w, bsr.w ArtCache_Alloc / bsr.w Spr_QueueArtDMA, the release
 *           (addq.b #1,(a1)+ / bne / bmi) and the fail exit (movea.l a1,a0 / move.w d2,d1); the
 *           resident path's addq.b #1,(a0) after the (a0)+ bumps the block count (kept); the three
 *           `move.l usp,a1 / rts` exits; only entry Spr_ChangeFrame's bne (w08's C, 2026-10-28)
 */
void Spr_SwapFrameArt(uint32_t *d0, uint32_t *frame, uint32_t *old, uint32_t *d3, uint32_t *entry, uint32_t *a1)
{
    swap_art(d0, frame, old, d3, entry, *a1);
    *a1 = dk::usp();                                  /* move.l usp,a1 */
}
