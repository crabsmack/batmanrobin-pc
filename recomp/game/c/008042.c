#include "game.h"

/* Spr_QueueArtDMA ($008042-$0080CF); its two ROM tables follow it. */

static uint32_t sext16(uint16_t w) { return (uint32_t)(int32_t)(int16_t)w; }
static void set_lo(uint32_t *r, uint16_t v) { *r = (*r & 0xFFFF0000u) | v; }
static void set_lo_byte(uint32_t *r, uint8_t v) { *r = (*r & ~0xFFu) | v; }

template <class T> static T *at(uint32_t a)
{
    PTR(T) p;
    p.v = a;
    return p;
}

/* lea (Tbl_0080D4-$4).l,a1: the DMA length registers ($93xx94xx) by chunk code (4 x tiles) */
static constexpr uint32_t DMA_LENGTH_BY_CODE = 0x0080D0;
/* lea (loc_0080B4+$1).l,a2: the VRAM write commands by block tile (an odd base: tile numbers are
   odd, $5F + $10n, so each long is word-aligned; the table starts inside this routine's code) */
static constexpr uint32_t VRAM_CMD_BY_TILE = 0x0080B5;

/* one queued DMA (14 bytes, sent by DMA_FlushQueue) */
static u16 *queue_long(u16 *q, uint32_t v)
{
    *q++ = (uint16_t)(v >> 16);
    *q++ = (uint16_t)v;
    return q;
}

/* Spr_QueueArtDMA - queues one DMA per art chunk from ROM into the frame's VRAM blocks and charges
 * the DMA budget (cost + chunks) and the queue (chunks).
 * In:  art = the art header (a4: word cost, word chunks n, n chunk codes = 4 x tiles, then the
 *      tiles); blocks = the destination block tiles (a3, from ArtCache_Alloc)
 * Out: n entries at g_DMAQueuePtr (lengths, source registers $95/$96/$97, VRAM command), the
 *      pointer advanced; g_DMAQueueFree -= n, g_DMABudgetLeft -= cost + n. The registers as
 *      left: d1.w = the last block tile, d3 = the source registers' long ($00HH95LL for several
 *      chunks, $97HH95LL for one), d2 = 0 / d7 = $0000FFFF (several) or d7 = 0 (one, d2 kept),
 *      a1 = the length table, a3 past the block tiles read (one chunk: not advanced), a4 past
 *      the chunk codes
 * Evidence: move.w (a4)+,d0 / moveq #0,d7 / move.w (a4)+,d7 / sub.w d7,(g_DMAQueueFree).w /
 *           add.w d7,d0 / sub.w d0,(g_DMABudgetLeft).w, move.l a4,d3 / lsr.l #1,d3 /
 *           add.l d7,d3 (the tiles' word address), the add.b d0,d0 / add.b d0,d3 / addx.b chain
 *           (the source + 16 words a tile of the chunk before: the byte doubling drops chunks of
 *           32+ tiles' carry, kept), the single-chunk path's move.l d3,(a0)+ ($97 before $95);
 *           callers Spr_SwapFrameArt and $6776, right after ArtCache_Alloc
 */
void Spr_QueueArtDMA(uint16_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d7, uint32_t *a1, uint32_t *blocks,
                     uint32_t *art)
{
    const u16 *hdr = at<const u16>(*art);
    uint16_t cost = *hdr++;
    *d7 = *hdr++;
    uint16_t chunks = (uint16_t)*d7;
    *art = ADDR68(hdr);
    g_DMAQueueFree = (uint16_t)(g_DMAQueueFree - chunks);
    g_DMABudgetLeft = (uint16_t)(g_DMABudgetLeft - (uint16_t)(cost + chunks));
    u16 *q = at<u16>(sext16(g_DMAQueuePtr));
    *a1 = DMA_LENGTH_BY_CODE;

    uint32_t src = (*art >> 1) + *d7;                 /* the tiles' address in words */
    uint32_t r96 = (src & 0xFFFF0000u) | (uint16_t)((uint16_t)src >> 8);   /* d4: bits 15-8 */
    src &= 0x00FF00FFu;                               /* d3: bits 23-16 and 7-0 */
    if (chunks == 0)
        dk::fatal("Spr_QueueArtDMA: 0 chunks (the dbra would queue 65,536 DMAs)");
    const u16 *code = at<const u16>(*art);           /* the chunk codes */
    if (chunks == 1) {
        *d7 = 0;
        *d3 = src | 0x97009500u;
        set_lo(&r96, (uint16_t)(r96 | 0x9600));
        uint16_t c = *code++;
        q = queue_long(q, *at<const u32>(DMA_LENGTH_BY_CODE + sext16(c)));
        q = queue_long(q, *d3);
        *q++ = (uint16_t)r96;
        *d1 = *at<const u16>(*blocks);
        q = queue_long(q, *at<const u32>(VRAM_CMD_BY_TILE + sext16(*d1)));
    } else {
        uint32_t r97 = (src << 16) | (src >> 16);     /* d5: swap */
        set_lo(&src, (uint16_t)(src | 0x9500));
        set_lo(&r96, (uint16_t)(r96 | 0x9600));
        set_lo(&r97, (uint16_t)(r97 | 0x9700));
        *d2 = 0;
        const u16 *tile = at<const u16>(*blocks);
        uint16_t c = 0;
        for (uint16_t n = 0; n < chunks; n++) {
            if (n != 0) {
                /* the source moves past the previous chunk: 2 x (code doubled) words, as a 24-bit
                   add over d5:d4:d3's low bytes */
                uint8_t step = (uint8_t)(c + c);
                for (int twice = 0; twice < 2; twice++) {
                    unsigned lo8 = (uint8_t)src + step;
                    set_lo_byte(&src, (uint8_t)lo8);
                    unsigned mid = (uint8_t)r96 + (lo8 >> 8);
                    set_lo_byte(&r96, (uint8_t)mid);
                    set_lo_byte(&r97, (uint8_t)((uint8_t)r97 + (mid >> 8)));
                }
            }
            c = *code++;
            q = queue_long(q, *at<const u32>(DMA_LENGTH_BY_CODE + sext16(c)));
            *q++ = (uint16_t)src;
            *q++ = (uint16_t)r96;
            *q++ = (uint16_t)r97;
            *d1 = *tile++;
            q = queue_long(q, *at<const u32>(VRAM_CMD_BY_TILE + sext16(*d1)));
        }
        *d7 = 0x0000FFFFu;
        *d3 = src;
        *blocks = ADDR68(tile);
    }
    *art = ADDR68(code);
    g_DMAQueuePtr = (uint16_t)ADDR68(q);
}
