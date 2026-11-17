#include "game.h"

/* the cat head's a2 script (ROM data right after this file) */
static constexpr uint32_t Scr2_03BDC6 = 0x03BDC6u;

enum {
    CATHEAD_SCROLL_CUSTOM = 4,            /* g_ScrollMode: VInt_ScrollCustom runs g_ScrollVIntFn */
    CATHEAD_SCROLL_LINES_COLUMNS = 0x2C,  /* line H-scroll + 2-cell column V-scroll */
    CATHEAD_SCROLL_PLANES_RASTER = 0x3C,  /* plane scroll + the H-int line table */
    CATHEAD_VDP_MODE3_LINES_COLUMNS = 0x8B07,   /* VDP reg $0B = $07 */
    CATHEAD_VDP_MODE3_PLANES = 0x8B03,          /* VDP reg $0B = $03 */
    CATHEAD_PIVOT_X = 0xA0,               /* the sway's pivot: the face's centre */
    CATHEAD_PIVOT_Y = 0x68,
    CATHEAD_SCREEN_LINES = 0xE0,
    CATHEAD_COLUMNS = 0x14,               /* 20 two-cell columns */
    CATHEAD_CURTAIN_LINES = 0xC0,         /* the curtain picture's height */
    CATHEAD_FLOOR_LINE = 0xAA,            /* where the face plate stands */
};

/* the integer part of a 16.16 view coordinate (the asm reads its high word) */
static inline uint16_t hi_word(uint32_t v) { return (uint16_t)(v >> 16); }

/* the H-int line table the game fills for the next frame (movea.w: the word, a handle,
   sign-extended: $9040 -> $FFFF9040) */
static inline u16 *hint_back_table(void) { return dk::handle<u16>((int16_t)g_HIntTblBack); }

/* Boss_CatHead_StartSwayRaster - switches the cat-head arena to the sway raster (the face tilts
 * and bulges): raster = Boss_CatHead_SwayRaster, scroll mode 4 with the one-shot V-int switch
 * Boss_CatHead_SwayVInt, pivot ($A0, $68)
 * In:  -
 * Out: the arena's raster/pivot_x/pivot_y; g_ScrollMode = 4, g_ScrollVIntFn
 * Evidence: move.l #Boss_CatHead_SwayRaster,(g_SceneVars).w / move.w #4,(g_ScrollMode).w /
 *           move.l #Boss_CatHead_SwayVInt,(g_ScrollVIntFn).w / move.w #$A0/#$68; Op2_CallNativeRelSaved
 *           at $03BA1A; TAS f87704 and f90023
 */
void Boss_CatHead_StartSwayRaster(void)
{
    CatHeadArena *arena = cat_head_arena();
    arena->raster = at_Boss_CatHead_SwayRaster;
    g_ScrollMode = CATHEAD_SCROLL_CUSTOM;
    g_ScrollVIntFn = at_Boss_CatHead_SwayVInt;
    arena->pivot_x = CATHEAD_PIVOT_X;
    arena->pivot_y = CATHEAD_PIVOT_Y;
}

/* Boss_CatHead_SwayVInt - one-shot V-int switch into the sway scroll: the real mode $2C, VDP reg
 * $0B = $07 (line H-scroll, 2-cell column V-scroll), then the standard mode handler
 * In:  ctrl = the VDP control port (a0, V-int); a2 passes through
 * Out: g_ScrollMode = $2C; then VInt_ScrollLinesColumns
 * Evidence: move.w #$2C,(g_ScrollMode).w / move.w #$8B07,(a0) / jmp VInt_ScrollLinesColumns;
 *           g_ScrollVIntFn from Boss_CatHead_StartSwayRaster, run by scroll mode 4; 4 corpus runs
 */
void Boss_CatHead_SwayVInt(uint32_t ctrl, uint32_t a2)
{
    g_ScrollMode = CATHEAD_SCROLL_LINES_COLUMNS;
    hw::port_write16(ctrl, CATHEAD_VDP_MODE3_LINES_COLUMNS);
    dk::jump(at_VInt_ScrollLinesColumns);
}

