#include "game.h"

/* the sine table: 512 signed words over a full turn (the cosine $100 bytes on) (ROM) */
static constexpr uint32_t Tbl_059B22 = 0x059B22u;
/* the lamp's a2 script (ROM data right after Stage2_SpawnLiftLamp's code; batmanrobin.asm) */
static constexpr uint32_t Scr2_033BB8 = 0x033BB8u;

enum {
    CLOUD_ROW_OFFSET = 2,          /* tile rows the cloud bank's V-scroll sits below the layer row */
    CLOUD_LINE_SHIFT_X = 0x20,     /* plane A H-scroll = wobble - (layer x - $20) */
    CLOUD_WOBBLE_MASK = 0x1FE,     /* byte offset into the 256-word period */
    CLOUD_LINES = 224,
    BEAM_SHIP_X = 0x172,           /* where the beam ship spawns (world) */
    BEAM_SHIP_Y = 0x70,
    FLIGHT_SCROLL_BEAM = 0x38,     /* g_ScrollMode: both planes' lines + the H-int V-scroll */
    BEAM_NOSE_DX = 0x18,           /* the beam starts 24 px left of the ship's x */
    BEAM_QUARTER_TURN = 0x100,
    BEAM_HEADING_MASK = 0x3FE,     /* even byte offsets into the sine table */
    SINE_COS_OFFSET = 0x80,        /* words: the cosine a quarter turn ($100 bytes) on */
};

static void set_low_word(uint32_t *r, uint16_t v) { *r = (*r & 0xFFFF0000u) | v; }
static void set_low_byte(uint32_t *r, uint8_t v) { *r = (*r & 0xFFFFFF00u) | v; }
static uint16_t view_x(void) { return (uint16_t)(g_ViewX >> 16); }   /* move.w: the integer word */
static uint16_t view_y(void) { return (uint16_t)(g_ViewY >> 16); }
/* movea.w (g_HIntTblBack).w,a1: the line table's RAM address, sign-extended */
static uint32_t hint_table(void) { return (uint32_t)(int32_t)(int16_t)(uint16_t)g_HIntTblBack; }

/* Flight_CopyWobbleWords - inner loop of Flight_BuildWobbleTable: count + 1 words, each the signed
 * high byte of every second sine word, / 16
 * In:  src = the sine table position (a0), dst = the output (a1), count = words - 1 (d7)
 * Out: src/dst advanced; count = $FFFF (the dbra's end); d0 = the last word
 * Evidence: move.b (a0),d0 / ext.w d0 / asr.w #4,d0 / move.w d0,(a1)+ / addq.l #4,a0 / dbra
 *           d7,loop / rts; bsr from Flight_BuildWobbleTable (256 words) and its fall-through (224
 *           words); TAS f46290
 */
void Flight_CopyWobbleWords(uint16_t *count, uint32_t *src, uint32_t *dst)
{
    PTR(const u8) in;
    PTR(s16) out;
    in.v = *src;
    out.v = *dst;
    const u8 *s = in.get();
    s16 *d = out.get();
    do {
        *d++ = (int16_t)((int16_t)(int8_t)*s >> 4);
        s += 4;
    } while ((*count)-- != 0);
    *src = dk::addr(s);
    *dst = dk::addr(d);
}

/* Flight_CloudBankLines - the cloud bank's line pass: the wobble phase moves on 2; plane A's
 * V-scroll = (the layer's tile row + 2) * 8 + its fine y; every line's H-scroll = the wobble word
 * at (V-scroll * 2 + phase) - (layer x - $20), 224 lines unrolled
 * In:  layer = g_LayerA (a6); g_SceneVars+$A0, the wobble table
 * Out: g_SceneVars+$A0 += 2, g_VScrollA, g_HScrollLinesA; d0 = the last line's word (upper word 0),
 *      d1's low word = layer x - $20, a0/a1 past the lines and the table window; the condition
 *      codes of the last move.w d0,(a0)+
 * Evidence: addq.w #2,g_SceneVars+$A0 / moveq #0,d0 / move.w $34(a6),d0 / addq.w #2 / lsl.w #3 /
 *           move.w 2(a6),d1 / andi.w #$F / or.b d1,d0 / move.w d0,g_VScrollA / move.w (a6),d1 /
 *           subi.w #$20 / lea g_SceneWorkBuf+$400,a1 / g_VScrollA*2 + phase, andi.w #$1FE, adda.w /
 *           lea g_HScrollLinesA,a0 / 224 x (move.w (a1)+,d0 / sub.w d1,d0 / move.w d0,(a0)+) / rts;
 *           bra.w from Flight_ScrollCloudBank; TAS f46291-f49938
 */
