#include "game.h"

/* The translucent cones of stage 4's hall and Mr. Freeze fight ($0471C0-$047507): the raster setup,
 * the per-frame draw of one cone slot, and the a2 natives with which gas orbs and Scr2_0451B6 own
 * and steer a cone (docs/NOTES.md "Stage 4 sections and scroll"). */

/* three colour sets of four CRAM words (red, green, blue cones), indexed by ConeSlot.colour */
static constexpr uint32_t Tbl_047508 = 0x047508u;

enum {
    SCROLL_LINES_VRASTER = 0x38,   /* g_ScrollMode: line H-scroll + the per-line V-scroll raster */
    CONE_SLOTS = 3,
    CONE_FADE_DARK = -10,          /* g_FadeLevelVInt at or below it: the colours stay as faded */
    CONE_DEPTH = 0x90,             /* a new cone's projection depth */
    CONE_WIDTH_STEP = 0x20,        /* per frame toward the target width */
    CONE_LENGTH_STEP = 0x1F,       /* and length */
    CONE_HEADING_TO_VIEW = 0x100,  /* the owner's heading minus a quarter turn: the sweep's view angle */
    CONE_SWEEP_STEP = 0x0B,        /* spin_a per frame in the sweeps (+ the turn rate in CONE_SWEEP) */
    CONE_SPIN_VIEW_B = 5,          /* CONE_SPIN: view_b per frame */
    CONE_SPIN_VIEW_C = -15,        /* and view_c */
    CONE_COLOURS = 3,
    CONE_CLEARS = 2,               /* idle frames that clear the spans: both span buffers */
};

/* the colour words a cone writes into palette line 3: colours 10, 11, 12 and 14 */
static const int CONE_PAL_WORDS[4] = { 0x74 / 2, 0x76 / 2, 0x78 / 2, 0x7C / 2 };

static void set_low_word(uint32_t *r, uint16_t v) { *r = (*r & 0xFFFF0000u) | v; }
static uint16_t high_word(int32_t v) { return (uint16_t)((uint32_t)v >> 16); }
/* move.w to a long's address: its integer (high) word, the low word kept */
static void set_high_word(s32 *v, uint16_t w) { *v = (int32_t)(((uint32_t)w << 16) | ((uint32_t)(int32_t)*v & 0xFFFFu)); }

static ConeSlot *cone_slot(int i) { return (ConeSlot *)g_ConeSlots + i; }

/* Stage4_InitConeRaster - scene setup for the translucent cones: scroll mode $38 (line H-scroll +
 * the per-line V-scroll raster), then the H-int V-scroll handler for plane A
 * In:  none
 * Out: g_ScrollMode = $38; HInt_InstallVScrollA's (a jmp)
 * Evidence: move.w #$38,g_ScrollMode / jmp HInt_InstallVScrollA; called by Stage4_RunHall ($4000)
 *           and Stage4_RunFreezeFight ($4401), the scenes whose loops call Stage4_DrawCones; TAS
 *           f116191, f128791
 */
void Stage4_InitConeRaster(void)
{
    g_ScrollMode = SCROLL_LINES_VRASTER;
    dk::jump(at_HInt_InstallVScrollA);
}

/* Stage4_DrawCones - per frame: picks the next of the three cone slots (round robin) and draws it as
 * a span triangle: apex = its screen point, the far edge = two points (+-length/32, width/16, 0)
 * turned by the slot's first angle set, then projected through the second; copies its colour set
 * into palette line 3 unless the screen is faded dark. An idle slot (free, or length 0) clears the
 * spans instead, on two frames (both span buffers), then leaves them.
 * In:  g_ConeSlots, g_ConeDrawSlot; d1-d6 / a2-a4 pass into the callees
 * Out: a6 = the slot; drawn: g_TriX0..g_TriY2 and the span buffers, d1-d6 / a2 / a3 as
 *      Span_DrawTriangle left them, a4 Rot3D_BuildMatrix's; cleared: d2 = Span_ClearScreenAlt's
 * Evidence: bra.w $0471D0 / g_ConeDrawSlot + 1 (bcs #3) / mulu.w #$1E / tst.b (a6) / tst.w $1C(a6) /
 *           cmpi.b #$F6,g_FadeLevelVInt / ble / the four word pairs to g_PalFaded and g_PalSource
 *           +$74/$76/$78/$7C / st.b g_Pal3Changed / the angles into g_Rot3DOuter / two
 *           Rot3D_TransformPoint in place in g_ConeEdge / Rot3D_BuildMatrix / two Rot3D_ProjectPoint
 *           + g_TriX0/Y0 / clr.b g_ConeClearCount / jmp Span_DrawTriangle; the idle path cmpi.b
 *           #2,g_ConeClearCount / addq.b / jmp Span_ClearScreenAlt; called every frame by the $4000
 *           and $4401 loops
 */