/* Boss_CatHead_SwayRaster - per-frame sway of the face picture: plane A's line H-scrolls are a ramp
 * of slope -shear about the pivot line, its 20 column V-scrolls a ramp of the same slope (x16)
 * about the pivot x (together a small rotation), plus a bulge added from the centre columns out
 * (each column 1.5x the last); plane B's columns all g_ViewY
 * In:  the arena's bulge, shear, pivot_x, pivot_y (Boss_CatHead_DriverSway, StartSwayRaster);
 *      d3/a1 pass to Mem_FillRamp
 * Out: g_HScrollLinesA (224 lines), g_SceneWorkBuf words 0-19 (plane A columns) and the jump to
 *      Mem_FillWords for words 20-39 (plane B columns): d0.w = view y, d7 = 20, a0 = the buffer
 *      +$28; d1 = the bulge's last step, d2 = its last half, d5 = the column slope
 * Evidence: Mem_FillRamp x2 + the dbra loop of add.w d0,(a0)+ / add.w d0,-(a1) + jmp Mem_FillWords;
 *           through g_SceneVars+0 from Boss_CatHead_FrameScroll; 6 corpus runs; the tilted face at
 *           f88000. The bulge loop never touches columns 0 and 19 (9 steps each way from the
 *           centre). d0/d7/a0 outputs (measured as clobbers): Mem_FillWords, jumped to, takes them
 */
void Boss_CatHead_SwayRaster(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t d3, uint32_t *d5, uint32_t *d7,
                             uint32_t *a0, uint32_t a1)
{
    const CatHeadArena *arena = cat_head_arena();
    int16_t slope = (int16_t)-arena->shear;                 /* neg.w / ext.l */
    uint32_t step = (uint32_t)(int32_t)slope;
    uint32_t col = 0u - (uint32_t)((int32_t)arena->pivot_x * slope);    /* muls.w / neg.l */
    uint32_t line = 0u - (uint32_t)((int32_t)arena->pivot_y * slope);
    uint32_t dst = dk::addr(g_HScrollLinesA);
    uint32_t a1v = a1;
    Mem_FillRamp(&line, step, &col, d3, CATHEAD_SCREEN_LINES, &dst, &a1v);

    /* the columns: from plane A's V-scroll, 16x the slope per column */
    uint32_t v = ((uint32_t)(uint16_t)g_VScrollA << 16) + col;
    step <<= 4;
    *d5 = step;
    dst = dk::addr(g_SceneWorkBuf);
    Mem_FillRamp(&v, step, &col, d3, CATHEAD_COLUMNS, &dst, &a1v);

    /* the bulge: added to the columns from the centre out, each step 1.5x the last */
    uint32_t acc = 0, bulge = (uint32_t)arena->bulge, half = 0;
    u16 *right = (u16 *)(g_SceneWorkBuf + 0x14);
    u16 *left = right;
    for (int i = 0; i < 9; i++) {
        acc += bulge;
        uint16_t lift = hi_word(acc);
        *right = (uint16_t)(*right + lift);
        right++;
        left--;
        *left = (uint16_t)(*left + lift);
        half = (uint32_t)((int32_t)bulge >> 1);
        bulge += half;
    }
    *d1 = bulge;
    *d2 = half;
    *d0 = (acc & 0xFFFF0000u) | hi_word(g_ViewY);
    *d7 = CATHEAD_COLUMNS;
    *a0 = dk::addr(g_SceneWorkBuf + 0x28);
    dk::jump(at_Mem_FillWords);
}

/* Boss_CatHead_StartCurtainRaster - switches the cat-head arena to the curtain raster: raster =
 * Boss_CatHead_CurtainRaster, scroll mode 4 with the one-shot V-int switch Boss_CatHead_CurtainVInt,
 * and the per-line V-scroll H-int on plane B
 * In:  -
 * Out: the arena's raster; g_ScrollMode = 4, g_ScrollVIntFn; then HInt_InstallVScrollB (which
 *      sets d7, a0, a1)
 * Evidence: move.l #Boss_CatHead_CurtainRaster,(g_SceneVars).w / move.w #4,(g_ScrollMode).w /
 *           move.l #Boss_CatHead_CurtainVInt,(g_ScrollVIntFn).w / jmp HInt_InstallVScrollB;
 *           Op2_CallNativeRelSaved at $03B962; the red curtain over the arena at f87400/f89640
 */