void Flight_CloudBankLines(uint32_t *d0, uint16_t *d1, uint32_t *a0, uint32_t *a1, uint32_t a6)
{
    PTR(const MapLayer) l;
    l.v = a6;
    const MapLayer *layer = l.get();
    FlightSceneVars *vars = g_FlightScene;
    vars->wobble_phase = (uint16_t)(vars->wobble_phase + 2);

    uint16_t vscroll = (uint16_t)((uint16_t)(layer->row + CLOUD_ROW_OFFSET) << 3);
    vscroll = (uint16_t)(vscroll | (layer->y & 0xF));   /* or.b: the fine y fits the low byte */
    g_VScrollA = vscroll;
    *d1 = (uint16_t)(layer->x - CLOUD_LINE_SHIFT_X);

    uint16_t offset = (uint16_t)((uint16_t)(vscroll * 2 + vars->wobble_phase) & CLOUD_WOBBLE_MASK);
    const s16 *wobble = g_FlightWobble + offset / 2;
    s16 *line = (s16 *)g_HScrollLinesA;
    uint16_t v = 0;
    for (int i = 0; i < CLOUD_LINES; i++) {
        v = (uint16_t)((uint16_t)*wobble++ - *d1);
        *line++ = (int16_t)v;
    }
    *d0 = v;   /* moveq #0 cleared the upper word; every later operation is a word one */
    *a0 = dk::addr(line);
    *a1 = dk::addr(wobble);
}

/* Flight_StartBeamShip - loader-script native of the final boss: the beam ship at ($172, $70)
 * (Boss_BeamShip_Spawn), the frame callback Flight_FrameBeamShip, scroll mode $38 with the plane A
 * V-scroll H-int (for the beam's spans)
 * In:  -
 * Out: d0 = 0 and Z set (Op6_CallNativeRetry: done); d4/d5's low words = the spawn point;
 *      Boss_BeamShip_Spawn's d1/a3-a6, HInt_InstallVScrollA's d7/a0/a1
 * Evidence: move.w #$172,d4 / move.w #$70,d5 / jsr Boss_BeamShip_Spawn / move.l
 *           #Flight_FrameBeamShip,g_SceneVars+$98 / move.w #$38,g_ScrollMode /
 *           HInt_InstallVScrollA / moveq #0,d0; `70BE 0003 3A62` at $330EA (Tbl_03301E) and $33242
 *           (Scr6_0331F0); TAS f77184
 */
void Flight_StartBeamShip(uint32_t *d0, uint32_t *d1, uint16_t *d4, uint16_t *d5, uint32_t *d7, uint32_t *a0,
                          uint32_t *a1, uint32_t *a3, uint32_t *a4, uint32_t *a5, uint32_t *a6)
{
    *d4 = BEAM_SHIP_X;
    *d5 = BEAM_SHIP_Y;
    Boss_BeamShip_Spawn(d1, *d4, *d5, a3, a4, a5, a6);
    g_FlightScene->frame_fn = at_Flight_FrameBeamShip;
    g_ScrollMode = FLIGHT_SCROLL_BEAM;
    HInt_InstallVScrollA(d7, a0, a1);
    *d0 = 0;
}