void Stage4_DrawCones(uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6,
                      uint32_t *a2, uint32_t *a3, uint32_t *a4, uint32_t *a6)
{
    uint16_t index = (uint16_t)(g_ConeDrawSlot + 1);
    if (index >= CONE_SLOTS)
        index = 0;
    g_ConeDrawSlot = index;
    ConeSlot *cone = cone_slot(index);
    *a6 = dk::addr(cone);
    uint32_t d0 = (uint32_t)index * sizeof(ConeSlot), d7 = 0;
    /* the span routines' targets: plane A's line buffer and the back H-int line table */
    uint32_t lines = dk::addr(g_HScrollLinesA);
    uint32_t table = (uint32_t)(int32_t)(int16_t)g_HIntTblBack;

    if (!cone->in_use || cone->length == 0) {
        if (g_ConeClearCount == CONE_CLEARS)
            return;
        g_ConeClearCount = (uint8_t)(g_ConeClearCount + 1);
        uint32_t hscroll = 0, count = d7;   /* Span_ClearScreenAlt hands on d0/d7 too */
        Span_ClearScreenAlt(&hscroll, d2, &count, lines, table);
        return;
    }

    if ((int8_t)g_FadeLevelVInt > CONE_FADE_DARK) {
        PTR(const u16) set;
        set.v = Tbl_047508 + cone->colour;
        const u16 *colours = set.get();
        for (int i = 0; i < 4; i++) {
            ((u16 *)g_PalFaded)[CONE_PAL_WORDS[i]] = colours[i];
            ((u16 *)g_PalSource)[CONE_PAL_WORDS[i]] = colours[i];
        }
        g_Pal3Changed = 0xFF;
    }

    g_TriX0 = (uint16_t)cone->apex_x;
    g_TriY0 = (uint16_t)cone->apex_y;
    uint16_t shake = high_word((int32_t)g_ShakeY);
    set_low_word(&d0, shake);
    g_TriY0 = (uint16_t)(g_TriY0 - shake);

    /* the first angle set turns the far edge in place */
    Rot3D *rot = (Rot3D *)g_Rot3DOuter;
    set_high_word(&rot->tx, 0);
    set_high_word(&rot->ty, 0);
    set_high_word(&rot->tz, 0);
    rot->angle_b = cone->spin_b;
    rot->angle_a = cone->spin_a;
    rot->angle_c = cone->spin_c;
    Rot3D_BuildMatrix(&d0, d1, d2, d3, rot, a2, a3, a4);

    s16 *edge = (s16 *)g_ConeEdge;
    int16_t half = (int16_t)(cone->length >> 5), across = (int16_t)(cone->width >> 4);
    set_low_word(&d0, (uint16_t)half);
    set_low_word(d1, (uint16_t)across);
    edge[0] = half;
    edge[1] = across;
    edge[2] = 0;
    set_low_word(&d0, (uint16_t)-half);
    edge[3] = (int16_t)-half;
    edge[4] = across;
    edge[5] = 0;
    const s16 *point = edge;
    for (int i = 0; i < 2; i++) {
        Rot3D_TransformPoint(&d0, d1, d4, d5, &d7, rot, &point);
        s16 *done = edge + 3 * i;
        done[0] = (int16_t)(uint16_t)*d4;
        done[1] = (int16_t)(uint16_t)*d5;
        done[2] = (int16_t)(uint16_t)d7;
    }

    /* the second set projects it */
    rot->angle_b = cone->view_b;
    rot->angle_a = cone->view_a;
    rot->angle_c = cone->view_c;
    set_high_word(&rot->tz, (uint16_t)cone->depth);
    Rot3D_BuildMatrix(&d0, d1, d2, d3, rot, a2, a3, a4);
    point = edge;
    Rot3D_ProjectPoint(&d0, d1, d4, d5, &d7, rot, &point);
    set_low_word(d4, (uint16_t)(*d4 + g_TriX0));
    set_low_word(d5, (uint16_t)(*d5 + g_TriY0));
    g_TriX1 = (uint16_t)*d4;
    g_TriY1 = (uint16_t)*d5;
    Rot3D_ProjectPoint(&d0, d1, d4, d5, &d7, rot, &point);
    set_low_word(d4, (uint16_t)(*d4 + g_TriX0));
    set_low_word(d5, (uint16_t)(*d5 + g_TriY0));
    g_TriX2 = (uint16_t)*d4;
    g_TriY2 = (uint16_t)*d5;

    g_ConeClearCount = 0;
    /* a jmp, called: its a0 / a1 are not this prototype's to hand on, and it plays no stack tricks */
    Span_DrawTriangle(&d0, d1, d2, d3, d4, d5, d6, &d7, &lines, &table, a2, a3);
}