uint32_t Boss_CatHead_StartCurtainRaster(void)
{
    cat_head_arena()->raster = at_Boss_CatHead_CurtainRaster;
    g_ScrollMode = CATHEAD_SCROLL_CUSTOM;
    g_ScrollVIntFn = at_Boss_CatHead_CurtainVInt;
    dk::jump(at_HInt_InstallVScrollB);
    return 0;   /* d7 comes from HInt_InstallVScrollB, which runs after this returns */
}

/* Boss_CatHead_CurtainVInt - one-shot V-int switch into the curtain scroll: the real mode $3C
 * (planes + the H-int raster), VDP reg $0B = $03, then the standard mode handler
 * In:  ctrl = the VDP control port (a0, V-int); a1 passes through
 * Out: g_ScrollMode = $3C; then VInt_ScrollPlanesRaster
 * Evidence: move.w #$3C,(g_ScrollMode).w / move.w #$8B03,(a0) / jmp VInt_ScrollPlanesRaster;
 *           g_ScrollVIntFn from Boss_CatHead_StartCurtainRaster; 2 corpus runs
 */
void Boss_CatHead_CurtainVInt(uint32_t ctrl, uint32_t a1)
{
    g_ScrollMode = CATHEAD_SCROLL_PLANES_RASTER;
    hw::port_write16(ctrl, CATHEAD_VDP_MODE3_PLANES);
    dk::jump(at_VInt_ScrollPlanesRaster);
}

/* Boss_CatHead_CurtainRaster - per-frame curtain raster: squeezes plane B's $C0-line curtain
 * picture into the top h lines (h = curtain_h clamped to 1-$C0, less the shake) with a line
 * V-scroll ramp; the lines below show the plane at the view y
 * In:  the arena's curtain_h (Boss_CatHead_DriverCurtainY), g_ShakeY, g_ViewY; d2/d3/a1 pass to
 *      Mem_FillRamp
 * Out: the H-int line table g_HIntTblBack points at (Mem_FillRamp, then the jump to Mem_FillWords
 *      with d0.w = view y, d7 = the lines left, a0 = past the ramp); d1 = the ramp's step, d6 =
 *      the ramp's line count
 * Evidence: divu.w d7,d1 ($C000 / h), mulu, Mem_FillRamp + jmp Mem_FillWords; through
 *           g_SceneVars+0 from Boss_CatHead_FrameScroll; shots f87400 (curtain low) / f87560
 *           (raised) / f89640 (closed). d0/d7/a0 outputs (measured as clobbers): Mem_FillWords,
 *           jumped to, takes them
 */
void Boss_CatHead_CurtainRaster(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t d3, uint16_t *d6, uint16_t *d7,
                                uint32_t *a0, uint32_t a1)
{
    int16_t h = cat_head_arena()->curtain_h;
    if (h <= 0)
        h = 1;
    if ((uint16_t)h >= CATHEAD_CURTAIN_LINES)   /* bcs: unsigned */
        h = CATHEAD_CURTAIN_LINES;
    uint16_t lines = (uint16_t)(h - hi_word(g_ShakeY));
    uint32_t scale = 0xC000;
    if (lines != 0) {
        uint32_t q = scale / lines;             /* divu.w: never overflows ($C000 / n <= $C000) */
        scale = (scale % lines) << 16 | q;
    }
    uint32_t start = (uint32_t)(uint16_t)(CATHEAD_CURTAIN_LINES - lines) * (uint16_t)scale;   /* mulu.w */
    uint16_t step_w = (uint16_t)((uint16_t)scale - 0x100);
    uint32_t step = ((uint32_t)step_w << 16) >> 8;
    start = (start << 6) + 0xF00000u;
    uint32_t dst = dk::addr(hint_back_table());
    uint16_t below = (uint16_t)((uint16_t)(CATHEAD_CURTAIN_LINES - lines) >> 2);
    *d6 = below;
    uint32_t a1v = a1;
    *d1 = step;
    if ((int16_t)lines <= (int16_t)below) {     /* ble after sub.w d6,d7 */
        *d0 = (start & 0xFFFF0000u) | hi_word(g_ViewY);
        *d7 = CATHEAD_SCREEN_LINES;
        *a0 = dst;
        dk::jump(at_Mem_FillWords);
        return;
    }
    uint16_t ramp = (uint16_t)(lines - below);
    *d6 = ramp;
    Mem_FillRamp(&start, step, d2, d3, ramp, &dst, &a1v);
    uint16_t rest = (uint16_t)(CATHEAD_SCREEN_LINES - ramp);
    *d0 = start;
    *d7 = rest;
    *a0 = dst;
    if ((int16_t)CATHEAD_SCREEN_LINES <= (int16_t)ramp)   /* bgt not taken: no lines left */
        return;
    *d0 = (start & 0xFFFF0000u) | hi_word(g_ViewY);
    dk::jump(at_Mem_FillWords);
}

