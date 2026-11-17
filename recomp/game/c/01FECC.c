#include "game.h"

/* the parade truck's stage header, palette lines 0 and 3, a6 graphics script and trigger table;
   the street callback's a4 script (ROM) */
static constexpr uint32_t Scr6_01FDF6 = 0x01FDF6u;
static constexpr uint32_t Tbl_015634 = 0x015634u;
static constexpr uint32_t Tbl_01FA36 = 0x01FA36u;
static constexpr uint32_t Scr6_01FE32 = 0x01FE32u;
static constexpr uint32_t Tbl_0166EA = 0x0166EAu;
static constexpr uint32_t Scr4_01FF8C = 0x01FF8Cu;

enum {
    STAGE1_MUSIC_TRUCK = 0x0D,
    STAGE1_TRUCK_LAYER_Y = 8,       /* Layer_MoveYWithCamera: plane A's offset from the camera */
    STAGE1_SCROLL_PLANES = 0x08,    /* g_ScrollMode: whole-plane scroll */
    STAGE1_FULL_HEALTH = 0x100,
    STAGE1_PLANE_B_SPEED = 0x10,    /* plane B (the road) streams left 16 px a frame */
};

static void set_low_word(uint32_t *r, uint16_t v) { *r = (*r & 0xFFFF0000u) | v; }

/* Trigger_LoadX on a ROM table, its typed results back into the registers. The stage tables are
   never empty, so its src / dst (a2 / a3) are written before they are read. */
static void load_triggers(uint32_t table, uint32_t *d6, uint32_t *d7, uint32_t *a0, uint32_t *a1, uint32_t *a2,
                          uint32_t *a3)
{
    PTR(const u8) rom;
    rom.v = table;
    const u8 *t = rom.get();
    TriggerKey *end = nullptr, *src = nullptr, *dst = nullptr;
    uint16_t count = (uint16_t)*d7;
    Trigger_LoadX(d6, &count, &t, &end, &src, &dst);
    set_low_word(d7, count);
    *a0 = dk::addr(t);
    *a1 = dk::addr(end);
    *a2 = dk::addr(src);
    *a3 = dk::addr(dst);
}

/* Stage1_RunTruck - section $1400: the stage 1 parade truck: music $D, the stage init with
 * Scr6_01FDF6, palette lines 0 and 3, plane A placed 8 px under the camera and drawn, the triggers
 * Tbl_0166EA, whole-plane scroll (mode 8), both players' health refilled; then every frame the
 * stage frame with the animated tiles, the layer-A scroll, Scroll_SetPlaneA and plane B streaming
 * left 16 px, until the section's exit unwinds it
 * In:  the registers the section runner passes through
 * Out: never returns here
 * Evidence: move.w #$D,d0 / Sound_PlayMusic / lea Scr6_01FDF6(pc),a6 / Scene_InitStage /
 *           Pal_LoadLine0 Tbl_015634 / Pal_LoadLine3 Tbl_01FA36 / Scr6_01FE32 VM6_Run / g_LayerA
 *           Layer_MoveYWithCamera 8 / Layer_DrawScreen / VIntObj_New Layer_FlushTileQueueIfIdle /
 *           Trigger_LoadX Tbl_0166EA / move.w #8,g_ScrollMode / the health refill / the loop with
 *           Layer_AnimateTiles, Layer_ScrollStep, Scroll_SetPlaneA, subi.w #$10,g_HScrollB;
 *           Tbl_00A0AC maps $1400 here; TAS f13217
 */
