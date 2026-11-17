#include "game.h"

/* The fade's clamp tables: one word per (channel value + brightness), the channel's bits of a CRAM
   colour clamped to 0..$E: Tbl + level + component, level -$E = black, +$E = white. They overlap
   (one table from $A670 to $A7EF, each channel's window at its own base). */
static constexpr uint32_t Tbl_00A6A8 = 0x00A6A8u;   /* red: bits 1-3 */
static constexpr uint32_t Tbl_00A728 = 0x00A728u;   /* green: bits 5-7 */
static constexpr uint32_t Tbl_00A7A8 = 0x00A7A8u;   /* blue: bits 9-11 */

enum { FADE_STEP = 2, FADE_COLOURS_PER_LINE = 16, FADE_LINES = 4 };

/* a channel's clamp table, shifted by its brightness (lea Tbl,aN / ext.w / adda.w) */
static const u16 *clamp_table(uint32_t base, int8_t level)
{
    PTR(const u16) at;
    at.v = base + (uint32_t)(int32_t)level;
    return at;
}

/* Fade_Step - the per-V-int palette fade: moves g_FadeLevel 2 toward g_FadeTarget every
 * g_FadeDelay + 1 frames, then rebuilds each palette line of g_PalFaded whose brightness (the
 * level plus the line's r/g/b tint) changed or whose source was reloaded.
 * In:  g_FadeTarget, g_FadeLevel, g_FadeDelay / g_FadeDelayCount, g_PalSource, the tint records
 *      (g_PalLineTint) and g_PalLineCacheRec
 * Out: the rebuilt lines of g_PalFaded; per line the cache's r/g/b and its changed flag ($FF
 *      rebuilt, 0 not); the reload flags of rebuilt lines cleared; g_PalFaded's word 0 (the
 *      backdrop) cleared
 * Evidence: moveq #2,d1 / cmp.b g_FadeLevel,d0 / beq / bpl / neg.b d1 / subq.b #1,count / bpl /
 *           add.b d1,g_FadeLevel; per line add.b (a2)+ x3 / tst.b (a2)+ / cmp.b (a3) x3; the
 *           rebuild masks two colours at once with $E000E and ORs the three tables' words;
 *           clr.w g_PalFaded; called last in VInt_Handler (V-int code: D2 checks it)
 */
void Fade_Step(void)
{
    uint8_t target = g_FadeTarget;
    uint8_t level = g_FadeLevel;
    if (target != level) {
        /* bpl: the sign of the byte difference, not a signed compare */
        int8_t step = ((uint8_t)(target - level) & 0x80) ? -FADE_STEP : FADE_STEP;
        g_FadeDelayCount = (uint8_t)(g_FadeDelayCount - 1);
        if ((int8_t)g_FadeDelayCount < 0) {
            g_FadeDelayCount = g_FadeDelay;
            g_FadeLevel = (uint8_t)(g_FadeLevel + step);
        }
    }

    const u16 *src = (const u16 *)g_PalSource;
    u16 *out = (u16 *)g_PalFaded;
    for (int line = 0; line < FADE_LINES; line++) {
        PalLineTint *tint = &g_PalLineTint[line];
        PalLineCacheRec *cache = &g_PalLineCacheRec[line];
        int8_t r = (int8_t)(g_FadeLevel + tint->r);
        int8_t g = (int8_t)(g_FadeLevel + tint->g);
        int8_t b = (int8_t)(g_FadeLevel + tint->b);
        const u16 *line_src = src + line * FADE_COLOURS_PER_LINE;
        u16 *line_out = out + line * FADE_COLOURS_PER_LINE;
        if (tint->reload) {
            tint->reload = 0;                     /* sf.b -1(a2) */
        } else if (r == cache->r && g == cache->g && b == cache->b) {
            cache->changed = 0;                   /* the same brightness: keep the line */
            continue;
        }
        cache->r = r;
        cache->g = g;
        cache->b = b;
        cache->changed = 0xFF;
        const u16 *red = clamp_table(Tbl_00A6A8, r);
        const u16 *green = clamp_table(Tbl_00A728, g);
        const u16 *blue = clamp_table(Tbl_00A7A8, b);
        for (int i = 0; i < FADE_COLOURS_PER_LINE; i++) {
            uint16_t c = line_src[i];
            /* each channel's 3 bits (even values 0..$E) index its table as a byte offset */
            line_out[i] = (uint16_t)(red[(c & 0xE) >> 1] | green[((c >> 4) & 0xE) >> 1]
                                     | blue[((c >> 8) & 0xE) >> 1]);
        }
    }
    out[0] = 0;
}
