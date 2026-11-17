#include "game.h"

/* the balloon boss's a2 script (ROM data after this file's code; batmanrobin.asm) */
static constexpr uint32_t Scr2_01C4E6 = 0x01C4E6u;
static constexpr uint16_t BALLOON_BLOCKS = 7;

static constexpr int16_t SPLIT_SCREEN_LINES = 0xA0;   /* d0 = $A0 - line: >= $A0 means line <= 0 */
static constexpr uint16_t VDP_HINT_OFF = 0x8004;      /* reg 0: H-int off */
static constexpr uint16_t VDP_HINT_ON = 0x8014;       /* reg 0: H-int on */
static constexpr uint16_t VDP_HINT_LINE = 0x8A9F;     /* reg 10 = $9F - d0 = line - 1 */
static constexpr uint16_t VDP_HINT_NEVER = 0x8AFF;    /* reg 10 = $FF: no more H-ints this frame */
static constexpr int16_t SPLIT_VSCROLL_BIAS = 0x100;  /* the balloon picture's V scroll = $1A0 - line */
static constexpr int16_t SPLIT_HSCROLL_BIAS = 0x7F;
static constexpr uint32_t VSRAM_WRITE_0 = 0x40000010u;   /* VSRAM 0: plane A's V scroll */
static constexpr uint32_t VRAM_WRITE_FC00 = 0x7C000002u; /* VRAM $FC00: plane A's H scroll */
static constexpr uint16_t OPCODE_JMP_ABS_L = 0x4EF9;     /* jmp (xxx).l: the RAM H-int stub */

/* g_SceneVars during the balloon fight: the stage's plane-A scroll, saved for the H-int */
typedef struct BalloonSceneVars {
    u16 stage_hscroll;              /* +$00 */
    u16 stage_vscroll;              /* +$02 */
} BalloonSceneVars;

/* Boss_JokerBalloon_VIntSplit - V-int object of the Joker balloon boss: scrolls plane A so its
 * balloon picture sits at the boss's position above screen line (y - g_ViewY), and arms an H-int on
 * that line that puts the stage's own plane-A scroll back for the rest of the screen
 * In:  vdp_ctrl/vdp_data = the VDP ports (a0/a1); split = the V-int object (a6: x/y from
 *      Boss_JokerBalloon_SyncSplit)
 * Out: line <= 0: reg 0 = $04 (H-int off), nothing else; else VDP reg 10 = line - 1, H-int on
 *      (reg 0 = $14) through the g_HIntCode jmp stub -> Boss_JokerBalloon_HIntRestore; the balloon V
 *      scroll $1A0 - line and H scroll x - g_ViewX - $7F written to the VDP (g_FrameBusy: the main
 *      loop is mid-frame) or swapped into g_VScrollA/g_HScrollA with the stage values saved;
 *      d0/d1/d2 clobbered
 * Evidence: move.w $1A(a6),d0 / sub.w g_ViewY,d0 / neg.w / addi.w #$A0 / cmpi.w #$A0 / blt / move.w
 *           #$8004,(a0) / rts; #$8A9F - d0 / addi.w #$100,d0 / $18(a6) - g_ViewX - $7F / tst.b
 *           g_FrameBusy / VSRAM $40000010, VRAM $7C000002; move.w #$4EF9,g_HIntCode / move.l #$1C468,
 *           g_HIntOperand / move.w #$8014,(a0); TAS f18913-f21099 (2187 frames)
 */
void Boss_JokerBalloon_VIntSplit(uint32_t vdp_ctrl, uint32_t vdp_data, const BalloonSplitObj *split)
{
    int16_t from_bottom = (int16_t)(SPLIT_SCREEN_LINES - (int16_t)(split->y - (int16_t)(g_ViewY >> 16)));
    if (from_bottom >= SPLIT_SCREEN_LINES) {
        hw::port_write16(vdp_ctrl, VDP_HINT_OFF);
        return;
    }
    uint16_t hint_line = (uint16_t)(VDP_HINT_LINE - from_bottom);
    uint16_t vscroll = (uint16_t)(from_bottom + SPLIT_VSCROLL_BIAS);
    uint16_t hscroll = (uint16_t)(split->x - (int16_t)(g_ViewX >> 16) - SPLIT_HSCROLL_BIAS);
    if (g_FrameBusy) {
        hw::port_write32(vdp_ctrl, VSRAM_WRITE_0);
        hw::port_write16(vdp_data, vscroll);
        hw::port_write32(vdp_ctrl, VRAM_WRITE_FC00);
        hw::port_write16(vdp_data, hscroll);
    } else {
        BalloonSceneVars *saved = (BalloonSceneVars *)g_SceneVars;
        saved->stage_hscroll = g_HScrollA;
        saved->stage_vscroll = g_VScrollA;
        g_HScrollA = hscroll;
        g_VScrollA = vscroll;
    }
    hw::port_write16(vdp_ctrl, hint_line);
    *(u16 *)g_HIntCode = OPCODE_JMP_ABS_L;
    *(u32 *)&g_HIntOperand = at_Boss_JokerBalloon_HIntRestore;
    hw::port_write16(vdp_ctrl, VDP_HINT_ON);
}