/* Stage4_ConeAlloc - a2 native: gives the object a free cone slot (its handle kept in the object)
 * starting at the object's position, width / length 0, every angle 0, depth $90 and the next of
 * the three colour sets; an object that has one keeps it
 * In:  self = the object (a6: +$1E its cone, +$3E/$42 x/y)
 * Out: d0 = 0 ok, 1 no free slot (Op2_CallNativeRelSwitch's case; the object's cone cleared); d7 =
 *      the slot loop's dbra count (2 - the slot found, $FFFF when all are taken)
 * Evidence: tst.w $1E(a6) / bne (moveq #0,d0) / the dbra over g_ConeSlots for a clear byte 0 / clr.w
 *           $1E(a6), moveq #1,d0 / st.b (a0), x/y to +4/+6, the clears, move.w #$90,$14(a0), the
 *           g_ConeNextColour cycle (bcs #3) / lsl.w #3 to +2 / moveq #0,d0; callers
 *           Op2_CallNativeRelSwitch at $04224E (gas orb) and $0451B6
 */
void Stage4_ConeAlloc(uint32_t *d0, uint32_t *d7, S4Obj *self)
{
    *d0 = 0;
    if (self->cone != 0)
        return;
    ConeSlot *cone = nullptr;
    *d7 = CONE_SLOTS - 1;
    for (int i = 0; i < CONE_SLOTS; i++) {
        if (!cone_slot(i)->in_use) {
            cone = cone_slot(i);
            break;
        }
        set_low_word(d7, (uint16_t)(*d7 - 1));   /* dbra */
    }
    if (!cone) {
        self->cone = 0;
        *d0 = 1;
        return;
    }
    self->cone = dk::handle_of(cone);
    cone->in_use = 0xFF;
    cone->apex_x = self->x;
    cone->apex_y = self->y;
    cone->mode = CONE_SPIN;
    cone->view_b = cone->view_a = cone->view_c = 0;
    cone->spin_b = cone->spin_a = cone->spin_c = 0;
    cone->target_width = cone->target_length = 0;
    cone->width = cone->length = 0;
    cone->depth = CONE_DEPTH;
    uint16_t colour = (uint16_t)(g_ConeNextColour + 1);
    if (colour >= CONE_COLOURS)
        colour = 0;
    g_ConeNextColour = colour;
    cone->colour = (uint16_t)(colour << 3);
}

/* ease a width or length toward its target by a step, never past it */
static int16_t ease(int16_t v, int16_t target, int16_t step)
{
    if (v < target) {
        v = (int16_t)(v + step);
        return v > target ? target : v;
    }
    v = (int16_t)(v - step);
    return v < target ? target : v;
}

/* Stage4_ConeUpdate - a2 native, every frame of the cone's owner: moves the apex to the object's
 * screen position, eases width toward its target by $20 and length by $1F, frees the slot when a
 * closing cone reaches length 0, and steps the angles by mode: spin (view angles turning, a random
 * colour each frame), sweep once (the view follows the heading, spin_a runs one turn), sweep (spin_a
 * by the owner's turn rate)
 * In:  self = the owner (a6: +$1E its cone, x/y), motion = its motion block (a3: heading +$C, turn
 *      rate +$10, their integer words)
 * Out: the slot updated (or freed); d0 / d1 as the steps leave them (the low words; moveq clears d0
 *      when a sweep-once turn completes)
 * Evidence: move.w $1E(a6),d0 / beq rts / x - g_CameraX, y - g_CameraY to +4/+6 / the two eases
 *           (cmp / blt / subi #$20 / bge ...) / bne / cmpi.b #3,1(a0) / beq.w Stage4_ConeFree / the
 *           mode cases at $047430, $047462, $047494 (Rand_Below 3, lsl.w #3 to +2); callers
 *           Op2_CallNativeRel at $0421EC and $042222 (gas orb loops)
 */
