#include "game.h"

/* the lift scene's stage header, palette lines 0 and 3, a6 graphics script and y-trigger table
   (ROM) */
static constexpr uint32_t Scr6_032E0E = 0x032E0Eu;
static constexpr uint32_t Tbl_015634 = 0x015634u;
static constexpr uint32_t Scr6_032DAE = 0x032DAEu;
static constexpr uint32_t Scr6_032E4A = 0x032E4Au;
static constexpr uint32_t Tbl_02105A = 0x02105Au;

enum {
    LIFT_MUSIC = 0x0C,
    LIFT_FLOOR_Y = 0xC4,          /* g_FloorY: the lift platform's flat floor */
    LIFT_MAP_START_Y = 0x1910,    /* g_MapScrollY's integer part: the bottom of the shaft */
    LIFT_LAYER_START_X = 0xFFF0,  /* plane A moved 16 px left */
    LIFT_FULL_HEALTH = 0x100,
};

static void set_low_word(uint32_t *r, uint16_t v) { *r = (*r & 0xFFFF0000u) | v; }

/* Trigger_RunY with the registers' next / table (a2 / a3) as typed pointers and back */
static void run_triggers_y(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t d3, uint32_t *d4, uint32_t *d5,
                           uint32_t *d6, uint32_t *d7, uint32_t *a1, uint32_t *a2, uint32_t *a3, uint32_t *a4,
                           uint32_t a5, uint32_t a6)
{
    PTR(const TriggerKey) next;
    PTR(const u8) table;
    next.v = *a2;
    table.v = *a3;
    const TriggerKey *n = next;
    const u8 *t = table;
    Trigger_RunY(d0, d1, d2, d3, d4, d5, d6, d7, a1, &n, &t, a4, a5, a6);
    *a2 = dk::addr(n);
    *a3 = dk::addr(t);
}

/* Stage2_RunLift - section $2100: the stage 2 zeppelin lift and the arena boss: music $C, the stage
 * init, the floor y $C4, palette lines 0 and 3, the scene art, plane A placed at the shaft's bottom
 * (map y $1910, 16 px left) and drawn, the V-int tile-queue flush, the y-trigger table Tbl_02105A,
 * the lift's V-int scroll, the camera locked, the lift started upward and both players' health
 * refilled; then every frame: the lift, the y-triggers, every object and the camera's x (or the
 * debug free camera), the lift / arena scroll, until the section's exit (in Frame_End) unwinds it
 * In:  the registers the section runner passes through
 * Out: never returns here: the scene-exit path ends the section
 * Evidence: move.w #$C,d0 / Sound_PlayMusic / lea Scr6_032E0E(pc),a6 / Scene_InitStage / move.w
 *           #$C4,g_FloorY / Pal_LoadLine0 Tbl_015634 / Pal_LoadLine3 Scr6_032DAE / Scr6_032E4A
 *           VM6_Run / lea g_LayerA,a6 / move.w #$1910,d0 / move.w d0,g_MapScrollY / Layer_MoveY /
 *           move.w #$FFF0,d0 / Layer_MoveX / Layer_DrawScreen / VIntObj_New
 *           Layer_FlushTileQueueIfIdle / Trigger_LoadY Tbl_02105A / Stage2_LiftInstallVInt /
 *           Camera_LockHere / Stage2_LiftStart / move.w #$100,g_Player1+4 / g_Player2+4; the loop
 *           Frame_BeginStage / tst.b g_FreeCam / Debug_FreeCamStep or Stage2_LiftUpdate /
 *           Trigger_RunY / Obj_RunList / Camera_TrackPlayersX, then Stage2_UpdateScroll / Frame_End;
 *           Tbl_00A0AC entry $2100; TAS f21125 (stage 2 start), level-skip scenario f4080
 */