/* Boss_CatHead_StartFallRaster - switches the cat-head arena to the fall raster after the defeat:
 * raster = Boss_CatHead_FallRaster, scroll mode 4 with the one-shot V-int switch
 * Boss_CatHead_FallVInt, and the per-line V-scroll H-int on plane A
 * In:  -
 * Out: the arena's raster; g_ScrollMode = 4, g_ScrollVIntFn; then HInt_InstallVScrollA (which
 *      sets d7, a0, a1)
 * Evidence: move.l #Boss_CatHead_FallRaster,(g_SceneVars).w / move.w #4 / move.l
 *           #Boss_CatHead_FallVInt / jmp HInt_InstallVScrollA; Op2_CallNativeRelSaved at $03BB08
 *           (Scr2_03BB06, the driver's last coroutine); 3 corpus runs
 */
uint32_t Boss_CatHead_StartFallRaster(void)
{
    cat_head_arena()->raster = at_Boss_CatHead_FallRaster;
    g_ScrollMode = CATHEAD_SCROLL_CUSTOM;
    g_ScrollVIntFn = at_Boss_CatHead_FallVInt;
    dk::jump(at_HInt_InstallVScrollA);
    return 0;   /* d7 comes from HInt_InstallVScrollA, which runs after this returns */
}

/* Boss_CatHead_FallVInt - one-shot V-int switch into the fall scroll: the real mode $3C, VDP reg
 * $0B = $03, then the standard mode handler
 * In:  ctrl = the VDP control port (a0, V-int); a1 passes through
 * Out: g_ScrollMode = $3C; then VInt_ScrollPlanesRaster
 * Evidence: move.w #$3C,(g_ScrollMode).w / move.w #$8B03,(a0) / jmp VInt_ScrollPlanesRaster;
 *           g_ScrollVIntFn from Boss_CatHead_StartFallRaster; 3 corpus runs
 */
void Boss_CatHead_FallVInt(uint32_t ctrl, uint32_t a1)
{
    g_ScrollMode = CATHEAD_SCROLL_PLANES_RASTER;
    hw::port_write16(ctrl, CATHEAD_VDP_MODE3_PLANES);
    dk::jump(at_VInt_ScrollPlanesRaster);
}

/* divs.w: the 68000's signed 32/16 division; on overflow (the quotient doesn't fit 16 bits) the
   dividend stays as it was */
static uint32_t divs_w(uint32_t dividend, int16_t divisor)
{
    int32_t q = (int32_t)dividend / divisor;
    int32_t r = (int32_t)dividend % divisor;
    if (q < -32768 || q > 32767)
        return dividend;
    return (uint32_t)(uint16_t)r << 16 | (uint16_t)q;
}

/* Boss_CatHead_FallRaster - per-frame raster of the falling face plate: plane B at the view y;
 * palette line 3 darkened (-2) while the plate is past upright; plane A's line table shows
 * nothing above the plate (lines -1, -2, ...), the plate scaled into the lines above the floor
 * line $AA (fall = $AA upright, smaller = tipping over), and the floor below
 * In:  the arena's fall (Boss_CatHead_DriverFallY), g_VScrollA, g_ViewY; d2/d3/a1 pass to
 *      Mem_FillRamp (d2's low word is the divisor)
 * Out: g_VScrollB, g_Pal3TintR/G/B, the H-int line table g_HIntTblBack points at; d1 = the
 *      ramp's step, d2 = the divisor, d6 = the floor's line count; the jump to Mem_FillWords
 *      (d0 = $E0, d7 = d6, a0 = past the ramp) when floor lines are left
 * Evidence: divs.w d2,d1 ($AA00 / fall), muls #$11E, the dbra fill of -1, -2, ..., Mem_FillRamp +
 *           jmp Mem_FillWords; through g_SceneVars+0 from Boss_CatHead_FrameScroll; shots
 *           f90150/f90300 (the plate flat on the floor). A fall of 1 overflows the divs (43520 >
 *           32767): the ramp then uses $AA00 itself. d0/d7/a0 outputs (measured as clobbers):
 *           Mem_FillWords, jumped to, takes them
 */
