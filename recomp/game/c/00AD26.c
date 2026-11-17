#include "game.h"

/* The fade's clamp tables (Fade_Step's, 00A43C.c): Tbl + brightness + a channel's bits = the
   channel clamped to 0..$E, already in its CRAM position. */
static constexpr uint32_t Tbl_00A6A8 = 0x00A6A8u;   /* red */
static constexpr uint32_t Tbl_00A728 = 0x00A728u;   /* green */
static constexpr uint32_t Tbl_00A7A8 = 0x00A7A8u;   /* blue */

/* Tint_Step - moves g_TintLevel 2 toward g_TintTarget once every g_TintDelay + 1 calls and
 * returns it.
 * In:  g_TintLevel, g_TintTarget, g_TintDelay, g_TintDelayCount
 * Out: level (d0.b) = the new g_TintLevel; step (d1) = $FFFFFFFE (moveq #-2), or $FFFFFF02 when
 *      the level is below the target (neg.b)
 * Evidence: moveq #-2,d1 / move.b g_TintLevel,d0 / cmp.b g_TintTarget,d0 / beq / bpl / neg.b d1 /
 *           subq.b #1,g_TintDelayCount / bpl / move.b g_TintDelay,g_TintDelayCount / add.b d1,d0
 *           / move.b d0,g_TintLevel; callers Tint_StepLine0/1/3, Attract_RooftopLightning and
 *           $48C36; TAS 12,372 frames
 */
void Tint_Step(uint8_t *level, uint32_t *step)
{
    uint32_t d1 = 0xFFFFFFFEu;
    uint8_t now = g_TintLevel;
    if (now != g_TintTarget) {
        /* bpl: the sign of the byte difference, not a signed compare */
        if ((uint8_t)(now - g_TintTarget) & 0x80)
            d1 = (d1 & 0xFFFFFF00u) | (uint8_t)(-(int8_t)(uint8_t)d1);
        g_TintDelayCount = (uint8_t)(g_TintDelayCount - 1);
        if ((int8_t)g_TintDelayCount < 0) {
            g_TintDelayCount = g_TintDelay;
            now = (uint8_t)(now + (uint8_t)d1);
        }
    }
    g_TintLevel = now;
    *level = now;
    *step = d1;
}

/* Pal_CopyBright - copies CRAM colours with one brightness offset on all three channels, through
 * the fade's clamp tables.
 * In:  d0.w = the offset (even, -$E black .. +$E white), d7.w = colours - 1, a0 = the source
 *      colours, a1 = the destination
 * Out: a0 / a1 past the colours; a2-a4 = the tables + the offset; d6.w = $F; d0-d2 as the last
 *      colour left them (the asm's long shifts and masks, kept bit for bit); d7 clobbered
 * Evidence: lea Tbl_00A6A8/A728/A7A8(pc),a2-a4 / adda.w d0 x3 / move.w #$F,d6; per colour move.w
 *           (a0)+,d0 / move.w d0,d1 / lsr.l #4,d1 / move.l d1,d2 / lsr.l #4,d2 / and.l d6 x3 /
 *           move.w (a2,d0.w),d0 / or.w (a3,d1.w),d0 / or.w (a4,d2.w),d0 / move.w d0,(a1)+ / dbra;
 *           only caller Title_PaletteFadeChannel (jmp); the idle run's title fade-in
 */
void Pal_CopyBright(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d6, uint16_t d7, uint32_t *a0,
                    uint32_t *a1, uint32_t *a2, uint32_t *a3, uint32_t *a4)
{
    int32_t offset = (int16_t)(uint16_t)*d0;
    *a2 = Tbl_00A6A8 + (uint32_t)offset;
    *a3 = Tbl_00A728 + (uint32_t)offset;
    *a4 = Tbl_00A7A8 + (uint32_t)offset;
    PTR(const u16) red_at, green_at, blue_at, src_at;
    red_at.v = *a2;
    green_at.v = *a3;
    blue_at.v = *a4;
    const u8 *red = (const u8 *)(const u16 *)red_at;      /* indexed by byte offsets */
    const u8 *green = (const u8 *)(const u16 *)green_at;
    const u8 *blue = (const u8 *)(const u16 *)blue_at;
    src_at.v = *a0;
    const u16 *src = src_at;
    PTR(u16) dst_at;
    dst_at.v = *a1;
    u16 *dst = dst_at;
    *d6 = (*d6 & 0xFFFF0000u) | 0x000F;
    uint32_t r = *d0, g = *d1, b = *d2;
    for (uint32_t n = (uint32_t)d7 + 1; n != 0; n--) {
        uint16_t colour = *src++;
        r = (r & 0xFFFF0000u) | colour;           /* move.w (a0)+,d0 */
        g = (g & 0xFFFF0000u) | colour;           /* move.w d0,d1 */
        g >>= 4;
        b = g >> 4;
        r &= *d6;
        g &= *d6;
        b &= *d6;
        /* a word of each table at the channel's byte offset */
        uint16_t out = (uint16_t)(red[r & 0xFFFF] << 8 | red[(r & 0xFFFF) + 1]);
        out |= (uint16_t)(green[g & 0xFFFF] << 8 | green[(g & 0xFFFF) + 1]);
        out |= (uint16_t)(blue[b & 0xFFFF] << 8 | blue[(b & 0xFFFF) + 1]);
        r = (r & 0xFFFF0000u) | out;
        *dst++ = out;
    }
    *d0 = r;
    *d1 = g;
    *d2 = b;
    *a0 = ADDR68(src);
    *a1 = ADDR68(dst);
}