void Stage2_RunLift(uint32_t *d1, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *a1,
                    uint32_t *a2, uint32_t *a3, uint32_t *a4, uint32_t *a5, uint32_t *a6)
{
    uint32_t d0 = 0, d2 = 0, d7 = 0, a0 = 0;   /* not inputs (measured): the callees set them */
    set_low_word(&d0, LIFT_MUSIC);
    Sound_PlayMusic(&d0, d1, &d7, *a1, *a2, *a3, *a4, *a5, *a6);
    *a6 = Scr6_032E0E;
    Scene_InitStage(&d0, d1, &d2, d3, d4, d5, d6, &d7, a2, a3, a4, a6);
    g_FloorY = LIFT_FLOOR_Y;
    dk::Flags cc{};   /* the condition codes nothing here reads */
    a0 = Tbl_015634;
    Pal_LoadLine0(&a0, a1, &cc);
    a0 = Scr6_032DAE;
    Pal_LoadLine3(&a0, a1, &cc);
    *a6 = Scr6_032E4A;
    /* the script runs to its end (Op6_End) and leaves its registers to the rest */
    VM6_Run(&d0, d1, &d2, d3, d4, d5, d6, &d7, a1, a2, a3, a4, a5, a6);

    MapLayer *layer = (MapLayer *)g_LayerA;
    *a6 = dk::addr(layer);
    set_low_word(&d0, LIFT_MAP_START_Y);
    g_MapScrollY = (g_MapScrollY & 0xFFFFu) | ((uint32_t)LIFT_MAP_START_Y << 16);   /* move.w: the integer word */
    uint16_t w0 = (uint16_t)d0, w1 = (uint16_t)*d1, w2 = (uint16_t)d2;
    Layer_MoveY(&w0, &w1, &w2, layer);
    set_low_word(&d0, w0);
    set_low_word(d1, w1);
    set_low_word(&d2, w2);
    set_low_word(&d0, LIFT_LAYER_START_X);
    Layer_MoveX((uint16_t)d0, layer);
    Layer_DrawScreen(&d2, d3, d4, d5, d6, d7, a3, a4, *a5, a6);
    a0 = at_Layer_FlushTileQueueIfIdle;
    VIntObj_New(&d0, a0, a1, a6);
    PTR(const u8) triggers;
    triggers.v = Tbl_02105A;
    TriggerKey *dst = nullptr;
    Trigger_LoadY(d6, triggers.get(), &dst);
    *a3 = dk::addr(dst);
    Stage2_LiftInstallVInt(*a6);
    Camera_LockHere();
    d0 = Stage2_LiftStart();
    ((PlayerRec *)g_Player1)->health = LIFT_FULL_HEALTH;
    ((PlayerRec *)g_Player2)->health = LIFT_FULL_HEALTH;
    for (;;) {
        Frame_BeginStage(&d0, d1, &d2, d3, d4, d5, d6, &d7, &a0, a1, a2, a3, a4, a5, a6);
        if (g_FreeCam) {
            Debug_FreeCamStep(&d0, d1, &d2, d3, d4, d5, d6, &d7, &a0, a1, a2, *a3, *a4, *a5, a6);
        } else {
            Stage2_LiftUpdate(&d0, d1, &d2, (uint16_t)*d4, (uint16_t)*d5, &d7, a2, a3, a4, a5, a6);
            run_triggers_y(&d0, d1, &d2, *d3, d4, d5, d6, &d7, a1, a2, a3, a4, *a5, *a6);
            Obj_RunList(&d0, d1, &d2, d3, d4, d5, d6, &d7, &a0, a1, a2, a3, a4, a5, a6);
            Camera_TrackPlayersX(d1, &d2);
        }
        Stage2_UpdateScroll(d1, &d2, d3, d4, d5, d6, d7, *a1, a2, a3, a4, a5, a6);
        /* the section's exit runs in here and never comes back */
        Frame_End(&d0, d1, &d2, d3, d4, d5, d6, &d7, a2, a3, a4, *a5, *a6);
    }
}
