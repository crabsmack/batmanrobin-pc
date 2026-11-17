#include "game.h"

static constexpr int SIGN_ROWS = 7;                    /* the panel is 12 x 7 cells: 7 rows ... */
static constexpr int SIGN_ROW_LONGS = 6;               /* ... of 6 longs (2 cells each) */
static constexpr int SIGN_MAP_STRIDE_LONGS = 0xD8 / 4; /* the facade map's row: $D8 bytes */
static constexpr uint32_t SIGN_CELL_ADD = 0x23122312u; /* palette 1, tile base $312, on both cells */
static constexpr uint32_t VDP_NEXT_ROW = 0x01000000u;  /* the write command's address + $100: the plane's next row */

/* Stage1_DrawSignPanel - writes one 12x7-tile facade panel of the stage 1 street (a 'BANK OF
 * GOTHAM CITY' sign, intact or blasted) into plane B
 * In:  state = the panel (0 intact, 8 blasted; d0's low byte); vdp_ctrl/vdp_data = the VDP ports
 *      (a0/a1); slot = the slot's record (a4: Tbl_015CF4 + 16 x slot)
 * Out: the panel's 7 rows written; d0-d3/a3 clobbered
 * Evidence: andi.w #$FF,d0 / move.l (a4,d0.w),d1 / movea.l 4(a4,d0.w),a3 / adda.l $F502,a3 /
 *           move.l #$23122312,d2 / moveq #6,d3 / 7 x (move.l d1,(a0) / 6 x move.l (a3)+ + d2 to
 *           (a1) / lea $C0(a3) / addi.l #$1000000,d1); destination +$100 per row (128-wide plane B
 *           at $E000); $20866 marks a slot 8 when a blast hits a sign; 20 TAS frames f1830-f4478
 */
void Stage1_DrawSignPanel(uint8_t state, uint32_t vdp_ctrl, uint32_t vdp_data, const Stage1SignSlot *slot)
{
    const Stage1StreetVars *vars = (const Stage1StreetVars *)g_SceneVars;
    const auto &panel = slot->panel[state >> 3];   /* (a4,d0.w): the states are 0 and 8 */
    uint32_t command = panel.vdp_command;
    PTR(const u32) src;
    src.v = panel.map_offset + vars->map_buffer;
    const u32 *row = src;
    for (int y = 0; y < SIGN_ROWS; y++) {
        hw::port_write32(vdp_ctrl, command);
        for (int x = 0; x < SIGN_ROW_LONGS; x++)
            hw::port_write32(vdp_data, row[x] + SIGN_CELL_ADD);
        row += SIGN_MAP_STRIDE_LONGS;
        command += VDP_NEXT_ROW;
    }
}
