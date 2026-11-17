#include "game.h"

static constexpr uint32_t VRAM_WRITE_FC00 = 0x7C000002u;   /* plane A/B H-scroll (line 0) */
static constexpr uint32_t LIFT_HSCROLL = 0xFFF80000u;      /* plane A -8, plane B 0 */
static constexpr uint32_t VSRAM_WRITE_0 = 0x40000010u;
static constexpr uint16_t OPCODE_JMP_ABS_L = 0x4EF9;       /* jmp (xxx).l: the RAM H-int stub */
static constexpr uint32_t VDP_HINT_ON_LINE = 0x80148A00u;  /* reg 0 = $14 (H-int on), reg 10 = the line */
static constexpr uint32_t VDP_HINT_OFF_AUTOINC = 0x8AFF8F04u;   /* reg 10 = $FF, reg 15 = 4 */
static constexpr uint16_t LIFT_FLOOR_HSCROLL = 0xFEA8;     /* plane A -$158 below the floor line */
static constexpr uint16_t LIFT_FLOOR_LINE = 0xAF;
static constexpr int LIFT_COLUMNS = 20;

/* Stage2_LiftVIntScroll - V-int object of the lift section: when a frame is ready, builds the
 * 20-column V-scroll of the shaft (plane A from the scene's column scrolls, plane B from layer A's
 * row) and the shake split line, then writes the H-scroll, DMAs the columns and arms the H-int stub
 * Stage2_LiftHIntFloor at line $AF - shake
 * In:  a0/a1 = the VDP control/data ports (the V-int object list); a2 for Scroll_DMAColumns
 * Out: g_SceneWorkBuf (plane A, plane B word pairs), the platform's shake scroll and split line;
 *      g_HIntCode = jmp Stage2_LiftHIntFloor; VDP reg 0 = $14, reg 10 = the split line
 * Evidence: tst.b g_FrameBusy / bne / g_ShakeY -> g_SceneVars2+$28/$2A, $AF - it -> +$26 / g_LayerA
 *           ($34 + 2) << 3 | ($2 & $F) / 20 x (move.w (a3)+,(a4)+ / move.w d0,(a4)+) / move.l
 *           #$7C000002,(a0) / move.l #$FFF80000,(a1) / jsr Scroll_DMAColumns / move.w #$4EF9,g_HIntCode
 *           / move.l #$24526,g_HIntOperand / move.l #$80148A00 with the line byte,(a0); installed by
 *           Stage2_LiftInstallVInt; TAS f21158-f32065 every frame
 */
void Stage2_LiftVIntScroll(uint32_t vdp_ctrl, uint32_t vdp_data, uint32_t a2)
{
    Stage2Vars2 *arena = g_Stage2Vars2;
    if (!g_FrameBusy) {
        uint16_t shake = (uint16_t)(g_ShakeY >> 16);
        arena->shake_vscroll[0] = shake;
        arena->shake_vscroll[1] = shake;
        arena->floor_line = (uint16_t)(LIFT_FLOOR_LINE - shake);
        const MapLayer *layer = (const MapLayer *)g_LayerA;
        uint16_t plane_b = (uint16_t)((uint16_t)(layer->row + 2) << 3);
        plane_b = (uint16_t)(plane_b | (layer->y & 0xF));   /* or.b: the low byte only */
        const Stage2Vars *vars = g_Stage2Vars;
        u16 *buf = (u16 *)g_SceneWorkBuf;
        for (int i = 0; i < LIFT_COLUMNS; i++) {
            buf[2 * i] = (uint16_t)vars->col_a[i];
            buf[2 * i + 1] = plane_b;
        }
    }
    hw::port_write32(vdp_ctrl, VRAM_WRITE_FC00);
    hw::port_write32(vdp_data, LIFT_HSCROLL);
    Scroll_DMAColumns(vdp_ctrl, a2);
    *(u16 *)g_HIntCode = OPCODE_JMP_ABS_L;
    *(u32 *)&g_HIntOperand = at_Stage2_LiftHIntFloor;
    uint8_t line = (uint8_t)arena->floor_line;              /* move.b g_SceneVars2+$27: its low byte */
    hw::port_write32(vdp_ctrl, (VDP_HINT_ON_LINE & 0xFFFFFF00u) | line);
}

/* Stage2_LiftHIntFloor - H-int at the lift floor's top line: stops further H-ints, sets plane A's
 * H-scroll to -$158 and all 20 column V-scrolls to the shake value, so the lift platform below the
 * line stays put while the shaft scrolls
 * In:  the interrupted code's d0/a0 (saved on the stack around the handler, as the original)
 * Out: VDP reg 10 = $FF, reg 15 = 4; VRAM $FC00 = $FEA8; VSRAM 0-$27 (20 words) = the shake pair; rte
 * Evidence: move.l a0,-(a7) / move.l d0,-(a7) / lea $C00000,a0 / move.l #$8AFF8F04,4(a0) / move.l
 *           #$7C000002,4(a0) / move.w #$FEA8,(a0) / move.l #$40000010,4(a0) / 10 x move.l
 *           g_SceneVars2+$28 / the pops / rte; the jmp stub Stage2_LiftVIntScroll writes at
 *           g_HIntCode; png f028800 shows the riveted floor fixed at the bottom
 */
void Stage2_LiftHIntFloor(uint32_t d0, uint32_t a0)
{
    dk::push_long(a0);
    dk::push_long(d0);
    hw::port_write32(hw::VDP_CTRL, VDP_HINT_OFF_AUTOINC);
    hw::port_write32(hw::VDP_CTRL, VRAM_WRITE_FC00);
    hw::port_write16(hw::VDP_DATA, LIFT_FLOOR_HSCROLL);
    hw::port_write32(hw::VDP_CTRL, VSRAM_WRITE_0);
    const Stage2Vars2 *arena = g_Stage2Vars2;
    uint32_t pair = ((uint32_t)arena->shake_vscroll[0] << 16) | arena->shake_vscroll[1];
    for (int i = 0; i < LIFT_COLUMNS / 2; i++)
        hw::port_write32(hw::VDP_DATA, pair);
    dk::pop_long();                   /* the saved d0 and a0: what the bridge restores anyway */
    dk::pop_long();
    dk::rte();
}

/* Stage2_ArenaVIntScroll - V-int object of the stage 2 boss arena (installed by Stage2_LiftArriveTop):
 * when a frame is ready copies the split line for the H-int, then continues into
 * Stage2_ArenaVIntUpload; a busy frame goes on at Stage2_ArenaVIntColumns (past the buffer copy)
 * In:  a0 = the VDP control port, a2 (the V-int object list)
 * Out: split_line = split_next; then as Stage2_ArenaVIntUpload (or Stage2_ArenaVIntColumns)
 * Evidence: tst.b g_FrameBusy / bne.s $245B0 / move.w g_SceneVars2+$1E,g_SceneVars2+$20, falling into
 *           Stage2_ArenaVIntUpload; move.l #$2456A into the V-int object by Stage2_LiftArriveTop; TAS
 *           f32066-f35416
 */
void Stage2_ArenaVIntScroll(uint32_t a0, uint32_t a2)
{
    if (g_FrameBusy) {
        dk::jump(at_Stage2_ArenaVIntColumns);
        return;
    }
    Stage2Vars2 *arena = g_Stage2Vars2;
    arena->split_line = arena->split_next;
    dk::jump(at_Stage2_ArenaVIntUpload);
}