void Stage1_RunTruck(uint32_t *d1, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *a1,
                     uint32_t *a2, uint32_t *a3, uint32_t *a4, uint32_t *a5, uint32_t *a6)
{
    uint32_t d0 = 0, d2 = 0, d7 = 0, a0 = 0;   /* not inputs (measured): the callees set them */
    set_low_word(&d0, STAGE1_MUSIC_TRUCK);
    Sound_PlayMusic(&d0, d1, &d7, *a1, *a2, *a3, *a4, *a5, *a6);
    *a6 = Scr6_01FDF6;
    Scene_InitStage(&d0, d1, &d2, d3, d4, d5, d6, &d7, a2, a3, a4, a6);
    dk::Flags cc{};   /* the condition codes nothing here reads */
    a0 = Tbl_015634;
    Pal_LoadLine0(&a0, a1, &cc);
    a0 = Tbl_01FA36;
    Pal_LoadLine3(&a0, a1, &cc);
    *a6 = Scr6_01FE32;
    /* the script runs to its end (Op6_End) and leaves its registers to the rest */
    VM6_Run(&d0, d1, &d2, d3, d4, d5, d6, &d7, a1, a2, a3, a4, a5, a6);

    MapLayer *layer = (MapLayer *)g_LayerA;
    *a6 = dk::addr(layer);
    set_low_word(&d0, STAGE1_TRUCK_LAYER_Y);
    Layer_MoveYWithCamera(STAGE1_TRUCK_LAYER_Y, *a6);
    Layer_DrawScreen(&d2, d3, d4, d5, d6, d7, a3, a4, *a5, a6);
    VIntObj_New(&d0, at_Layer_FlushTileQueueIfIdle, a1, a6);
    load_triggers(Tbl_0166EA, d6, &d7, &a0, a1, a2, a3);
    g_ScrollMode = STAGE1_SCROLL_PLANES;
    ((PlayerRec *)g_Player1)->health = STAGE1_FULL_HEALTH;
    ((PlayerRec *)g_Player2)->health = STAGE1_FULL_HEALTH;
    for (;;) {
        Frame_BeginStage(&d0, d1, &d2, d3, d4, d5, d6, &d7, &a0, a1, a2, a3, a4, a5, a6);
        Frame_RunWorldLayerA(d0, d1, &d2, d3, d4, d5, d6, d7, &a0, a1, a2, a3, a4, a5, a6);
        *a6 = dk::addr(layer);
        uint16_t w1 = (uint16_t)*d1, w5 = (uint16_t)*d5;
        Layer_AnimateTiles(&w1, &d2, d3, d4, &w5, a0, a4, a5, layer);
        set_low_word(d1, w1);
        set_low_word(d5, w5);
        uint16_t w4 = (uint16_t)*d4, w6 = (uint16_t)*d6, w7 = (uint16_t)d7;
        Layer_ScrollStep(d1, &d2, d3, &w4, d5, &w6, &w7, &a0, a2, a3, layer);
        set_low_word(d4, w4);
        set_low_word(d6, w6);
        set_low_word(&d7, w7);
        uint16_t fine = (uint16_t)d2;
        Scroll_SetPlaneA(&d0, d1, &fine, a6);
        set_low_word(&d2, fine);
        g_HScrollB = (uint16_t)(g_HScrollB - STAGE1_PLANE_B_SPEED);
        /* the section's exit runs in here and never comes back */
        Frame_End(&d0, d1, &d2, d3, d4, d5, d6, &d7, a2, a3, a4, *a5, *a6);
    }
}

/* unk_Stage1_01FF78 - street trigger callback: starts the a4 object Scr4_01FF8C at the trigger x/y;
 * it shows RAM-bank frame $FC40 (4 pieces, 64 px wide, from 31 px above) until it leaves the
 * screen; its role is not seen
 * In:  d4/d5 = the trigger x/y (entries at x $801 and $8A9, y $70 in Tbl_015F5A), integer parts
 *      in the low words
 * Out: d4/d5 with their words swapped (16.16 for the a4 object); a4 = the script; the registers as
 *      the script left them (Fx_RunScriptNoParent); d0 = 0, Z set
 * Evidence: swap d4 / swap d5 / lea Scr4_01FF8C,a4 / jsr Fx_RunScriptNoParent / moveq #0,d0 / rts;
 *           a4 objects at (2049,112) and (2217,112) = the two bank-sign hanging points in RAM
 *           f1680-f1980
 */
uint32_t unk_Stage1_01FF78(uint32_t d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5,
                           uint32_t *d6, uint32_t *d7, uint32_t *a0, uint32_t *a1, uint32_t *a2, uint32_t *a3,
                           uint32_t *a4, uint32_t *a5, uint32_t *a6)
{
    *d4 = (*d4 << 16) | (*d4 >> 16);
    *d5 = (*d5 << 16) | (*d5 >> 16);
    *a4 = Scr4_01FF8C;
    /* the a4 script runs on the registers as this callback has them (d2-d5: radius, heading, x, y)
       and leaves its own in them (the prototype passes every register, as the script's) */
    Fx_RunScriptNoParent(&d0, d1, d2, d3, d4, d5, d6, d7, a0, a1, a2, a3, a4, a5, a6);
    return 0;
}
