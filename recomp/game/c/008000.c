#include "game.h"

/* Spr_ReleaseFrame ($008000-$008041). */

static uint32_t sext16(uint16_t w) { return (uint32_t)(int32_t)(int16_t)w; }

template <class T> static T *at(uint32_t a)
{
    PTR(T) p;
    p.v = a;
    return p;
}

static constexpr uint32_t SPR_RAM_FRAMES = 0xFFFFA764u;   /* frames < 0: back from g_ArtRelRingWr */

/* Spr_ReleaseFrame - drops one reference to the art of a frame; at the last one its blocks count
 * as free and its descriptor goes on the release ring (once) for later eviction. A refcount that
 * was not negative (not in use) is reset to 0.
 * In:  frame (d2.w: >= 0 Tbl_1DDB60, < 0 the RAM table below $A764)
 * Out: d0.w = the handle's descriptor word; frame = its low byte the block count when released,
 *      the ring slot when it was queued; a0 = where the original's pointer stopped (the handle
 *      address, the descriptor +1 / +2, or the ring slot)
 * Evidence: lea (Tbl_1DDB60).l,a0 / tst.w d2 / bpl / lea (g_ArtRelRingWr).w,a0,
 *           movea.w (a0,d2.w),a0 / move.w (a0),d0 / beq, addq.b #1,(a0)+ / bne / bmi /
 *           clr.b -$1(a0), the release-ring push; callers Obj_ReleaseFrames, Op4_Kill's tail,
 *           $109A0 (each clears the frame word after it)
 */
void Spr_ReleaseFrame(uint16_t *d0, uint16_t *frame, uint32_t *a0)
{
    uint32_t base = (int16_t)*frame >= 0 ? SPR_ROM_FRAMES : SPR_RAM_FRAMES;
    *a0 = sext16(at<const SprFrame>(base + sext16(*frame))->art_handle);
    *d0 = *at<u16>(*a0);
    if (*d0 == 0)
        return;
    ArtDesc *desc = dk::handle<ArtDesc>((int16_t)*d0);
    int8_t users = (int8_t)(desc->refcount + 1);
    desc->refcount = users;
    *a0 = ADDR68(&desc->blocks);
    if (users != 0) {
        if (users > 0)
            desc->refcount = 0;                       /* was not in use */
        return;
    }
    *frame = (uint16_t)((*frame & 0xFF00u) | desc->blocks);
    *a0 = ADDR68(&desc->ring_slot);
    g_ArtBlocksFree = (uint8_t)(g_ArtBlocksFree + desc->blocks);
    g_ArtDescsFree = (uint8_t)(g_ArtDescsFree + 1);
    if (desc->ring_slot != 0)
        return;                                       /* already on the release ring */
    *frame = g_ArtRelRingWr;
    g_ArtRelRingWrLo = (uint8_t)(g_ArtRelRingWrLo + 2);
    desc->ring_slot = *frame;
    *a0 = sext16(*frame);
    *at<u16>(*a0) = *d0;
}
