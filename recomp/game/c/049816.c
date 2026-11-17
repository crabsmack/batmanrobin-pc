#include "game.h"

/* the low word of a data register, the upper word kept (a .w operation) */
static void set_low_word(uint32_t *r, uint16_t v) { *r = (*r & 0xFFFF0000u) | v; }

/* Title_PaletteFadeChannel - one channel of the title fade-in: after its start delay, every
 * period + 1 frames it raises its brightness by 2 (-$E up to 0), flags the palette line for
 * reload and copies its colour range from the title palette into g_PalSource at that brightness
 * (Pal_CopyBright), so the logo's parts light up one after another.
 * In:  ch = the channel; self = the fade object (its palette); the registers Pal_CopyBright
 *      passes through
 * Out: the channel stepped; d0's low word = the level read (raised by 2 when it steps); a step:
 *      the line's reload flag set, Pal_CopyBright's results (a0/a1 past the range, a4 past the
 *      record)
 * Evidence: tst.w (a5) / subq.w #1,(a5) / subq.w #1,$2(a5) bcc / move.w $4(a5),d0 bpl / addq.w #2 /
 *           st.b (a0) of the record's flag word / jmp Pal_CopyBright; Tbl_04984C, e.g. delay $2D,
 *           6 colours to $E114 (line 2) with flag $F4D2 = g_Pal2Reload; idle f1172-f33094
 */
void Title_PaletteFadeChannel(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d6, uint32_t *a0, uint32_t *a1,
                              uint32_t *a2, uint32_t *a3, uint32_t *a4, TitleFadeChannel *ch,
                              const TitleFadeObj *self)
{
    if (ch->delay != 0) {
        ch->delay = (uint16_t)(ch->delay - 1);
        return;
    }
    uint16_t count = ch->period_count;
    ch->period_count = (uint16_t)(count - 1);
    if (count != 0)   /* subq.w #1 / bcc: steps only when the count wraps below 0 */
        return;
    int16_t level = ch->level;
    set_low_word(d0, (uint16_t)level);
    if (level >= 0)   /* fully lit: done */
        return;
    level = (int16_t)(level + 2);
    set_low_word(d0, (uint16_t)level);
    ch->level = level;

    const TitleFadeRange *r = ch->range;
    ch->period_count = r->period;
    *dk::handle<u8>(r->reload_flag) = 0xFF;
    uint16_t count_minus1 = r->count_minus1;
    *a0 = self->palette.raw() + (uint32_t)(int32_t)(int16_t)r->buffer_offset;
    *a1 = (uint32_t)(int32_t)(int16_t)r->dest;          /* movea.w: a RAM address $FFFFxxxx */
    *a4 = dk::addr(r + 1);                               /* (a4)+ past the five words */
    Pal_CopyBright(d0, d1, d2, d6, count_minus1, a0, a1, a2, a3, a4);
}