void Stage4_ConeUpdate(uint32_t *d0, uint16_t *d1, const Vm2Motion *motion, S4Obj *self)
{
    set_low_word(d0, (uint16_t)self->cone);
    if (self->cone == 0)
        return;
    ConeSlot *cone = dk::handle<ConeSlot>(self->cone);
    cone->apex_x = (int16_t)(self->x - high_word((int32_t)g_CameraX));
    cone->apex_y = (int16_t)(self->y - high_word((int32_t)g_CameraY));
    *d1 = (uint16_t)cone->target_width;
    cone->width = ease(cone->width, cone->target_width, CONE_WIDTH_STEP);
    *d1 = (uint16_t)cone->target_length;
    int16_t length = ease(cone->length, cone->target_length, CONE_LENGTH_STEP);
    set_low_word(d0, (uint16_t)length);
    cone->length = length;
    if (length == 0 && cone->mode == CONE_CLOSE) {
        set_low_word(d0, Stage4_ConeFree(self));
        return;
    }

    if (cone->mode == CONE_SPIN) {
        cone->view_b = (uint16_t)(cone->view_b + CONE_SPIN_VIEW_B);
        cone->view_a = 0;
        cone->view_c = (uint16_t)(cone->view_c + CONE_SPIN_VIEW_C);
        cone->spin_b = cone->spin_a = cone->spin_c = 0;
        set_low_word(d0, CONE_COLOURS);
        Rand_Below(d0);
        set_low_word(d0, (uint16_t)(*d0 << 3));
        cone->colour = (uint16_t)*d0;
        return;
    }
    uint16_t view = (uint16_t)(high_word(motion->heading) - CONE_HEADING_TO_VIEW);
    if (cone->mode == CONE_SWEEP) {
        uint16_t turn = (uint16_t)(high_word(motion->turn_rate) + CONE_SWEEP_STEP);
        cone->spin_a = (uint16_t)(cone->spin_a + turn);
        set_low_word(d0, view);
        cone->view_b = view;
        cone->view_a = 0;
        cone->view_c = 0;
        return;
    }
    /* CONE_SWEEP_ONCE (and a closing cone still open): spin_a runs from 1 up to one turn, then 0 */
    cone->view_b = view;
    cone->view_a = 0;
    cone->view_c = 0;
    uint16_t a = (uint16_t)(cone->spin_a & 0x3FF);
    set_low_word(d0, a);
    if (a != 0) {
        a = (uint16_t)(a + CONE_SWEEP_STEP);
        set_low_word(d0, a);
        *d1 = (uint16_t)(a & 0xFC00);
        if (*d1 != 0) {
            a = 0;
            *d0 = 0;   /* moveq */
        }
    }
    cone->spin_a = a;
}

/* Stage4_ConeSetInline - a2 native with inline arguments: byte colour (0 = keep, else colour set
 * offset + 8), byte mode (CONE_SPIN .. CONE_CLOSE), word target width, word target length; the
 * sweep also starts at a random spin_a. An object without a cone skips the arguments.
 * In:  self = the object (a6: +$1E its cone); args = the script at the 6 argument bytes (a2)
 * Out: args past them; d0 = the bytes as read (the mode in its low byte; Rand_Next's for a sweep;
 *      the low word 0 without a cone)
 * Evidence: move.w $1E(a6),d0 / beq (addq.l #6,a2) / moveq #0,d0 / move.b (a2)+,d0 / beq / subq.w
 *           #8 to +2 / move.b (a2)+,d0 to +1 / two words to +$16/+$18 / cmpi.b #2 / Rand_Next to
 *           +$A; callers Op2_CallNativeRel at $042214/$042258 and 24 times in $0451CA-$0452C6
 */
void Stage4_ConeSetInline(uint32_t *d0, const u8 **args, S4Obj *self)
{
    set_low_word(d0, (uint16_t)self->cone);
    if (self->cone == 0) {
        *args += 6;
        return;
    }
    ConeSlot *cone = dk::handle<ConeSlot>(self->cone);
    const u8 *p = *args;
    *d0 = *p++;
    if (*d0 != 0) {
        set_low_word(d0, (uint16_t)(*d0 - 8));
        cone->colour = (uint16_t)*d0;
    }
    uint8_t mode = *p++;
    *d0 = (*d0 & 0xFFFFFF00u) | mode;
    cone->mode = mode;
    cone->target_width = (int16_t)(uint16_t)((p[0] << 8) | p[1]);
    cone->target_length = (int16_t)(uint16_t)((p[2] << 8) | p[3]);
    *args = p + 4;
    if (mode == CONE_SWEEP) {
        *d0 = (uint32_t)Rand_Next();
        cone->spin_a = (uint16_t)*d0;
    }
}

/* Stage4_ConeFree - a2 native: releases the object's cone slot at once
 * In:  self = the object (a6: +$1E its cone)
 * Out: d0.w = the slot's handle (0 when it had none); the slot's in-use byte and the object's
 *      handle cleared
 * Evidence: move.w $1E(a6),d0 / beq rts / clr.w $1E(a6) / clr.b (a0); callers Op2_CallNativeRel at
 *           $042202/$042230/$04226C before Op2_Kill (gas orb deaths); Stage4_ConeUpdate branches
 *           here for a closed mode-3 cone
 */
uint16_t Stage4_ConeFree(S4Obj *self)
{
    uint16_t h = (uint16_t)self->cone;
    if (h == 0)
        return 0;
    ConeSlot *cone = dk::handle<ConeSlot>(self->cone);
    self->cone = 0;
    cone->in_use = 0;
    return h;
}
