#include "game.h"

static constexpr uint16_t OPCODE_JMP_ABS_L = 0x4EF9;        /* jmp (xxx).l: the RAM H-int stub */
static constexpr uint16_t VDP_REG_HINT_LINE = 0x8A00;       /* reg $A: the H-int line (low byte) */
static constexpr uint16_t VDP_PLANEB_AT_E000 = 0x8407;      /* reg 4: plane B's own name table */
static constexpr uint32_t VDP_COLUMN_VSCROLL_HINT = 0x8B078014u;   /* reg $B = 7 (column V-scroll,
                                                                       line H-scroll), reg 0 = $14 */
enum { ARENA_COLUMNS = 20 };

/* the code at $0245B0, shared by both entries (Stage2_ArenaVIntUpload falls into it) */
static void arena_columns_and_split(uint32_t *d0, uint32_t vdp_ctrl, uint32_t a2)
{
    Scroll_DMAColumns(vdp_ctrl, a2);
    *(u16 *)g_HIntCode = OPCODE_JMP_ABS_L;
    *(u32 *)&g_HIntOperand = at_Stage2_ArenaHIntSplit;
    uint8_t line = (uint8_t)g_Stage2Vars2->split_line;   /* the word's low byte (+$21) */
    *d0 = (*d0 & 0xFFFF0000u) | VDP_REG_HINT_LINE | line;
    if (line != 0)
        hw::port_write16(vdp_ctrl, VDP_PLANEB_AT_E000);
    hw::port_write16(vdp_ctrl, (uint16_t)*d0);
    hw::port_write32(vdp_ctrl, VDP_COLUMN_VSCROLL_HINT);
}

/* Stage2_ArenaVIntColumns - the tail of the arena V-int upload, also entered by
 * Stage2_ArenaVIntScroll while the main loop is mid-frame (the scroll tables untouched): DMAs the
 * column V-scroll and arms the split H-int
 * In:  vdp_ctrl = the VDP control port (a0), a2 = the DMA command scratch; g_Stage2Vars2->split_line
 * Out: g_HIntCode = jmp Stage2_ArenaHIntSplit; VDP reg 4 = 7 when a split line is set, reg $A =
 *      the line, reg $B = 7, reg 0 = $14 (H-int on); d0.w = $8A00 | the line
 * Evidence: jsr Scroll_DMAColumns / move.w #$4EF9,g_HIntCode / move.l #$245E0,g_HIntOperand /
 *           move.w #$8A00,d0 / move.b g_SceneVars2+$21,d0 / beq / move.w #$8407,(a0) / move.w d0,(a0)
 *           / move.l #$8B078014,(a0); bne $245B0 in Stage2_ArenaVIntScroll (g_FrameBusy); TAS
 *           f32066-f35416. Named for its C (the generated code entered it)
 */
void Stage2_ArenaVIntColumns(uint32_t *d0, uint32_t vdp_ctrl, uint32_t a2)
{
    arena_columns_and_split(d0, vdp_ctrl, a2);
}

/* Stage2_ArenaVIntUpload - arena V-int upload: interleaves plane A's column V-scrolls with plane
 * B's into the column table, keeps plane B's lower columns for the split H-int, DMAs the line and
 * column scroll and arms Stage2_ArenaHIntSplit at the split line
 * In:  vdp_ctrl = the VDP control port (a0), a2 = the DMA command scratch; a1/a3 only saved
 * Out: g_SceneWorkBuf = col_a/col_b word pairs (20), col_b_split = col_b_lower; d7.w = $FFFF;
 *      a4 past col_b_split; then as Stage2_ArenaVIntColumns
 * Evidence: movem.l a0-a3,-(a7) / lea g_SceneVars+$A,a0 / +$32,a1 / +$5A,a2 / g_SceneWorkBuf,a3 /
 *           +$82,a4 / move.w #$13,d7 / 3 x move.w (an)+,(am)+ / dbra / movem.l (a7)+,a0-a3 / jsr
 *           Scroll_DMALines, then the Stage2_ArenaVIntColumns tail; reached from
 *           Stage2_ArenaVIntScroll; TAS f32066-f35416 every frame. The prototype gains d0/d7/a4 as
 *           outputs (measured clobbers) and a1/a3 inputs (only pushed: the movem image)
 */
void Stage2_ArenaVIntUpload(uint32_t *d0, uint32_t *d7, uint32_t vdp_ctrl, uint32_t a1, uint32_t a2, uint32_t a3,
                            uint32_t *a4)
{
    /* movem.l a0-a3,-(a7): a0 lowest */
    dk::push_long(a3);
    dk::push_long(a2);
    dk::push_long(a1);
    dk::push_long(vdp_ctrl);
    Stage2Vars *v = g_Stage2Vars;
    s16 *out = (s16 *)g_SceneWorkBuf;
    for (int i = 0; i < ARENA_COLUMNS; i++) {
        *out++ = v->col_a[i];
        *out++ = v->col_b[i];
        v->col_b_split[i] = v->col_b_lower[i];
    }
    *d7 = (*d7 & 0xFFFF0000u) | 0xFFFFu;   /* dbra ends at -1 */
    *a4 = dk::addr(&v->col_b_split[ARENA_COLUMNS]);
    for (int i = 0; i < 4; i++)
        (void)dk::pop_long();
    Scroll_DMALines(vdp_ctrl, a2);
    arena_columns_and_split(d0, vdp_ctrl, a2);   /* falls into $0245B0 */
}
