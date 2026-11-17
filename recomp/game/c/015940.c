#include "game.h"

/* Debug_DrawHexBytes: the digit sprite frames are 10 apart from frame $A ('0') on */
enum {
    HEX_FRAME_STEP = 0x0A,
    HEX_FRAME_FIRST = 0x0A,
    HEX_CELL = 8,              /* one digit per 8-px cell */
    HEX_ORIGIN = 0x20,         /* the cell grid starts at sprite coordinate $20 */
};
static constexpr uint16_t HEX_ATTR = 0x8000;   /* d6 of Spr_DrawDirect: high priority */

/* Stage1_VIntFireStep: the fire's VRAM and the blank it starts from */
static constexpr uint32_t FIRE_CLEAR_COMMAND = 0x40000003u;   /* VRAM write $C000: plane A */
static constexpr uint32_t FIRE_CLEAR_TILES = 0x05FC05FCu;      /* two blank cells (tile $5FC) */
static constexpr int FIRE_CLEAR_LONGS = 0x30 * 8;              /* 384 longs: plane A's top 6 rows */
static constexpr uint32_t FIRE_DMA_LENGTH = 0x93909401u;       /* VDP regs 19/20: $190 words */
static constexpr uint32_t VDP_DMA_SRC_MID_LOW = 0x97009500u;   /* regs 23 (top bits) and 21 (low byte) */
static constexpr uint16_t VDP_DMA_SRC_MID = 0x9600;            /* reg 22: the middle byte */
static constexpr int FIRE_ROW_LONGS = 7;                       /* a fire row: 14 cells */
static constexpr uint32_t VDP_NEXT_ROW = 0x01000000u;          /* the write command's address + $100 */
static constexpr int FIRE_BUF = 3;                             /* g_BufPtrs[3] ($E09E): the fire's art and maps */
static constexpr uint32_t Tbl_015A7E = 0x015A7Eu;              /* the fire's frame list (20-byte entries) */

/* an entry of the fire's frame list Tbl_015A7E */
typedef struct FireFrame {
    u16 art_offset;            /* +$00: the tiles' offset in the art (after its header long) */
    u32 dma_command;           /* +$02: the VDP command that starts the tile DMA */
    u16 rows_minus_1;          /* +$06 */
    u16 attr;                  /* +$08: added to both cells of every map long */
    u16 map_offset;            /* +$0A: the frame's map in the map data (after its header long) */
    u32 map_command;           /* +$0C: VDP write command of the frame's first plane A row */
    u16 x_offset;              /* +$10: Stage1StreetVars.fire_frame_x */
    u16 loop;                  /* +$12: nonzero: the next frame is the list's first again */
} FireFrame;
static_assert(sizeof(FireFrame) == 20, "FireFrame");
static_assert(offsetof(FireFrame, map_command) == 0x0C, "FireFrame.map_command");

/* Debug_DrawHexBytes - draws d1 bytes from a6 as hex digit sprites straight into the SAT at a
 * character-cell position
 * In:  d0 = position (high byte column, low byte row, 8 px each, +$20); d1 = byte count (0 = 65536);
 *      a6 = the bytes
 * Out: d4 = x past the last digit, d5 = y, d6 = $8000; d0/d2/d3/d7/a0-a2 as Spr_ChangeFrame /
 *      Spr_DrawDirect left them, d1 = the last Spr_DrawDirect's high word over 0, a6 = past the
 *      bytes; g_DebugHexFrame = the last digit's frame
 * Evidence: d5 = (d0 & $FF) << 3 + $20 / d4 = (d0 & $FF00) >> 5 + $20; per nibble (high first):
 *           mulu.w #$A / addi.w #$A / move.w g_DebugHexFrame,d2 / jsr Spr_ChangeFrame / move.w d1,
 *           g_DebugHexFrame / move.w #$8000,d6 / jsr Spr_DrawDirect / addq.w #8,d4; move.w (a7)+,d1
 *           / subq.w #1,d1 / bne; caller Debug_FreeCamStep (9 readouts); static only (the debug free
 *           camera). d0, d1, a6 outputs (measured: clobbers): Debug_FreeCamStep's own outputs are
 *           what its last readout leaves in them
 */
void Debug_DrawHexBytes(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint16_t *d4, uint16_t *d5,
                        uint16_t *d6, uint16_t *d7, uint32_t *a0, uint32_t *a1, uint32_t *a2, uint32_t *a6)
{
    *d5 = (uint16_t)(((*d0 & 0xFF) << 3) + HEX_ORIGIN);
    *d4 = (uint16_t)(((*d0 & 0xFF00) >> 5) + HEX_ORIGIN);
    PTR(const u8) bytes;
    bytes.v = *a6;
    const u8 *p = bytes;
    uint16_t n = (uint16_t)*d1;     /* the count, kept on the stack around the digits */
    do {
        uint8_t b = *p++;
        for (int nibble = 0; nibble < 2; nibble++) {
            *d1 = (uint32_t)((nibble == 0 ? b >> 4 : b & 0xF) * HEX_FRAME_STEP + HEX_FRAME_FIRST);   /* mulu.w */
            *d2 = (*d2 & 0xFFFF0000u) | g_DebugHexFrame;
            Spr_ChangeFrame(d0, d1, d2, d3, a0, a1);
            g_DebugHexFrame = (uint16_t)*d1;
            *d6 = HEX_ATTR;
            uint16_t handle = (uint16_t)*d2;
            Spr_DrawDirect(d0, d1, &handle, d3, *d4, *d5, d6, d7, a0, a1, a2);
            *d2 = (*d2 & 0xFFFF0000u) | handle;
            *d4 = (uint16_t)(*d4 + HEX_CELL);
        }
        *d1 = (*d1 & 0xFFFF0000u) | --n;
    } while (n != 0);
    *a6 = ADDR68(p);
}

