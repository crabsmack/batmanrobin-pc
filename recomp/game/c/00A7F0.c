#include "game.h"

/* Tbl_00A87E: 15 long pointers, one per nonzero set of changed lines (bits 5-2 = lines 0-3, so
   the set times 1 is the byte offset past the table's -4), each to one or two CRAM DMA
   descriptors: word command high word, then two longs of DMA register writes ($93/$94, $95/$96,
   or with $97), then the next descriptor's command word or 0. */
static constexpr uint32_t Tbl_00A87E = 0x00A87Eu;

enum {
    PALUP_ALL_LINES = 0x3C,            /* bits 5-2: every line (the flash's end) */
    PALUP_SHADOW_ON = 0x8C89,          /* reg 12: H40, shadow/highlight on */
    PALUP_SHADOW_OFF = 0x8C81,         /* reg 12: H40, shadow/highlight off (the white flash) */
    PALUP_DMA_CMD_LOW = 0x0080,        /* the CRAM write command's low word: DMA */
};
static constexpr uint32_t PALUP_CRAM_WRITE = 0xC0000000u;
static constexpr uint32_t PALUP_WHITE_PAIR = 0x0EEE0EEEu;

/* a long of two descriptor words, high first (move.l (a3)+,(a0)) */
static uint32_t desc_long(const u16 *d)
{
    return (uint32_t)(uint16_t)d[0] << 16 | (uint16_t)d[1];
}

/* Pal_VIntUpload - the V-int's palette upload: runs the white flash (CRAM filled with $0EEE once,
 * the faded palette restored when g_FlashTimer runs out), or else DMAs to CRAM the lines of
 * g_PalFaded that Fade_Step rebuilt.
 * In:  ctrl (a0) / data (a1) = the VDP ports, scratch (a2) = the RAM word pair the DMA command is
 *      sent from ($FF9910); g_FlashTimer, g_FlashReq, g_SceneShadowHL, g_PalNChanged
 * Out: CRAM; g_FlashReq cleared when the flash starts, g_FlashTimer counted down; reg 12 = $81
 *      during the flash, $89 again after it when g_SceneShadowHL; the scratch's words
 * Evidence: tst.b g_FlashTimer / subq.b / tst.b g_SceneShadowHL / move.w #$8C89,(a0) / move.w
 *           #$3C,d0; the flash: move.l #$C0000000,(a0) / 32x move.l #$EEE0EEE,(a1) / move.w
 *           #$8C81,(a0); the lines: g_PalNChanged & $20/$10/$8/$4, movea.l $A87A(pc,d0.w),a3 /
 *           move.w #$80,2(a2) / move.w (a3)+,(a2) / 2x move.l (a3)+,(a0) / move.w (a2),(a0) /
 *           move.w 2(a2),(a0) / move.w (a3)+,(a2) / beq; bsr from VInt_Handler every full V-int
 *           (V-int code: D2 checks it)
 */
void Pal_VIntUpload(uint32_t ctrl, uint32_t data, uint32_t scratch)
{
    unsigned lines;
    if (g_FlashTimer != 0) {
        g_FlashTimer = (uint8_t)(g_FlashTimer - 1);
        if (g_FlashTimer != 0) {
            if (g_FlashReq == 0)
                return;
            g_FlashReq = 0;                       /* the flash's first frame: CRAM white */
            hw::port_write32(ctrl, PALUP_CRAM_WRITE);
            for (int i = 0; i < 32; i++)
                hw::port_write32(data, PALUP_WHITE_PAIR);
            hw::port_write16(ctrl, PALUP_SHADOW_OFF);
            return;
        }
        if (g_SceneShadowHL)                      /* the flash is over: restore everything */
            hw::port_write16(ctrl, PALUP_SHADOW_ON);
        lines = PALUP_ALL_LINES;
    } else {
        lines = (g_Pal0Changed & 0x20) | (g_Pal1Changed & 0x10) | (g_Pal2Changed & 0x08)
              | (g_Pal3Changed & 0x04);
        if (lines == 0)
            return;
    }

    PTR(const u32) tbl;
    tbl.v = Tbl_00A87E - 4;
    PTR(const u16) at;
    at.v = ((const u32 *)tbl)[lines / 4];
    const u16 *desc = at;
    PTR(u16) cmd_at;
    cmd_at.v = scratch;
    u16 *cmd = cmd_at;
    cmd[1] = PALUP_DMA_CMD_LOW;
    cmd[0] = *desc++;
    for (int n = 0; n < 2; n++) {                 /* one DMA, or two for lines not adjacent */
        hw::port_write32(ctrl, desc_long(desc));
        hw::port_write32(ctrl, desc_long(desc + 2));
        desc += 4;
        hw::port_write16(ctrl, cmd[0]);
        hw::port_write16(ctrl, cmd[1]);           /* the second word starts the DMA */
        if (n == 1)
            return;
        cmd[0] = *desc++;                         /* the second descriptor's command, or 0 */
        if (cmd[0] == 0)
            return;
    }
}
