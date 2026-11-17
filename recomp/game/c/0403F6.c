#include "game.h"

/* the card hall's stage header, palette lines 0 and 3, a6 graphics script, trigger table and
   parallax bands (ROM) */
static constexpr uint32_t Tbl_04036E = 0x04036Eu;
static constexpr uint32_t Tbl_015634 = 0x015634u;
static constexpr uint32_t Tbl_03FDE8 = 0x03FDE8u;
static constexpr uint32_t Tbl_0403AA = 0x0403AAu;
static constexpr uint32_t Tbl_034142 = 0x034142u;
static constexpr uint32_t Tbl_00978E = 0x00978Eu;

enum {
    MUSIC_CARD_HALL = 0x0A,
    CARD_HALL_FLOOR_Y = 0x9C,  /* g_FloorY: the checkerboard floor */
};

static void set_low_word(uint32_t *r, uint16_t v) { *r = (*r & 0xFFFF0000u) | v; }

/* Trigger_LoadX on a ROM table, its typed results back into the registers (the stage tables are
   never empty, so its src / dst are written before they are read) */
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

/* Stage3_RunCardHall - section $3400, the stage 3 card hall: music $A, the stage init, the flat
 * floor at y $9C, palettes 0 and 3, the scene art, the triggers; then every frame the world with
 * layer A, the perspective floor (Stage3_CardHallFloorScroll) and the parallax bands Tbl_00978E,
 * until the section's exit (in Frame_End) unwinds it
 * In:  the registers the section runner passes through
 * Out: never returns here: the scene-exit path ends the section
 * Evidence: move.w #$A,d0 / Sound_PlayMusic / lea Tbl_04036E(pc),a6 / Scene_InitStage / move.w
 *           #$9C,g_FloorY / Pal_LoadLine0 Tbl_015634 / Pal_LoadLine3 Tbl_03FDE8 / Tbl_0403AA VM6_Run
 *           / Trigger_LoadX Tbl_034142; the loop Frame_BeginStage / Frame_RunWorldLayerA / bsr.w
 *           Stage3_CardHallFloorScroll / Scroll_RunParallax Tbl_00978E / Frame_End; Tbl_00A0AC
 *           entry $3400; TAS f108293 (png f108600: card portraits above a checkerboard floor)
 */
void Stage3_RunCardHall(uint32_t *d1, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *a1,
                        uint32_t *a2, uint32_t *a3, uint32_t *a4, uint32_t *a5, uint32_t *a6)
{
    uint32_t d0 = 0, d2 = 0, d7 = 0, a0 = 0;   /* not inputs (measured): the callees set them */
    set_low_word(&d0, MUSIC_CARD_HALL);
    Sound_PlayMusic(&d0, d1, &d7, *a1, *a2, *a3, *a4, *a5, *a6);
    *a6 = Tbl_04036E;
    Scene_InitStage(&d0, d1, &d2, d3, d4, d5, d6, &d7, a2, a3, a4, a6);
    g_FloorY = CARD_HALL_FLOOR_Y;
    dk::Flags cc{};   /* the condition codes nothing here reads */
    a0 = Tbl_015634;
    Pal_LoadLine0(&a0, a1, &cc);
    a0 = Tbl_03FDE8;
    Pal_LoadLine3(&a0, a1, &cc);
    *a6 = Tbl_0403AA;
    /* the script runs to its end (Op6_End) and leaves its registers to the rest */
    VM6_Run(&d0, d1, &d2, d3, d4, d5, d6, &d7, a1, a2, a3, a4, a5, a6);
    load_triggers(Tbl_034142, d6, &d7, &a0, a1, a2, a3);
    for (;;) {
        Frame_BeginStage(&d0, d1, &d2, d3, d4, d5, d6, &d7, &a0, a1, a2, a3, a4, a5, a6);
        Frame_RunWorldLayerA(d0, d1, &d2, d3, d4, d5, d6, d7, &a0, a1, a2, a3, a4, a5, a6);
        uint16_t w6 = (uint16_t)*d6, w7 = (uint16_t)d7;
        Stage3_CardHallFloorScroll(&d0, d1, &d2, *d3, &w7, a1);
        PTR(const u16) bands;
        bands.v = Tbl_00978E;
        const u16 *list = bands.get();
        Scroll_RunParallax(&d0, *d1, &d2, d3, &w6, &w7, a1, &list, a6);
        *a5 = dk::addr(list);
        set_low_word(d6, w6);
        set_low_word(&d7, w7);
        /* the section's exit runs in here and never comes back */
        Frame_End(&d0, d1, &d2, d3, d4, d5, d6, &d7, a2, a3, a4, *a5, *a6);
    }
}