/* Flight_FrameBeamShip - frame callback of the beam-ship fight: the sky, then the translucent beam
 * drawn with spans on plane A: a dithered triangle from the ship's nose (g_TriX0 - $18, g_TriY0)
 * to the two sides of the spot g_TriX1/Y1 (+-g_BeamRadius across the beam), and a dithered disc of
 * g_BeamRadius between them; with the death flash (g_SceneVars+$A2) only the disc, at g_TriX0/Y0;
 * a radius of 0 clears the spans
 * In:  the registers Flight_RunScene's frame passes; g_BeamRadius, g_TriX0/Y0 (the ship),
 *      g_TriX1/Y1 (the spot)
 * Out: g_HScrollLinesA and the H-int table g_HIntTblBack (the span shapes, by the jumps to
 *      Span_ClearScreen / Span_ClearDrawDisc / Span_DrawDiscDither); g_TriX0-2/Y0-2 rewritten as
 *      the screen triangle
 * Evidence: bsr Flight_DrawSky / tst.b g_SceneVars+$A2 / the radius tests and jmps to
 *           Span_ClearScreen / Span_BuildCircle + jmp Span_ClearDrawDisc; subi.w #$18,g_TriX0 /
 *           Mot_Atan2 / subi.w #$100,d2 / andi.w #$3FE,d2 / muls.w $100(a0),d0 / muls.w (a0),d1 /
 *           add.l / swap / the vertex updates / Span_DrawTriangleDither / Span_BuildCircle / the
 *           mid-point + jmp Span_DrawDiscDither; stored by Flight_StartBeamShip; TAS f77184-f79303.
 *           The prototype gains &d0, &a0 and &a1 (measured clobbers: the only caller is
 *           Flight_RunScene's jsr (a0), whose type passes every register) so the jumps hand the
 *           span routines their radius and line tables
 */
