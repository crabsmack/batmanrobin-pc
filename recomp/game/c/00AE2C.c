#include "game.h"

/* VDP_LoadRegs - writes VDP registers 0-18 from a table of 19 bytes.
 * In:  regs (a0) = the 19 register values, register 0 first (ROM: Tbl_0155BC, or the scene
 *      header's table)
 * Out: d0.w = $93xx (the last command: register 19's number, the last byte), d7 = $0000FFFF
 *      (moveq #$12, then dbra to -1); a0 past the table (clobbered)
 * Evidence: moveq #$12,d7 / move.w #$8000,d0 / move.b (a0)+,d0 / move.w d0,$C00004 / addi.w
 *           #$100,d0 / dbra; callers Main_Start (Tbl_0155BC) and Scene_InitCommon; TAS 26 frames
 */
void VDP_LoadRegs(uint16_t *d0, uint32_t *d7, uint32_t regs)
{
    PTR(const u8) at;
    at.v = regs;
    const u8 *value = at;
    uint16_t cmd = 0x8000;                        /* $8000 + register number * $100 + value */
    for (int reg = 0; reg <= 0x12; reg++) {
        cmd = (uint16_t)((cmd & 0xFF00) | value[reg]);
        hw::port_write16(hw::VDP_CTRL, cmd);
        cmd = (uint16_t)(cmd + 0x100);
    }
    *d0 = cmd;
    *d7 = 0x0000FFFFu;
}