/* Boss_JokerBalloon_HIntRestore - H-int handler of the Joker balloon split: at the split line
 * writes the saved stage plane-A scroll back to VSRAM 0 and VRAM $FC00 and turns further H-ints
 * off (reg 10 = $FF)
 * In:  H-int (via the jmp stub at g_HIntCode)
 * Out: rte
 * Evidence: target long $1C468 stored at g_HIntOperand by Boss_JokerBalloon_VIntSplit; move.l
 *           #$40000010,$C00004 / move.w g_SceneVars+2,$C00000 / move.l #$7C000002 / move.w
 *           g_SceneVars / move.w #$8AFF / rte
 */
void Boss_JokerBalloon_HIntRestore(void)
{
    const BalloonSceneVars *saved = (const BalloonSceneVars *)g_SceneVars;
    hw::port_write32(hw::VDP_CTRL, VSRAM_WRITE_0);
    hw::port_write16(hw::VDP_DATA, saved->stage_vscroll);
    hw::port_write32(hw::VDP_CTRL, VRAM_WRITE_FC00);
    hw::port_write16(hw::VDP_DATA, saved->stage_hscroll);
    hw::port_write16(hw::VDP_CTRL, VDP_HINT_NEVER);
    dk::rte();
}

/* Boss_JokerBalloon_SyncSplit - balloon-boss native: copies the boss's x/y to its scroll-split V-int
 * object once a frame
 * In:  self = the balloon boss (a6)
 * Out: the split object's x/y; a0 clobbered
 * Evidence: movea.w $1E(a6),a0 / move.w $3E(a6),$18(a0) / move.w $42(a6),$1A(a0); op $2CB2
 *           (CallNativeRel) at $1C5D8 in the boss's frame loop; 3 corpus runs incl. the TAS
 */
void Boss_JokerBalloon_SyncSplit(const BalloonObj *self)
{
    BalloonSplitObj *split = dk::handle<BalloonSplitObj>(self->split);
    split->x = self->x;
    split->y = self->y;
}

/* Boss_JokerBalloon_HideSplit - balloon-boss native: sets the split object's y to 0, so its next
 * V-int turns the H-int split off and leaves plane A to the stage
 * In:  self = the balloon boss (a6)
 * Out: the split object's y = 0; a0 clobbered
 * Evidence: movea.w $1E(a6),a0 / clr.w $1A(a0); op $2CB2 at $1D8B2 in Scr2_01D470; TAS only
 */
void Boss_JokerBalloon_HideSplit(const BalloonObj *self)
{
    dk::handle<BalloonSplitObj>(self->split)->y = 0;
}

/* Boss_JokerBalloon_StartSplit - balloon-boss native: allocates a V-int list object, gives it the
 * handler Boss_JokerBalloon_VIntSplit with y 0 and keeps it in the boss
 * In:  self = the balloon boss (a6)
 * Out: d0 = 0, or 1 when the V-int pool is empty (the script retries); a1 as VIntObj_NewIdle left
 *      it; a0 clobbered
 * Evidence: move.l a6,-(a7) / jsr VIntObj_NewIdle / movea.l a6,a0 / movea.l (a7)+,a6 / bne
 *           (VIntObj_NewIdle's moveq #0 / #-1) / move.w a0,$1E(a6) / clr.w $1A(a0) / move.l #$1C3F2,
 *           (a0) / moveq #0,d0; op $2CB2 at $1C50C at the head of Scr2_01C4E6; TAS only
 */
void Boss_JokerBalloon_StartSplit(uint32_t *d0, uint32_t *a1, BalloonObj *self)
{
    uint32_t a0 = 0, obj = ADDR68(self);
    VIntObj_NewIdle(d0, &a0, a1, &obj);   /* the new block comes back in a6 */
    if (*d0 != 0) {
        *d0 = 1;
        return;
    }
    PTR(BalloonSplitObj) split;
    split.v = obj;
    self->split = (int16_t)(uint16_t)obj;
    split->y = 0;
    split->handler = at_Boss_JokerBalloon_VIntSplit;
    *d0 = 0;
}

/* Boss_JokerBalloon_Spawn - trigger callback of the parade truck: spawns the Joker balloon boss
 * (boss class, 7 pool blocks) running Scr2_01C4E6
 * In:  d4/d5 = x/y (trigger entry Tbl_0166EA +$24C)
 * Out: script = Scr2_01C4E6, d7 = 7; then Obj_SpawnBossN's
 * Evidence: lea Scr2_01C4E6,a2 / move.w #7,d7 / jmp Obj_SpawnBossN; the object at $B72C runs
 *           Scr2_01C4E6 from f18840 (shot f19260). a2 and d7 outputs (measured as clobbers):
 *           Obj_SpawnBossN, jumped to, takes them
 */
void Boss_JokerBalloon_Spawn(uint16_t *d1, uint16_t d4, uint16_t d5, uint16_t *d7, uint32_t *a0, uint32_t *script,
                             uint32_t *a4, uint32_t *a5, uint32_t *a6)
{
    *script = Scr2_01C4E6;
    *d7 = BALLOON_BLOCKS;
    dk::jump(at_Obj_SpawnBossN);
}