void Flight_FrameBeamShip(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5,
                          uint32_t *d6, uint32_t *d7, uint32_t *a0, uint32_t *a1, uint32_t *a2, uint32_t *a3)
{
    uint16_t w7 = (uint16_t)*d7;
    Flight_DrawSky(d0, *d1, d2, d3, &w7, a0, *a1);
    set_low_word(d7, w7);

    if (g_FlightScene->beam_flash) {
        set_low_word(d0, g_BeamRadius);
        if ((uint16_t)*d0 == 0) {
            *a0 = dk::addr(g_HScrollLinesA);
            *a1 = hint_table();
            dk::jump(at_Span_ClearScreen);
            return;
        }
        uint16_t w4 = (uint16_t)*d4;
        Span_BuildCircle((uint16_t)*d0, d2, d3, &w4);
        set_low_word(d4, w4);
        *d0 = g_BeamRadius;   /* its upper word 0: Span_BuildCircle's moveq #0,d0 */
        set_low_word(d1, (uint16_t)(g_TriX0 - view_x()));
        set_low_word(d2, (uint16_t)(g_TriY0 - view_y()));
        *a0 = dk::addr(g_HScrollLinesA);
        *a1 = hint_table();
        dk::jump(at_Span_ClearDrawDisc);   /* the death flash: the disc alone at the ship */
        return;
    }
    if (g_BeamRadius == 0) {
        *a0 = dk::addr(g_HScrollLinesA);
        *a1 = hint_table();
        dk::jump(at_Span_ClearScreen);   /* d0: Flight_DrawSky's line pair, as the asm leaves it */
        return;
    }

    /* the beam's direction from the spot to the ship's nose */
    g_TriX0 = (uint16_t)(g_TriX0 - BEAM_NOSE_DX);
    set_low_word(d0, (uint16_t)(g_TriX0 - g_TriX1));
    set_low_word(d1, (uint16_t)(g_TriY0 - g_TriY1));
    Mot_Atan2(d0, d1, d2);
    uint16_t across = (uint16_t)((uint16_t)((uint16_t)*d2 - BEAM_QUARTER_TURN) & BEAM_HEADING_MASK);
    set_low_word(d2, across);
    PTR(const s16) sine;
    sine.v = Tbl_059B22 + across;   /* adda.w: the masked offset is positive */
    const s16 *s = sine.get();
    *a0 = dk::addr(s);
    /* half-widths across the beam: radius * cos / sin (1.15 fixed point, the high word of 2x) */
    int16_t radius = (int16_t)g_BeamRadius;
    uint32_t dx = (uint32_t)((int32_t)radius * (int32_t)(int16_t)s[SINE_COS_OFFSET]);
    uint32_t dy = (uint32_t)((int32_t)radius * (int32_t)(int16_t)s[0]);
    dx += dx;
    dy += dy;
    dx = (dx << 16) | (dx >> 16);   /* swap */
    dy = (dy << 16) | (dy >> 16);
    *d0 = dx;
    *d1 = dy;
    g_TriX2 = g_TriX1;
    g_TriY2 = g_TriY1;
    g_TriX1 = (uint16_t)(g_TriX1 - (uint16_t)dx);
    g_TriX2 = (uint16_t)(g_TriX2 + (uint16_t)dx);
    g_TriY1 = (uint16_t)(g_TriY1 - (uint16_t)dy);
    g_TriY2 = (uint16_t)(g_TriY2 + (uint16_t)dy);

    /* world to screen */
    uint16_t vx = view_x();
    set_low_word(d0, vx);
    g_TriX0 = (uint16_t)(g_TriX0 - vx);
    g_TriX1 = (uint16_t)(g_TriX1 - vx);
    g_TriX2 = (uint16_t)(g_TriX2 - vx);
    uint16_t vy = view_y();
    set_low_word(d0, vy);
    g_TriY0 = (uint16_t)(g_TriY0 - vy);
    g_TriY1 = (uint16_t)(g_TriY1 - vy);
    g_TriY2 = (uint16_t)(g_TriY2 - vy);

    *a0 = dk::addr(g_HScrollLinesA);
    *a1 = hint_table();
    Span_DrawTriangleDither(d0, d1, d2, d4, d5, *d6, d7, a0, a1);

    /* the disc at the middle of the beam's far side */
    set_low_word(d0, g_BeamRadius);
    uint16_t w4 = (uint16_t)*d4;
    Span_BuildCircle((uint16_t)*d0, d2, d3, &w4);
    set_low_word(d4, w4);
    set_low_word(d1, (uint16_t)(((int16_t)(uint16_t)(g_TriX1 + g_TriX2) >> 1) + 1));
    set_low_word(d2, (uint16_t)(((int16_t)(uint16_t)(g_TriY2 + g_TriY1) >> 1) - 1));
    *d0 = g_BeamRadius;   /* its upper word 0: Span_BuildCircle's moveq #0,d0 (the span count) */
    *a0 = dk::addr(g_HScrollLinesA);
    *a1 = hint_table();
    dk::jump(at_Span_DrawDiscDither);
}

/* Stage2_SpawnLiftLamp - y-trigger callback of the stage 2 lift: spawns a breakable lamp (script
 * Scr2_033BB8: energy $20, score $14, a +-12 px body box; glows with frame $3552 flipping, rides
 * the lift; on destruction a white flash and the dark frame $355C)
 * In:  x/y = the trigger x/y (world)
 * Out: script = Scr2_033BB8, then Obj_SpawnEnemy: Z set and a6 = the new lamp when it spawned, Z
 *      clear when the pool is short (d0 is clobbered: no caller reads it)
 * Evidence: lea Scr2_033BB8,a2 / jmp Obj_SpawnEnemy; dc.l entries in Tbl_02105A ($021074,
 *           $021082, $02109E, $0210C8, $0210F2, $02112A); TAS f22282-f31762. The prototype gains &a2
 *           (measured a clobber: Trigger_Fire saves and restores a2 around its callback) so the jump
 *           hands the script on
 */
void Stage2_SpawnLiftLamp(uint16_t *d1, uint16_t x, uint16_t y, uint32_t *a0, uint32_t *script, uint32_t *a4, uint32_t *a5, uint32_t *a6)
{
    *script = Scr2_033BB8;
    dk::jump(at_Obj_SpawnEnemy);   /* jmp: Obj_SpawnEnemy returns to our caller */
}
