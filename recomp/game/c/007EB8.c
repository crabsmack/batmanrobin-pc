#include "game.h"

/* Spr_InitBankFrames ($007EB8-$007EF5). */

static uint32_t sext16(uint16_t w) { return (uint32_t)(int32_t)(int16_t)w; }

template <class T> static T *at(uint32_t a)
{
    PTR(T) p;
    p.v = a;
    return p;
}

/* the RAM frame table: negative frame numbers are byte offsets back from g_ArtRelRingWr ($A764) */
static constexpr uint32_t SPR_RAM_FRAMES = 0xFFFFA764u;
static constexpr uint16_t ART_DUMMY_HANDLE = 0x8000;   /* g_ArtHandles' first word: owned by no frame */

/* Spr_InitBankFrames - builds the RAM frame-table entries of a loaded sprite bank: per frame the
 * art handle's address and the piece list and art, made absolute; a handle that still holds art
 * gets the dummy owner $8000 (so a later eviction clears that, not the handle) and is released.
 * In:  first = the first entry's offset from $A764 (d0.w); handle_base = the handles' base
 *      (d1.w, $8002); bank = word count, then per frame: word handle offset, long piece list and
 *      long art, each relative to its own address (a0)
 * Out: count 10-byte entries from $A764 + first; each handle cleared; d0 = the last handle's word
 *      or ArtCache_Release's result; d7 = $FFFF; a1 = past the entries; a2 = the last art pointer
 * Evidence: lea (g_ArtRelRingWr).w,a1 / adda.w d0,a1 / move.w (a0)+,d7, movea.w (a0)+,a2 /
 *           adda.w d1,a2 / move.w (a2),d0 / beq, move.w #$8000,$4(a0) / bsr.w ArtCache_Release
 *           (d1/a0 saved around it), clr.w (a2), the two movea.l a0,a2 / adda.l (a0)+,a2; only
 *           caller Op6_InitBankFrames ($70E6, handle base $8002)
 */
void Spr_InitBankFrames(uint32_t *d0, uint16_t handle_base, uint16_t *count, uint32_t bank, uint32_t *a1,
                        uint32_t *a2)
{
    uint32_t out = SPR_RAM_FRAMES + sext16((uint16_t)*d0);
    const u8 *src = at<const u8>(bank);
    *count = *(const u16 *)src;
    src += 2;
    for (uint16_t n = *count; n-- != 0;) {
        *a2 = sext16(*(const u16 *)src) + sext16(handle_base);
        src += 2;
        u16 *handle = at<u16>(*a2);
        *d0 = (*d0 & 0xFFFF0000u) | (uint16_t)*handle;
        if ((uint16_t)*d0 != 0) {
            dk::handle<ArtDesc>((int16_t)(uint16_t)*d0)->owner = ART_DUMMY_HANDLE;
            uint16_t d1 = handle_base;   /* movem.l d1/a0: restored after the call */
            ArtCache_Release(d0, &d1, *a2);
        }
        *handle = 0;
        SprFrame *e = at<SprFrame>(out);
        e->art_handle = (uint16_t)*a2;
        e->pieces = ADDR68(src) + (uint32_t)*(const u32 *)src;   /* relative to the long itself */
        src += 4;
        *a2 = ADDR68(src) + (uint32_t)*(const u32 *)src;
        e->art = *a2;
        src += 4;
        out += sizeof(SprFrame);
    }
    *count = 0xFFFF;
    *a1 = out;
}