void Boss_CatHead_FallRaster(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t d3, uint16_t *d6, uint16_t *d7,
                             uint32_t *a0, uint32_t a1)
{
    g_VScrollB = hi_word(g_ViewY);
    int16_t fall = cat_head_arena()->fall;
    uint8_t tint = fall < 0 ? (uint8_t)0xFE : 0;
    g_Pal3TintR = tint;
    g_Pal3TintG = tint;
    g_Pal3TintB = tint;

    /* above the plate: lines that show nothing (V-scroll -1, -2, ...) */
    u16 *out = hint_back_table();
    uint16_t above = CATHEAD_FLOOR_LINE;
    if (fall >= 0)
        above = (uint16_t)(above - fall);
    uint16_t floor_lines = (uint16_t)(CATHEAD_SCREEN_LINES - above);
    uint32_t v = 0xFFFFFFFFu;
    for (uint16_t n = above; n != 0; n--) {      /* dbra entered at the test: `above` lines */
        *out++ = (uint16_t)v;
        v = (v & 0xFFFF0000u) | (uint16_t)(v - 1);
    }
    uint32_t dst = dk::addr(out);
    v = (v & 0xFFFFu) << 16;                      /* swap / clr.w */

    uint32_t scale = 0xAA00;
    int32_t lines;
    if (fall < 0) {
        *d2 = (*d2 & 0xFFFF0000u) | (uint16_t)-fall;
        scale = divs_w(scale, (int16_t)*d2);
        scale = ((uint32_t)(int32_t)(int16_t)scale << 8) - 0x10000u;
        v += 0xBE0000u;
        scale = 0u - scale;
        lines = ((int32_t)(int16_t)-fall * 0x11E) >> 7;
    } else {
        if (fall == 0)
            *d2 = 1;
        else
            *d2 = (*d2 & 0xFFFF0000u) | (uint16_t)fall;
        scale = divs_w(scale, (int16_t)*d2);
        scale = ((uint32_t)(int32_t)(int16_t)scale << 8) - 0x10000u;
        uint16_t top = (uint16_t)((uint16_t)(v >> 16) + g_VScrollA + 1);
        v = (uint32_t)top << 16 | (v & 0xFFFFu);
        lines = ((int32_t)fall * 0x11E) >> 8;
    }
    uint16_t ramp = (uint16_t)lines;
    floor_lines = (uint16_t)(floor_lines - ramp);
    if ((int16_t)floor_lines < 0)                 /* bpl not taken: the ramp fills only what is left */
        ramp = (uint16_t)(ramp + floor_lines);
    *d1 = scale;
    *d6 = floor_lines;
    uint32_t a1v = a1;
    Mem_FillRamp(&v, scale, d2, d3, ramp, &dst, &a1v);
    *d0 = v;
    *d7 = floor_lines;
    *a0 = dst;
    if ((int16_t)floor_lines <= 0)
        return;
    *d0 = (v & 0xFFFF0000u) | CATHEAD_SCREEN_LINES;
    dk::jump(at_Mem_FillWords);
}

/* Boss_CatHead_Spawn - trigger callback that spawns the cat-head boss of the stage 3 backlot arena
 * (an attack-schedule coroutine, then the head body at $03B21C that spawns both paws and the
 * raster driver)
 * In:  d4/d5 = position (trigger entry)
 * Out: script (a2) = Scr2_03BDC6, then Obj_SpawnBoss's results
 * Evidence: lea Scr2_03BDC6,a2 / jmp Obj_SpawnBoss; the only entry of Tbl_033F20, loaded by the
 *           arena scene $03FFBE; the head is the object at (160,99) in RAM f88320. a2 an output
 *           (measured as a clobber): Obj_SpawnBoss, jumped to, takes the script in it
 */
void Boss_CatHead_Spawn(uint16_t *d1, uint16_t x, uint16_t y, uint32_t *a0, uint32_t *a4, uint32_t *a5,
                        uint32_t *a6, uint32_t *script)
{
    *script = Scr2_03BDC6;
    dk::jump(at_Obj_SpawnBoss);
}