/* Stage1_VIntFireStep - V-int step of the stage 1 street's building fire on plane A: state 2 blanks
 * the top 6 rows of plane A and moves to 1; state 1 DMAs the next fire frame's tiles and writes its
 * map rows, cycling through the frame list Tbl_015A7E
 * In:  state = g_Stage1FireState (1 or 2); vdp_ctrl/vdp_data = the VDP ports (a0/a1); dma_cmd = a
 *      RAM long the DMA's last command word pair is written from (a2: the V-int's scratch)
 * Out: state 2: g_Stage1FireState = 1, VRAM $C000 filled with tile $5FC (384 longs); state 1: the
 *      frame drawn, the street's fire_frame_x = its x offset, fire_frame = the next entry (the list's
 *      first after one with the loop flag)
 * Evidence: subq.b #1,d0 / beq; move.b #1,g_Stage1FireState / move.l #$40000003,(a0) / 48 x 8
 *           move.l #$5FC05FC,(a1); state 1: movea.l $F4F4,a3 / move.l #$93909401,(a0) / movea.l
 *           $E09E,a4 / adda.l (a4),a4 / adda.w (a3)+,a4 / the source >> 1 into regs $95-$97 / move.l
 *           (a3)+,(a2) / move.w (a2),(a0) / move.w 2(a2),(a0) / the rows: move.l d1,(a0) / 7 x
 *           (a4)+ + attr to (a1) / addi.l #$1000000,d1 / dbra; called by Stage1_VIntFacade while
 *           the state > 0; the fire burns over the bank from TAS f2160
 */
void Stage1_VIntFireStep(uint8_t state, uint32_t vdp_ctrl, uint32_t vdp_data, uint32_t dma_cmd)
{
    if ((uint8_t)(state - 1) != 0) {
        g_Stage1FireState = 1;
        hw::port_write32(vdp_ctrl, FIRE_CLEAR_COMMAND);
        for (int i = 0; i < FIRE_CLEAR_LONGS; i++)
            hw::port_write32(vdp_data, FIRE_CLEAR_TILES);
        return;
    }
    Stage1StreetVars *vars = (Stage1StreetVars *)g_SceneVars;
    PTR(const FireFrame) entry;
    entry.v = vars->fire_frame;
    const FireFrame *f = entry;
    hw::port_write32(vdp_ctrl, FIRE_DMA_LENGTH);
    /* the art and the maps: g_BufPtrs[3] holds two offsets to them (from its own start and from +4) */
    const PTR(const u32) *bufs = (const PTR(const u32) *)g_BufPtrs;
    uint32_t base = bufs[FIRE_BUF].raw();
    PTR(const u32) header;
    header.v = base;
    uint32_t art = base + *header.get() + (uint32_t)(int32_t)(int16_t)f->art_offset;
    uint32_t src = art >> 1;
    uint16_t mid = (uint16_t)((uint16_t)src >> 8 | VDP_DMA_SRC_MID);
    hw::port_write32(vdp_ctrl, (src & 0x7F00FFu) | VDP_DMA_SRC_MID_LOW);
    hw::port_write16(vdp_ctrl, mid);
    PTR(u16) cmd;                       /* the DMA's start command goes out from RAM (move.w (a2)) */
    cmd.v = dma_cmd;
    u16 *words = cmd;
    uint32_t start = f->dma_command;
    words[0] = (uint16_t)(start >> 16);
    words[1] = (uint16_t)start;
    hw::port_write16(vdp_ctrl, words[0]);
    hw::port_write16(vdp_ctrl, words[1]);
    uint16_t rows = f->rows_minus_1;
    uint32_t attr = ((uint32_t)(uint16_t)f->attr << 16) | (uint16_t)f->attr;
    header.v = base + 4;
    PTR(const u32) map;
    map.v = base + 4 + *header.get() + (uint32_t)(int32_t)(int16_t)f->map_offset;
    const u32 *cells = map;
    uint32_t command = f->map_command;
    do {
        hw::port_write32(vdp_ctrl, command);
        for (int i = 0; i < FIRE_ROW_LONGS; i++)
            hw::port_write32(vdp_data, *cells++ + attr);
        command += VDP_NEXT_ROW;
    } while (rows-- != 0);
    vars->fire_frame_x = f->x_offset;
    vars->fire_frame = f->loop ? Tbl_015A7E : ADDR68(f + 1);
}
