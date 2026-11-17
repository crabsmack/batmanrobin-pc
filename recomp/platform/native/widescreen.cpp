/* widescreen.cpp - the native runner's 16:9 view (docs/phases/P6-enhance.md E3; rulings R4, R5,
 * R11, R12, R16; docs/NOTES.md "Phase 6: widescreen"). The model is X-Men 2's
 * platform/native/widescreen.cpp.
 *
 * The view is 400x224: 40 px beside each side of the original 320. A screen is wide or 4:3 by the
 * scene it runs (a stage section by g_SectionId; a plain scene by the header Scene_InitCommon
 * started it from); s_screens says which and recomp/game/widescreen_screens.toml why (E3 compares
 * the two). Everything here reads the game through the 68000's view of memory (nm_ram_read8, the
 * ROM) and the VDP's, never the C's variables, so the checks are independent of the code they check.
 */
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
#include <process.h>
#define ws_getpid _getpid
#else
#include <unistd.h>
#define ws_getpid getpid
#endif
#include <string>
#include <vector>

extern "C" {
#include "video/genesis_machine.h"
}
#include "enhance.h"
#include "native_machine.h"
#include "options.h"
#include "present.h"
#include "spr_host.h"
#include "widescreen.h"
#include "ws_draw.h"

WsView g_ws_view;
extern "C" const uint32_t *nm_palette(void);   /* machine.cpp: the output palette (ARGB) */

/* ---- the game's memory, as the 68000 sees it --------------------------------------------- */

static uint8_t m8(uint32_t a)
{
    a &= 0xFFFFFFu;
    if (a < 0x400000u) return g_rom[a];
    if (a >= 0xFF0000u) return nm_ram_read8(a);
    return 0;
}
static uint16_t m16(uint32_t a) { return (uint16_t)(m8(a) << 8 | m8(a + 1)); }
static uint32_t m32(uint32_t a) { return (uint32_t)m16(a) << 16 | m16(a + 2); }
static int16_t s16at(uint32_t a) { return (int16_t)m16(a); }

static GVDP *vdp(void) { return &g_machine.vdp; }
static uint16_t vram16(uint32_t a) { a &= 0xFFFEu; return (uint16_t)(vdp()->vram[a] << 8 | vdp()->vram[a + 1]); }

/* RAM */
#define RAM_SECTION   0xFFF602u   /* g_SectionId */
#define RAM_CAMERA_X  0xFFDFC4u   /* g_CameraX (16.16, high word) */
#define RAM_LOCK_X    0xFFDFC0u   /* g_CameraLockX */
#define RAM_VIEW_X    0xFFDFF0u   /* g_ViewX */
#define RAM_LAYER_A   0xFFED9Cu   /* g_LayerA (MapLayer) */
#define RAM_SCROLL_MODE 0xFFE55Au /* g_ScrollMode */

/* ---- what the branch snapshot carries ------------------------------------------------------ */

/* the view's memory of the stage 1 street's signs (street_now): the segment and the drawn states
   last frame, and the segment before's (its drawn states when the segment grew) */
struct StreetSaved { int16_t segment; uint8_t drawn[4], before[4]; uint8_t known; uint8_t fire_known; int16_t fire_xoff; };

/* R20's measurement (BR_WS_PLAY_LOG): an object the game placed relative to the screen, followed
   until it goes (its block's serial changes or it leaves the object list) */
#define WS_TRACK_MAX 48
struct WsTracked {
    uint32_t obj, what, handler;  /* the object (a6), its spawner / script, its handler when placed */
    uint16_t serial, section;
    int16_t sx0, moved, minx, maxx, lastx;
    uint8_t kind, ever_in, locked0, pad;
    uint32_t frames, in_view, off_run, off_max, born, first_in;
};

struct WsSaved {
    uint32_t scene_header;        /* the header the running scene started from (ws_scene_begin) */
    StreetSaved street;
    WsTracked tracked[WS_TRACK_MAX];
    uint32_t ntracked;
    uint32_t lock_run;            /* drawn gameplay frames the camera has been locked (play_frame) */
};
static WsSaved s_saved;
static uint64_t s_ext_sync = ~0ull;   /* view_extent's cache (R20): the sync interval it holds */

extern "C" size_t ws_saved_size(void) { return sizeof s_saved; }
extern "C" void ws_saved_get(void *dst) { memcpy(dst, &s_saved, sizeof s_saved); }
extern "C" void ws_saved_put(const void *src)
{
    memcpy(&s_saved, src, sizeof s_saved);
    s_ext_sync = ~0ull;   /* a restored branch computes its own */
}

extern "C" void ws_scene_begin(uint32_t header) { s_saved.scene_header = header & 0xFFFFFFu; }

/* ---- diagnostics: BR_WS_DUMP=file writes one row per drawn frame (the measurements) ---------- */

static FILE *s_dump;
static uint64_t s_frame;

static void dump_frame(void)
{
    static int opened;
    if (!opened) {
        opened = 1;
        const char *p = getenv("BR_WS_DUMP");
        if (p && *p) {   /* one file per process (a corpus pass runs several) */
            static int seq;
            char path[600];
            snprintf(path, sizeof path, "%s.%d.%d", p, (int)ws_getpid(), seq++);
            s_dump = fopen(path, "w");
        }
        if (s_dump)
            fprintf(s_dump, "frame,section,scene,gameplay,cam_x,lock_x,view_x,lx,ly,bcol,brow,col,row,width,height,"
                            "plane,cells_x,cells_y,blocks_x,r11,r16,r2,r4,r13,hsA,hsB,vsA,vsB,mode,sat\n");
    }
    if (!s_dump) return;
    static unsigned every;
    if (!every) { const char *e = getenv("BR_WS_DUMP_EVERY"); every = e && *e ? (unsigned)atoi(e) : 1; if (!every) every = 1; }
    if (s_frame % every) return;
    const GVDP *v = vdp();
    uint32_t L = RAM_LAYER_A;
    uint32_t hb = (uint32_t)(v->reg[13] & 0x3F) << 10;
    fprintf(s_dump, "%llu,%04X,%06X,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%04X,%d,%d,%d,%02X,%02X,%02X,%02X,%02X,%d,%d,%d,%d,%d,",
            (unsigned long long)s_frame, m16(RAM_SECTION), s_saved.scene_header, g_ws_view.gameplay,
            s16at(RAM_CAMERA_X), s16at(RAM_LOCK_X), s16at(RAM_VIEW_X),
            s16at(L), s16at(L + 2), s16at(L + 0x26), s16at(L + 0x28), s16at(L + 0x32), s16at(L + 0x34),
            s16at(L + 0x2E), s16at(L + 0x30), m16(L + 0x24), s16at(L + 0x3A), s16at(L + 0x3C), s16at(L + 0x3E),
            v->reg[11], v->reg[16], v->reg[2], v->reg[4], v->reg[13],
            (int16_t)(vram16(hb) << 6) >> 6, (int16_t)(vram16(hb + 2) << 6) >> 6, v->vsram[0] & 0x7FF, v->vsram[1] & 0x7FF,
            s16at(RAM_SCROLL_MODE));
    /* the sprite table as the VDP walks it: index:x:y:size:attr */
    uint32_t sat = (uint32_t)(v->reg[5] & 0x7E) << 9;
    int link = 0;
    for (int n = 0; n < 80; n++) {
        uint32_t e = sat + (uint32_t)link * 8u;
        fprintf(s_dump, "%s%d:%d:%d:%X:%04X", n ? " " : "", link, (vram16(e + 6) & 0x1FF) - 128, (vram16(e) & 0x1FF) - 128,
                v->vram[(e + 2) & 0xFFFF] & 0xF, vram16(e + 4));
        link = v->vram[(e + 3) & 0xFFFF] & 0x7F;
        if (!link || link >= 80) break;
    }
    fputc('\n', s_dump);
}

/* ---- screens (R5) ------------------------------------------------------------------------- */

/* The plain scenes by the header Scene_InitCommon read (docs/NOTES.md "Phase 6: widescreen"; each
   one's mode and reason in recomp/game/widescreen_screens.toml). Every stage section (g_SectionId
   $1100-$4FFF, started from its stage header) is wide. */
struct PlainScene { uint32_t header; const char *id; int wide; };
static const PlainScene k_plain[] = {
    { 0x049CCA, "sega-logo", 0 },        { 0x049CEE, "sega-logo-palette", 0 }, { 0x049CDC, "copyright", 0 },
    { 0x049674, "title", 0 },            { 0x049A56, "player-select", 0 },     { 0x04A050, "batmobile-intro", 0 },
    { 0x04A062, "stage-card-1", 0 },     { 0x04A074, "stage-card-2", 0 },      { 0x04A098, "stage-card-4", 0 },
    { 0x047CD0, "story-asylum-gate", 0 },{ 0x047CE2, "story-gate-scroll", 0 }, { 0x047CF4, "story-searchlight", 0 },
    { 0x047D06, "story-tower", 0 },      { 0x047D18, "story-facade", 0 },      { 0x0489E2, "attract-skyline", 0 },
    { 0x0489F4, "attract-scroll", 0 }, { 0x048EDE, "continue", 0 }, { 0x048EF0, "game-over", 1 },
    { 0x04A52A, "ending-epilogue", 0 },  { 0x04A53C, "ending-freeze-cell", 0 },
};

static char s_screen_id[24];
static int s_screen_wide;

static const int s_allwide = getenv("BR_WS_ALLWIDE") && *getenv("BR_WS_ALLWIDE") == '1';   /* diagnosing: every scene wide */

static void screen_now(void)
{
    uint32_t h = s_saved.scene_header;
    uint16_t sec = m16(RAM_SECTION);
    if (!h) { snprintf(s_screen_id, sizeof s_screen_id, "boot"); s_screen_wide = 0; return; }
    for (const PlainScene &p : k_plain)
        if (p.header == h) { snprintf(s_screen_id, sizeof s_screen_id, "%s", p.id); s_screen_wide = p.wide || s_allwide; return; }
    if (sec >= 0x1100u && sec < 0x5000u) {
        snprintf(s_screen_id, sizeof s_screen_id, "section-%04X", sec);
        s_screen_wide = 1;
        return;
    }
    snprintf(s_screen_id, sizeof s_screen_id, "scene-%06X", h);   /* not listed: 4:3, and E3 names it */
    s_screen_wide = 0;
}

static int is_gameplay(void)
{
    uint16_t s = m16(RAM_SECTION);
    return s >= 0x1100u && s < 0x5000u && !m8(0xFFF703) && !m8(0xFFF702);   /* enh_gameplay */
}

/* ---- the planes as the VDP shows them ------------------------------------------------------ */

struct Planes {
    int wcells, hcells;            /* plane size in cells */
    uint32_t base[2];              /* name tables: plane A, plane B */
    uint32_t hbase;                /* H-scroll table */
    int hmode;                     /* reg 11 bits 0-1: 0 full, 2 cell, 3 line */
    int vcol;                      /* reg 11 bit 2: 2-cell column V-scroll */
};

static Planes planes_now(void)
{
    const uint8_t *r = vdp()->reg;
    static const int dim[4] = { 32, 64, 32, 128 };
    Planes p;
    p.wcells = dim[r[16] & 3];
    p.hcells = dim[(r[16] >> 4) & 3];
    p.base[0] = (uint32_t)(r[2] & 0x38) << 10;
    p.base[1] = (uint32_t)(r[4] & 0x07) << 13;
    p.hbase = (uint32_t)(r[13] & 0x3F) << 10;
    p.hmode = r[11] & 3;
    p.vcol = (r[11] & 4) != 0;
    return p;
}

/* the H-scroll word of plane (0 A, 1 B) at line y, as the game left it (before the render camera) */
static uint8_t s_hs_orig[0x400];
static int s_hs_saved;
static int hscroll_at(const Planes &p, int plane, int y)
{
    int idx = p.hmode == 0 ? 0 : p.hmode == 2 ? (y & ~7) : y;
    uint32_t off = (uint32_t)idx * 4u + (plane ? 2u : 0u);
    uint16_t w = s_hs_saved ? (uint16_t)(s_hs_orig[off] << 8 | s_hs_orig[off + 1]) : vram16(p.hbase + off);
    return w & 0x3FF;
}

/* ---- R12: the margin columns of 2-cell V-scroll -------------------------------------------- */

static int s_col_set[GVDP_WS_VCOL_LAST - GVDP_WS_VCOL_FIRST + 1];
static uint16_t s_col_val[GVDP_WS_VCOL_LAST - GVDP_WS_VCOL_FIRST + 1][2];
static int s_vs_logic;   /* the game's decisions (R20, view_extent): the margin columns at the clamp, not the
                            last drawn line's fill (host state a restored branch would not have) */

/* high_fps's capture hook (widescreen.h) */
extern "C" int ws_margin_columns(const int **set, const uint16_t (**val)[2])
{
    *set = s_col_set;
    *val = s_col_val;
    return g_opt.widescreen && g_ws_view.margin ? (int)(sizeof s_col_set / sizeof s_col_set[0]) : 0;
}

static int vscroll_at(const Planes &p, int plane, int sx)
{
    const GVDP *v = vdp();
    if (!p.vcol) return v->vsram[plane] & 0x7FF;
    int col = sx < 0 ? -((-sx + 15) / 16) : sx / 16;
    if (col >= 0 && col < 20) return v->vsram[col * 2 + plane] & 0x7FF;
    int k = col - GVDP_WS_VCOL_FIRST;
    if (!s_vs_logic && k >= 0 && k < (int)(sizeof s_col_set / sizeof s_col_set[0]) && s_col_set[k])
        return s_col_val[k][plane] & 0x7FF;
    return v->vsram[(col < 0 ? 0 : 19) * 2 + plane] & 0x7FF;   /* the clamp */
}

/* the name-table word plane `plane` shows at screen pixel (x, y) of the 4:3 view's coordinates */
static uint16_t cell_at(const Planes &p, int plane, int x, int y)
{
    int px = (x - hscroll_at(p, plane, y)) & (p.wcells * 8 - 1);
    int py = (y + vscroll_at(p, plane, x)) & (p.hcells * 8 - 1);
    return vram16(p.base[plane] + (uint32_t)((py >> 3) * p.wcells + (px >> 3)) * 2u);
}

/* ---- the street (section $1100, ws-street 2026-10-30) ------------------------------------------
   Stage 1's street draws its facade once into plane B (128 x 32 cells, $E000) from the map
   buffer g_BufPtrs[1]: picture columns 0..83 (672 px), then the bank-sign panels' source cells
   (columns 84..107: every map offset in Tbl_015CF4 lands at column 84 or 96), then the clear tile
   ($05FC, columns 108..127). The street never scrolls the plane through the map: the facade
   repeats every $168-px segment of facade x (g_ViewX x 35/32), and Stage1_ScrollFacadeLines gives
   each plane B line the scroll of the segment offset g_Stage1SegOfs (-$B4..+$B4 px) through five
   bands of shear, so each line has its own period P = 288..360 px of plane x (the bands' depths).
   At a segment change the offset jumps by $168 and every line by its P, and the picture goes on.
   Over the offset's range a line's 4:3 view stays inside [L(+$B4), L(-$B4) + 320) of the plane,
   so the 40-px margins reach past the facade there: on the right the panel source cells (the
   blasted-sign art), on the left the clear tile (backdrop, and content_bars barred it black). Seen
   by the user (16:9, stage 1) and measured on the movie: a margin past the facade on 11 of 199
   sampled street frames (every 30th of its first 6,400), each time the offset neared a segment's end.
   The view: a margin pixel whose plane x leaves the line's 4:3 range is drawn from the next
   segment's parametrization on the right (the line scroll the game would give at offset + $168)
   or the previous one's on the left (offset - $168), as the game draws the same facade point one
   segment on or back. A sign panel a folded pixel lands on is drawn (from its source cells, as
   Stage1_DrawSignPanel) in the state of the sign it stands for: one segment on, slots 0-1 stand
   for 2-3 (their drawn states) and 2-3 for the segment after next (intact: the game clears 2-3
   when the segment grows); one segment back, 2-3 stand for 0-1 and 0-1 for the segment before,
   whose drawn states the view keeps from before the change (WsSaved). The pixel is
   drawn through the host sprite layer (ws_draw's draw_line calls ws_street_line), composited with
   plane A and the backdrop as the VDP would (priority, shadow/highlight) and written only where
   no sprite drew; the game's VRAM is never touched. */

#define RAM_VIEW_Y      0xFFDFF4u  /* g_ViewY (16.16, high word) */
#define RAM_SEG_OFS     0xFFF50Au  /* g_Stage1SegOfs (16.16) */
#define RAM_SEGMENT     0xFFF50Eu  /* g_Stage1Segment */
#define RAM_SIGN_DRAWN  0xFFF4FEu  /* Stage1StreetVars.sign_drawn[4] (g_SceneVars + $A) */
#define RAM_SIGN_MAP    0xFFF502u  /* Stage1StreetVars.map_buffer (g_SceneVars + $E) */
#define TBL_SIGN_SLOTS  0x015CF4u  /* Tbl_015CF4: 4 slots x 2 panels x (VDP command, map offset) */
#define STREET_SEGMENT  0x1680000  /* $168 px, 16.16 */
#define STREET_OFS_MAX  0x00B40000 /* the offset's range (-$B4, +$B4] px (Stage1_UpdateStreetScroll) */
#define STREET_COLS     84         /* the facade picture's columns in plane B (the panels' sources at 84, 96) */
#define SIGN_W 12
#define SIGN_H 7

/* Stage1_ScrollFacadeLines' plane B line words for offset `ofs` and view y `vy` (the game's
   arithmetic: divs.w, the 16.16 ramps, muls.w on band 1's step), lines 0..223 */
static uint32_t st_divs_w(uint32_t d, int16_t s)
{
    int32_t q = (int32_t)d / s, r = (int32_t)d % s;
    if (q < -32768 || q > 32767) return d;
    return ((uint32_t)(uint16_t)r << 16) | (uint16_t)q;
}
static uint32_t st_slope(uint32_t ofs, int16_t div) { return (uint32_t)(int32_t)(int16_t)st_divs_w(ofs, div) << 4; }

static bool facade_lines(uint32_t ofs, int vy, int16_t out[224])
{
    if (vy < 0 || vy > 0x58) return false;
    int n = 0;
    auto ramp = [&](uint32_t &d0, uint32_t d1, int count) {   /* Mem_FillRamp: the word, then the step */
        for (int i = 0; i < count; i++, n++) {
            if (n < 224) out[n] = (int16_t)(d0 >> 16);
            d0 += d1;
        }
    };
    uint32_t d1 = 0u - st_slope(ofs, 0x3720);
    uint32_t d0 = ofs + 0xFF508000u;
    for (int i = 0; i < vy; i++) d0 += d1;   /* band 1 starts the view y's lines above the screen */
    ramp(d0, d1, 0x58 - vy);
    uint32_t d3 = d0, d4 = d1;
    d1 = st_slope(ofs, 0x14A0);
    ramp(d0, d1, 0x10);
    d0 -= d1;
    d1 = 0u - st_slope(ofs, 0x1C20);
    ramp(d0, d1, 8);
    d0 = d3 + (uint32_t)((int32_t)(int16_t)d4 * 0x18);   /* muls.w: band 1's step's low word x $18 */
    d1 = d4;
    ramp(d0, d1, 0x40);
    d1 = st_slope(ofs, 0x1950);
    ramp(d0, d1, 0x30 + vy);
    return n == 224;
}

struct Street {
    int valid;                     /* this frame is the street's, with a margin (the model holds) */
    int on;                        /* the view folds its margins (not under BR_WS_PLANT_STREET=1) */
    int sh;                        /* shadow/highlight mode (reg 12 bit 3) */
    int16_t lo[224], hi[224];      /* each line's 4:3 range of plane x [lo, hi), inside the facade */
    int16_t dl[224], dr[224];      /* plane x moves: the previous segment's (left), the next's (right) */
    int16_t game[224];             /* the replica's words at the offset (the game's table must equal them) */
    int slot_col[4], slot_row[4];  /* each sign slot's panel in plane B (cells) */
    uint32_t slot_src[4][2];       /* its panels' source cells (map buffer + offset): intact, blasted */
    uint8_t left_state[4], right_state[4];   /* the states a folded pixel shows in each slot */
};
static Street s_st;
/* the controls: BR_WS_PLANT_STREET=1 the view as before (no fold), =n > 1 the fold n px off its
   period (E3's street check must fail on both) */
static const int s_plant_street = getenv("BR_WS_PLANT_STREET") ? atoi(getenv("BR_WS_PLANT_STREET")) : 0;

static int street_now(const Planes &p, int m, StreetSaved *mem)
{
    s_st.on = s_st.valid = 0;
    if (!m || m16(RAM_SECTION) != 0x1100u || s16at(RAM_SCROLL_MODE) != 0x0C) { mem->known = 0; return 0; }
    if (p.wcells != 128 || p.hcells != 32 || p.hmode != 3 || p.vcol) return 0;
    uint32_t ofs = m32(RAM_SEG_OFS);
    int vy = (int16_t)m16(RAM_VIEW_Y);
    int16_t at[224], next[224], prev[224], first[224], last[224];
    if (!facade_lines(ofs, vy, at) || !facade_lines(ofs + STREET_SEGMENT, vy, next) ||
        !facade_lines(ofs - STREET_SEGMENT, vy, prev) || !facade_lines(STREET_OFS_MAX, vy, first) ||
        !facade_lines(0u - STREET_OFS_MAX + 1u, vy, last))
        return 0;
    for (int y = 0; y < 224; y++) {
        int lo = -first[y], hi = -last[y] + 320;
        s_st.lo[(size_t)y] = (int16_t)(lo < 0 ? 0 : lo);
        s_st.hi[(size_t)y] = (int16_t)(hi > STREET_COLS * 8 ? STREET_COLS * 8 : hi);
        s_st.dl[(size_t)y] = (int16_t)(at[y] - prev[y]);
        s_st.dr[(size_t)y] = (int16_t)(at[y] - next[y]);
        s_st.game[(size_t)y] = at[y];
    }
    uint32_t map = m32(RAM_SIGN_MAP) & 0xFFFFFFu;
    for (int k = 0; k < 4; k++) {
        uint32_t rec = TBL_SIGN_SLOTS + 16u * (uint32_t)k;
        uint32_t cmd = m32(rec);
        uint32_t a = ((cmd >> 16) & 0x3FFFu) | ((cmd & 3u) << 14);
        int cell = (int)((a - p.base[1]) & 0xFFFFu) / 2;
        s_st.slot_col[k] = cell % p.wcells;
        s_st.slot_row[k] = cell / p.wcells;
        s_st.slot_src[k][0] = map + m32(rec + 4u);
        s_st.slot_src[k][1] = map + m32(rec + 12u);
    }
    /* the signs: slots 0-1 are this segment's, 2-3 the next's. When the segment grows the game moves
       2-3's into 0-1 and clears 2-3; the segment before's drawn states are the view's to keep. */
    int16_t seg = (int16_t)m16(RAM_SEGMENT);
    uint8_t drawn[4];
    for (int k = 0; k < 4; k++) drawn[k] = m8(RAM_SIGN_DRAWN + (uint32_t)k);
    if (!mem->known) { memset(mem->before, 0, sizeof mem->before); mem->known = 1; }
    else if (seg == mem->segment + 1) memcpy(mem->before, mem->drawn, sizeof mem->before);
    else if (seg != mem->segment) memset(mem->before, 0, sizeof mem->before);
    mem->segment = seg;
    memcpy(mem->drawn, drawn, sizeof drawn);
    for (int k = 0; k < 4; k++) {
        s_st.left_state[k] = k < 2 ? mem->before[k] : drawn[k - 2];   /* one segment back: 0-1 were 2-3 */
        s_st.right_state[k] = k < 2 ? drawn[k + 2] : 0;              /* one segment on: 2-3 cleared */
    }
    s_st.sh = (vdp()->reg[12] & 0x08) != 0;
    s_st.valid = 1;
    s_st.on = s_plant_street != 1;
    return 1;
}

/* ---- the street's building fire (plane A, ws-balloon 2026-10-31) ------------------------------
   The bank bombing sets a fire in an upper-floor window (Stage1_StartFireAt): Stage1_VIntFireStep
   blanks plane A's top rows and animates the fire there in two copies, plane columns 0..13 and
   64..77 (14 cells, 112 px, 512 px apart), writing one copy while the other is shown (the entries'
   x offset, g_SceneVars+4, is 0 or 512: the shown copy always sits at the fire's place, the other
   512 px from it). Stage1_SwayFireLines gives plane A's lines 0..$4F - g_ViewY the fire's place
   (fire x - facade x, x ~32/35, rounded, + the offset) leaning by a sine per line, and stops
   writing them once the fire is $A0 facade px (~146 px) left of the screen: the 4:3 view no longer
   shows it. The lines keep their last words (the Harley mid-boss then stops the street's line
   effects: g_Stage1FireState $FF), and the animation goes on alternating the copies under them.
   In 4:3 that is off screen; the wide view's left margin shows the fire's right end (up to ~40 px
   at the top lines, with the lean) frozen there for the rest of the street, garbled by the copies'
   alternation: the user's "visual issue on the top left" (the Harley fight, BOSS TIME).
   The view: past the game's cut, the fire's lines take the words SwayFireLines would write (the
   same arithmetic, the phase as it stands, the drawn lines' offset), so the fire goes on leaving through the
   margin with the facade, until the shown copy's right end is past the wide view's left edge
   (x <= -m - 112): there it stays, and the other copy, 512 px on, is just past the right edge (the
   copies' gap is 400 px, the wide view's width). Only while the frame is drawn (journaled VRAM
   words, as the render camera's): the game's RAM, VRAM after the frame and timing are the 4:3
   run's, and the 4:3 columns show plane A's clear tiles at either word (rows 0..9 hold only the
   copies). Control BR_WS_PLANT_FIRE=1: the view as before (the frozen words). */
static void pw16(uint32_t a, uint16_t w);   /* the journaled VRAM write (below) */

#define RAM_FIRE_STATE  0xFFF518u  /* g_Stage1FireState: 1 burning, $FF stopped (the Harley fight) */
#define RAM_FIRE_X      0xFFF512u  /* g_Stage1FireX (16.16 facade x) */
#define RAM_FIRE_PHASE  0xFFF510u  /* g_Stage1FirePhase */
#define RAM_FIRE_XOFF   0xFFF4F8u  /* g_SceneVars + 4: the shown copy's x offset (0 / 512) */
#define RAM_FACADE_X    0xFFF506u  /* g_SceneVars + $12: the facade x (16.16) */
#define TBL_FIRE_SINE   0x059B22u  /* Tbl_059B22: the sine words of the sway */
#define FIRE_BAND       0x50       /* the fire's lines: 0 .. $4F - g_ViewY */
#define FIRE_W          112        /* a copy's 14 columns */
#define FIRE_CUT        ((int32_t)0xFF600000)   /* SwayFireLines writes while fire x - facade x > -$A0 */

struct Fire {
    int valid;                     /* this frame is the street's, wide, with the fire burning or stopped */
    int n;                         /* its lines */
    int cut;                       /* past the game's cut: the view's words, not the game's */
    int16_t want[FIRE_BAND];       /* each line's plane A word: the game's arithmetic (clamped past the cut) */
};
static Fire s_fire;
static const int s_plant_fire = getenv("BR_WS_PLANT_FIRE") ? atoi(getenv("BR_WS_PLANT_FIRE")) : 0;

/* Stage1_SwayFireLines' words for fire x - facade x `d0` (16.16), lines 0..n-1 */
static void fire_lines(uint32_t d0, int xoff, uint16_t phase, int n, int16_t *out)
{
    int16_t q = (int16_t)st_divs_w(d0, 0x3720);           /* divs.w #$3720 (unchanged on overflow), ext.l */
    d0 -= (uint32_t)((int32_t)q * 0x500);                 /* muls.w #$500: x 32/35 */
    uint32_t d1 = (uint32_t)((int32_t)(int16_t)m16(TBL_FIRE_SINE + (phase & 0x3FEu)) - 0x2000);
    d0 += 0x8000u;
    d0 = ((uint32_t)(uint16_t)((d0 >> 16) + (uint32_t)xoff) << 16) | (d0 & 0xFFFFu);   /* swap, add.w, swap */
    for (int y = n - 1; y >= 0; y--) {                   /* from the bottom line up */
        out[y] = (int16_t)(d0 >> 16);
        d0 += d1;
    }
}

static void fire_now(const Planes &p, int m)
{
    s_fire.valid = 0;
    StreetSaved &sv = s_saved.street;
    if (!s_st.valid || p.hmode != 3) { sv.fire_known = 0; return; }
    /* The offset the drawn lines were written with: the V-int DMAs the lines the last logic frame
       wrote, then Stage1_VIntFacade steps the fire (the next entry's offset), so by this frame's setup
       RAM holds the offset of the lines to come; the drawn ones took the one RAM held at the last
       drawn frame's setup (measured on the movie's street: 2,078 frames before the cut, every one
       matched by it, 519 of them not by RAM's) */
    int xoff_now = (int16_t)m16(RAM_FIRE_XOFF);
    int xoff = sv.fire_known ? sv.fire_xoff : xoff_now;
    sv.fire_xoff = (int16_t)xoff_now;
    sv.fire_known = 1;
    uint8_t st = m8(RAM_FIRE_STATE);
    if (st != 1 && st != 0xFF) return;
    int n = FIRE_BAND - (int16_t)m16(RAM_VIEW_Y);
    if (n <= 0 || n > FIRE_BAND) return;
    uint32_t d0 = m32(RAM_FIRE_X) - m32(RAM_FACADE_X);
    /* the phase as it stands: SwayFireLines steps it before it writes, and not at all past the cut */
    fire_lines(d0, xoff, m16(RAM_FIRE_PHASE), n, s_fire.want);
    s_fire.n = n;
    s_fire.cut = (int32_t)d0 <= FIRE_CUT;
    s_fire.valid = 1;
    if (!s_fire.cut) return;   /* the game writes them: the check holds the replica to its words */
    for (int y = 0; y < n; y++) {
        int base = (int16_t)(uint16_t)(s_fire.want[y] - xoff);   /* the shown copy's screen x */
        if (base < -m - FIRE_W) base = -m - FIRE_W;
        s_fire.want[y] = (int16_t)(base + xoff);
        if (s_plant_fire == 1) continue;
        uint32_t a = p.hbase + (uint32_t)y * 4u;
        pw16(a, (uint16_t)((vram16(a) & 0xFC00u) | ((uint16_t)s_fire.want[y] & 0x3FFu)));
    }
}

struct PlanePx { int idx, op, hi; };

static PlanePx pattern_px(const GVDP *v, uint16_t word, int fx, int fy)
{
    PlanePx r;
    if (word & 0x0800) fx = 7 - fx;
    if (word & 0x1000) fy = 7 - fy;
    uint8_t b = v->vram[(uint16_t)((word & 0x7FF) * 32 + fy * 4 + (fx >> 1))];
    int nib = (fx & 1) ? (b & 0x0F) : (b >> 4);
    r.idx = ((word >> 13) & 3) * 16 + nib;
    r.op = nib != 0;
    r.hi = (word & 0x8000) != 0;
    return r;
}

/* the plane x (unwrapped: the facade's own coordinates) a line's scroll word puts at screen x */
static int street_x(int hs, int x)
{
    int L = (-hs) & 0x3FF;
    if (L >= 768) L -= 1024;
    return L + x;
}

/* plane B as the wide view shows it at screen (x, y): the plane's cell, or the folded one; *fold
   -1 left, 1 right, 0 none. The line's scroll word `hs` (the game's, or high_fps's interpolated). */
static uint16_t street_bword(const GVDP *v, uint32_t base_b, int hs, int vs, int x, int y, int *fold, int *px, int *py,
                             int skew = 0)
{
    int X = street_x(hs, x);
    *fold = 0;
    if (y >= 0 && y < 224) {
        if (X < s_st.lo[y]) { X += s_st.dl[y] + skew; *fold = -1; }
        else if (X >= s_st.hi[y]) { X += s_st.dr[y] - skew; *fold = 1; }
    }
    int PX = X & 1023, PY = (y + vs) & 255;
    *px = PX;
    *py = PY;
    int cx = PX >> 3, cy = PY >> 3;
    if (*fold) {
        for (int k = 0; k < 4; k++) {
            int dx = cx - s_st.slot_col[k], dy = cy - s_st.slot_row[k];
            if (dx < 0 || dx >= SIGN_W || dy < 0 || dy >= SIGN_H) continue;
            int state = *fold < 0 ? s_st.left_state[k] : s_st.right_state[k];
            /* Stage1_DrawSignPanel: the source long of two cells + $23122312 */
            uint32_t at = s_st.slot_src[k][(state >> 3) & 1] + (uint32_t)dy * 0xD8u + (uint32_t)(dx & ~1) * 2u;
            uint32_t cells = m32(at) + 0x23122312u;
            return (dx & 1) ? (uint16_t)cells : (uint16_t)(cells >> 16);
        }
    }
    return (uint16_t)(v->vram[(uint16_t)(base_b + (uint32_t)(cy * 128 + cx) * 2u)] << 8 |
                      v->vram[(uint16_t)(base_b + (uint32_t)(cy * 128 + cx) * 2u + 1u)]);
}

/* What the street's composite (no sprites) is at screen (x, y): the plane winner's CRAM index and
   whether a high-priority plane pixel is there (shadow/highlight: none = shadowed) */
struct StreetPx { int idx, hi_present, fold; PlanePx b_engine; };

static StreetPx street_px(const GVDP *v, int hs_a, int hs_b, int x, int y, int skew = 0)
{
    StreetPx r;
    uint32_t base_a = (uint32_t)(v->reg[2] & 0x38) << 10, base_b = (uint32_t)(v->reg[4] & 0x07) << 13;
    int vs_a = v->vsram[0] & 0x7FF, vs_b = v->vsram[1] & 0x7FF;
    int ax = (x - hs_a) & 1023, ay = (y + vs_a) & 255;
    uint16_t aw = (uint16_t)(v->vram[(uint16_t)(base_a + (uint32_t)((ay >> 3) * 128 + (ax >> 3)) * 2u)] << 8 |
                             v->vram[(uint16_t)(base_a + (uint32_t)((ay >> 3) * 128 + (ax >> 3)) * 2u + 1u)]);
    PlanePx a = pattern_px(v, aw, ax & 7, ay & 7);
    int px, py;
    uint16_t bw = street_bword(v, base_b, hs_b, vs_b, x, y, &r.fold, &px, &py, skew);
    PlanePx b = pattern_px(v, bw, px & 7, py & 7);
    int ex = (x - hs_b) & 1023, ey = (y + vs_b) & 255;   /* the plane's own pixel (what the VDP draws there) */
    uint16_t ew = (uint16_t)(v->vram[(uint16_t)(base_b + (uint32_t)((ey >> 3) * 128 + (ex >> 3)) * 2u)] << 8 |
                             v->vram[(uint16_t)(base_b + (uint32_t)((ey >> 3) * 128 + (ex >> 3)) * 2u + 1u)]);
    r.b_engine = pattern_px(v, ew, ex & 7, ey & 7);
    if (a.op && a.hi) r.idx = a.idx;
    else if (b.op && b.hi) r.idx = b.idx;
    else if (a.op) r.idx = a.idx;
    else if (b.op) r.idx = b.idx;
    else r.idx = v->reg[7] & 0x3F;
    r.hi_present = (a.op && a.hi) || (b.op && b.hi);
    return r;
}

/* the host sprite layer's line (ws_draw's draw_line, after the margin pieces): the folded margin
   pixels where no sprite drew. Also records, on the machine's own frames, which margin pixels a
   sprite drew (the check leaves them). */
static uint8_t s_st_sprite[224][GVDP_MAX_WIDTH];
static uint64_t s_st_painted;

extern "C" void ws_street_line(const GVDP *v, const GVDPSpriteLayer *layer)
{
    int line = layer->line;
    if (!g_opt.widescreen || !g_ws_view.margin || !s_st.valid || line < 0 || line >= 224) return;
    int c0 = layer->offset - g_ws_view.shift, c1 = c0 + 320;
    if (v == vdp())
        for (int ox = 0; ox < layer->total && ox < GVDP_MAX_WIDTH; ox++) s_st_sprite[line][ox] = layer->opaque[ox];
    if (!s_st.on) return;
    uint32_t hb = (uint32_t)(v->reg[13] & 0x3F) << 10;
    int hs_a = (v->vram[(uint16_t)(hb + line * 4)] << 8 | v->vram[(uint16_t)(hb + line * 4 + 1)]) & 0x3FF;
    int hs_b = (v->vram[(uint16_t)(hb + line * 4 + 2)] << 8 | v->vram[(uint16_t)(hb + line * 4 + 3)]) & 0x3FF;
    for (int ox = 0; ox < layer->total; ox++) {
        if (ox == c0) { ox = c1 - 1; continue; }   /* the 4:3 view's columns: never drawn */
        if (layer->opaque[ox]) continue;
        int x = ox - layer->offset;
        StreetPx s = street_px(v, hs_a, hs_b, x, line, s_plant_street > 1 ? s_plant_street : 0);   /* the control: off its period */
        if (!s.fold) continue;
        /* a sprite pixel in front of every plane wherever a high-priority plane pixel is there (the
           VDP then draws it unshadowed, operators applied); else a low one, behind the high planes
           and shadowed with the line, unless the plane's own (unfolded) pixel is a high one: then
           the colour as drawn (shadowed by hand) */
        int idx = s.idx, high = s.hi_present;
        if (!s.hi_present && s.b_engine.op && s.b_engine.hi) { high = 1; if (s_st.sh) idx += GVDP_PALETTE_SHADOW; }
        layer->index[ox] = (uint8_t)idx;
        layer->opaque[ox] = 1;
        layer->high[ox] = (uint8_t)high;
        s_st_painted++;
    }
}

/* Plane B's word at screen (x, y) as the wide view shows it (pixel_drawn, check_whole) */
static uint16_t cell_at(const Planes &p, int plane, int x, int y);

static int pixel_drawn(const Planes &p, int x, int y)
{
    if (s_st.on && (x < 0 || x >= 320)) {
        int fold, px, py;
        uint16_t bw = street_bword(vdp(), p.base[1], hscroll_at(p, 1, y), vdp()->vsram[1] & 0x7FF, x, y, &fold, &px, &py);
        return cell_at(p, 0, x, y) != 0x05FC || bw != 0x05FC;
    }
    return cell_at(p, 0, x, y) != 0x05FC || cell_at(p, 1, x, y) != 0x05FC;
}

/* ---- the render camera (R16) ------------------------------------------------------------------
   The game's camera only scrolls right and stops at arena locks with the map going on past them, so
   with the view presentation only the 400 px around it show the map past the arena's 4:3 edges. The
   window moves only where the 400 px would leave the map: rx = clamp(view - m, 0, mapW - 400), the
   map being the streamed layer's (g_LayerA: its x is the view's left on the map, its width in
   16-px blocks). The shift rx - (view - m) is at most m either way, so the 4:3 view stays in the
   frame (its x 0 at frame x m - shift: g_enh_centre_x0). A map narrower than 400 px is centred
   with bars past its ends, except with 2-cell column V-scroll (shift 0: shift_for). Sections without a streamed layer keep the window centred (shift 0). */

struct Layer {
    int valid;
    int plane;                     /* 0 A, 1 B */
    int x, y, block_col, block_row, col, row, width, height;
    uint32_t map, tiles, cell_add;
    uint16_t map_row_bytes;
};

static Layer layer_now(const Planes &p)
{
    Layer l = {};
    uint32_t L = RAM_LAYER_A;
    l.width = s16at(L + 0x2E);
    l.height = s16at(L + 0x30);
    l.map = m32(L + 0x0C) & 0xFFFFFFu;
    l.tiles = m32(L + 0x10) & 0xFFFFFFu;
    uint16_t pv = m16(L + 0x24);
    if (l.width <= 0 || l.height <= 0 || !l.map || !l.tiles) return l;
    if (pv == p.base[0]) l.plane = 0;
    else if (pv == p.base[1]) l.plane = 1;
    else return l;
    l.x = (int16_t)m16(L);
    l.y = (int16_t)m16(L + 2);
    l.block_col = s16at(L + 0x26);
    l.block_row = s16at(L + 0x28);
    l.col = m16(L + 0x32);
    l.row = m16(L + 0x34);
    l.cell_add = m32(L + 8);
    l.map_row_bytes = m16(L + 0x2A);
    /* the layer drives the plane only while the plane shows it: plane x = screen x + layer x; and
       not while the a6 redraw (VM6_StartRedraw, one column a frame) walks its block and cell
       columns along the screen (g_A6ResumeHook = VM6_RedrawStep $743A: the 4:3 view is being
       redrawn too; the columns go back at its end) */
    l.valid = s16at(L + 0x3A) == p.wcells && (m32(0xFFE03Au) & 0xFFFFFFu) != 0x00743Au;
    return l;
}

static int s_rx_l, s_rx_r;         /* the map's [L, R) in screen x of the 4:3 view, this frame */

static int shift_for(int m, const Planes &p, const Layer &l, int *bar_l, int *bar_r)
{
    *bar_l = *bar_r = 0;
    s_rx_l = -m; s_rx_r = 320 + m;
    if (!m || !l.valid) return 0;   /* both modes: the camera keeps the wide view inside the map (R4, R16) */
    int hs = hscroll_at(p, l.plane, 0);
    (void)hs;
    int L = -l.x, R = l.width * 16 - l.x;   /* the map in screen x */
    s_rx_l = L; s_rx_r = R;
    int w = 320 + 2 * m, base = -m, rx;
    if (R - L >= w) rx = base < L ? L : base > R - w ? R - w : base;
    else rx = (L + R) / 2 - w / 2;
    int s = rx - base;
    if (s < -m) s = -m;
    if (s > m) s = m;
    /* 2-cell column V-scroll (the lift $2100, the only streamed column scene): the VDP's columns
       are the screen's 16-px columns, so a window moved by the H-scroll words alone would draw each
       column's scroll `s` px off its pixels (the lift's towers torn at every column edge, and the
       4:3 view's centre unequal to the 4:3 frame once its columns scroll apart: ws-centre). The
       window stays where the 4:3 view is, the bars past the map's ends as before. */
    if (p.vcol) s = 0;
    int left = L - (base + s), right = (base + s + w) - R;
    *bar_l = left > 0 ? (left > m - s ? m - s : left) : 0;
    *bar_r = right > 0 ? (right > m + s ? m + s : right) : 0;
    return s;
}

/* Sections drawn whole (no streamed layer: arenas, the street, the flights): where the picture
   ends inside the margin, black bars past it. A pixel shows nothing when both planes show the clear
   tile $05FC (VDP_ClearPlanes, Op6_ClearPlane: never drawn); a margin pixel that shows nothing on a
   line whose 4:3 edge pixel shows something is past the picture's end: its column and every column
   outward are barred. Where the 4:3 edge shows nothing too (the backdrop goes on), nothing is barred. */
static int pixel_drawn(const Planes &p, int x, int y);

static void content_bars(const Planes &p, int m, int *bar_l, int *bar_r)
{
    *bar_l = *bar_r = 0;
    for (int y = 0; y < 224; y++) {
        if (pixel_drawn(p, 0, y))
            for (int x = -1; x >= -m + *bar_l; x--)
                if (!pixel_drawn(p, x, y)) { *bar_l = m + x + 1; break; }
        if (pixel_drawn(p, 319, y))
            for (int x = 320; x < 320 + m - *bar_r; x++)
                if (!pixel_drawn(p, x, y)) { *bar_r = 320 + m - x; break; }
    }
}

/* ---- the HUD (pinned to the screen's edges) --------------------------------------------------
   The players' panels (HUD_DrawPanelSprites: the piece tables Tbl_011974 player 1, left, and
   Tbl_0119AC player 2, right, whose tile words are the HUD's own) are moved to the wide view's
   edges while a frame is drawn: x - m on the left, + m on the right, whatever the render camera
   does. Every other sprite moves with the render camera. */
static uint16_t s_hud_attr[2][8];
static int s_nhud_attr[2];

static void hud_tables(void)
{
    static const uint32_t tbl[2] = { 0x011974u, 0x0119ACu };
    for (int side = 0; side < 2; side++) {
        s_nhud_attr[side] = 0;
        for (int i = 0; i < 7; i++) {
            uint16_t a = m16(tbl[side] + 8u * (uint32_t)i + 4u);
            bool dup = false;
            for (int k = 0; k < s_nhud_attr[side]; k++) dup |= s_hud_attr[side][k] == a;
            if (!dup) s_hud_attr[side][s_nhud_attr[side]++] = a;
        }
    }
}

#define COVER_TILE 0x5D8           /* Spr_MaskLeftColumn's solid tiles (g_SceneFixedSpr) */
#define RAM_FIXED_SPR 0xFFF4E8u

/* A shown table entry's HUD item: 0 not the HUD, 1 player 1's panel (left), 2 player 2's (right),
   3 the x 0-15 cover, WS_HUD_METER (4) the boss meter, WS_HUD_SIGN (5) the GO! sign (the two the
   game writes straight into its SAT buffer: tagged by their writers, ws_draw.h). */
static int hud_side_attr(uint16_t attr);

static int hud_side(unsigned entry, uint16_t attr)
{
    if (int kind = ws_hud_shown_kind(entry)) return kind;
    return hud_side_attr(attr);
}

/* by the attribute alone (the cover's tile, the panels' tile words): what a sprite of the host table
   is, which has no table entry for a writer to tag (only display-list pieces go there) */
static int hud_side_attr(uint16_t attr)
{
    if ((attr & 0x7FF) == COVER_TILE) return 3;
    for (int side = 0; side < 2; side++)
        for (int k = 0; k < s_nhud_attr[side]; k++)
            if (s_hud_attr[side][k] == attr) return side + 1;
    return 0;
}

/* Where the wide view pins each item, as the move from the game's x (VDP x) for margin m; every
   other sprite moves with the render camera (-shift). The panels go to their edges. The boss meter
   sits between the panels (its x is their home x's mean + $A2), so it stays at the screen's centre:
   pinned = not moved by the render camera. The GO! sign starts at screen x 360 (g_GoSignX $168: one
   margin past the 4:3 edge), brakes to a stop ~79 px inside the right edge, waits and leaves the
   way it came: its path is the right edge's, so it is pinned there as player 2's panel (x + m): it
   enters through the wide view's edge on the frame the 4:3 sign enters through the 4:3 edge, rests
   as far from the edge, and its pieces the game clips (x >= 320) stay past the wide edge (x >= 360),
   so Spr_EmitClipped's margin pieces are not drawn for it (007D76.c). */
static int hud_move(int side, int m)
{
    switch (side) {
    case 1: case 3: return -m;
    case 2: case WS_HUD_SIGN: return m;
    case WS_HUD_METER: return 0;
    default: return 0;
    }
}

/* A sprite's move for the frame (VDP x word `x`, HUD item `side`): the HUD pinned (hud_move), the
   cover to the wide view's left edge, the rest with the render camera; a sprite at the VDP's x 0
   stays (it masks the line: the hardware rule, kept). The VDP table's sprites (shift_vram) and the
   host table's (host_view) alike. */
static int sprite_move(int side, uint16_t x, int m, int shift)
{
    return side ? hud_move(side, m) : (x & 0x1FF) ? -shift : 0;
}


/* ---- what the frame's drawing moves in VRAM, put back after it --------------------------------- */

static uint16_t s_spr_at[80], s_spr_y[80], s_spr_x[80];
static int s_nspr, s_shifted;
static uint16_t s_spr_want[80];    /* the x the HUD check expects (VDP x) */
static int s_spr_side[80];

static void put8(uint16_t a, uint8_t b)
{
    GVDP *v = vdp();
    v->vram[a] = b;
    gvdp_sat_byte(v, a, b);
    hf_display_vram(a);   /* high_fps's capture hook (present.h) */
}

/* Every word the view writes for a frame (the margin columns, the shifted H-scroll and sprite x) is
   journaled with the game's value the first time, and put back at the frame's end byte by byte,
   except where the game itself wrote inside the window (an H-int's H-scroll table write, a DMA): the
   game's VRAM after the frame is the 4:3 run's, whatever the scene does next with those bytes (a name
   table moved, the area reused for patterns). Inside the window the game's reads and copies see its
   own bytes (ws_vram_game8). */
int g_ws_window;
static uint32_t s_jstamp[0x8000], s_stamp;   /* word address -> journaled this frame */
static uint16_t s_jidx[0x8000];
static uint16_t s_jaddr[0x8000], s_jorig[0x8000];
static uint8_t s_jtouch[0x8000];             /* bit 0 the high byte, bit 1 the low byte: the game wrote it */
static uint8_t s_jkind[0x8000];              /* 1 a margin cell the streamer wrote (centre_reclaim) */
static uint16_t s_jnew[0x8000];              /* the view's word (a streamed cell's, to put back after a line) */
static uint16_t s_jline[0x8000];             /* line + 1 whose 4:3 view showed the cell */
static int s_nj, s_hs_touched, s_jnext_kind;
static int s_stream_plane, s_streamed;       /* the plane stream_margins wrote this frame */
static uint16_t s_given[0x8000];             /* the streamed cells holding the game's word now */
static int s_ngiven;
static uint64_t s_reclaimed;                 /* line-cells given back to a line's 4:3 view (diagnostic) */

static void pw16(uint32_t a, uint16_t w)
{
    uint16_t at = (uint16_t)(a & 0xFFFEu);
    uint16_t wi = (uint16_t)(at >> 1);
    if (s_jstamp[wi] != s_stamp) {
        s_jstamp[wi] = s_stamp;
        s_jidx[wi] = (uint16_t)s_nj;
        s_jaddr[s_nj] = at;
        s_jorig[s_nj] = vram16(at);
        s_jtouch[s_nj] = 0;
        s_jkind[s_nj] = (uint8_t)s_jnext_kind;
        s_jline[s_nj] = 0;
        s_nj++;
    }
    s_jnew[s_jidx[wi]] = w;
    put8(at, (uint8_t)(w >> 8));
    put8((uint16_t)(at + 1), (uint8_t)w);
}

extern "C" void ws_vram_touch(uint16_t addr)
{
    uint16_t wi = (uint16_t)(addr >> 1);
    if (s_jstamp[wi] != s_stamp) return;
    s_jtouch[s_jidx[wi]] |= (uint8_t)(1u << (addr & 1));
    s_hs_touched = 1;
}

extern "C" uint8_t ws_vram_game8(uint16_t addr)
{
    uint16_t wi = (uint16_t)(addr >> 1);
    if (s_jstamp[wi] == s_stamp) {
        int j = s_jidx[wi];
        if (!(s_jtouch[j] & (1u << (addr & 1)))) return (addr & 1) ? (uint8_t)s_jorig[j] : (uint8_t)(s_jorig[j] >> 8);
    }
    return vdp()->vram[addr];
}

static void journal_begin(void)
{
    if (++s_stamp == 0) { memset(s_jstamp, 0, sizeof s_jstamp); s_stamp = 1; }
    s_nj = 0;
    s_hs_touched = 0;
    s_streamed = 0;
    s_ngiven = 0;
}

static void journal_restore(void)
{
    for (int j = 0; j < s_nj; j++) {
        uint16_t a = s_jaddr[j];
        if (!(s_jtouch[j] & 1)) put8(a, (uint8_t)(s_jorig[j] >> 8));
        if (!(s_jtouch[j] & 2)) put8((uint16_t)(a + 1), (uint8_t)s_jorig[j]);
    }
    s_nj = 0;
    if (++s_stamp == 0) { memset(s_jstamp, 0, sizeof s_jstamp); s_stamp = 1; }   /* nothing journaled until the next frame */
}

/* unlimited_sprites' host table (the sprites past the 80-entry table, drawn by the engine after the
   SAT chain): moved for the frame as the VDP table's sprites are (sprite_move), in the copy the engine
   draws (spr_host_set_view: the game's table, spr_host_shown, is never touched). The control
   BR_WS_PLANT_HOST=1 leaves it as the game made it (the gap before ws-host: check_host must see it). */
static int s_host_m, s_host_shift;
static const bool s_plant_host = getenv("BR_WS_PLANT_HOST") && *getenv("BR_WS_PLANT_HOST") == '1';

static uint16_t moved_x(uint16_t x, int d) { return (uint16_t)((x & ~0x3FFu) | ((unsigned)((x & 0x3FF) + d) & 0x3FFu)); }

/* The diagnostic BR_WS_HOST_MIRROR=1 (with unlimited_sprites): every VDP table sprite that is not the
   HUD and not at x 0 is drawn again from the host table, behind itself. Its twin is the VDP table
   entry, whose x shift_vram moved: check_host compares the two as drawn (the same place, so the
   picture is the frame without the mirror), and with BR_WS_PLANT_HOST=1 they part by the shift. The
   corpus has no host sprite on a frame the render camera shifts (they are on the game-over snow and
   in section $1100: shift 0), so this is how the shifted case is exercised (NOTES). */
static const bool s_mirror = getenv("BR_WS_HOST_MIRROR") && *getenv("BR_WS_HOST_MIRROR") == '1';
static SprHostPiece s_mirror_pc[80];
static uint16_t s_mirror_twin[80];
static unsigned s_nmirror;

static void host_view(const SprHostPiece *in, SprHostPiece *out, unsigned n)
{
    for (unsigned i = 0; i < n; i++) {
        out[i] = in[i];
        if (s_plant_host) continue;
        int d = sprite_move(hud_side_attr(in[i].attr), in[i].x, s_host_m, s_host_shift);
        if (d) out[i].x = moved_x(in[i].x, d);
    }
}

static void shift_vram(int m, int shift, const Planes &p)
{
    s_nspr = 0;
    GVDP *v = vdp();
    memcpy(s_hs_orig, &v->vram[(uint16_t)p.hbase], sizeof s_hs_orig);
    s_hs_saved = 1;
    if (!m) return;
    s_host_m = m;
    s_host_shift = shift;
    s_nmirror = 0;
    uint32_t sat = (uint32_t)(v->reg[5] & 0x7E) << 9;
    int link = 0;
    for (int n = 0; n < 80; n++) {
        uint32_t e = sat + (uint32_t)link * 8u;
        uint16_t x = vram16(e + 6u);
        int side = hud_side((unsigned)link, vram16(e + 4u));
        int d = sprite_move(side, x, m, shift);
        if (s_mirror && !side && (x & 0x1FF) && s_nmirror < 80) {
            SprHostPiece &mp = s_mirror_pc[s_nmirror];
            mp.y = vram16(e);
            mp.size = v->vram[(uint16_t)(e + 2u)];
            mp.link = 0;
            mp.attr = vram16(e + 4u);
            mp.x = x;
            s_mirror_twin[s_nmirror++] = (uint16_t)e;
        }
        /* the control: one HUD item pinned n px off its place (E3's HUD check must see it):
           BR_WS_PLANT_HUD=n, BR_WS_PLANT_HUD_KIND=1 player 1's panel (the default), 2 player 2's,
           4 the boss meter, 5 the GO! sign */
        static const int plant_hud = getenv("BR_WS_PLANT_HUD") ? atoi(getenv("BR_WS_PLANT_HUD")) : 0;
        static const int plant_kind = getenv("BR_WS_PLANT_HUD_KIND") ? atoi(getenv("BR_WS_PLANT_HUD_KIND")) : 1;
        if (side == plant_kind && m) d += plant_hud;
        if (d || (side && side != 3)) {   /* every HUD item kept for the check, moved or not */
            int xs = (int)(x & 0x3FF) + d;
            s_spr_at[s_nspr] = (uint16_t)e;
            s_spr_y[s_nspr] = vram16(e);
            s_spr_x[s_nspr] = x;
            s_spr_side[s_nspr] = side;
            s_spr_want[s_nspr] = (uint16_t)xs;
            s_nspr++;
            /* the VDP reads 9 bits: an x pushed outside them is off the wide view's 512 (sprites_lost) */
            if (d) pw16(e + 6u, (uint16_t)((x & ~0x3FFu) | ((unsigned)xs & 0x3FFu)));
        }
        link = v->vram[(uint16_t)(e + 3)] & 0x7F;
        if (!link || link >= 80) break;
    }
    if (s_mirror) s_nmirror = spr_host_set_extra(s_mirror_pc, s_nmirror);
    spr_host_set_view(host_view);   /* the host table moved alike (sends it) */
    if (!shift) return;
    s_shifted = 1;
    /* plane x = screen x - scroll: the window at +shift. Only the entries the VDP reads for the
       frame's lines (one in full mode, a line's in cell and line modes): the 1 KB past them is other
       VRAM (with H-scroll at $BC00, $BF80 is the clear tile $5FC's pattern). */
    int lines = (v->reg[1] & 0x08) ? 240 : 224, last = -1;
    for (int y = 0; y < lines; y++) {
        int idx = p.hmode == 0 ? 0 : p.hmode == 2 ? (y & ~7) : y;
        if (idx == last) continue;
        last = idx;
        for (int pl = 0; pl < 2; pl++) {
            uint32_t a = p.hbase + (uint32_t)idx * 4u + (pl ? 2u : 0u);
            uint16_t w = vram16(a);
            pw16(a, (uint16_t)((w & 0xFC00) | ((w - shift) & 0x3FF)));
        }
    }
}

/* The game wrote H-scroll words inside the window (an H-int's raster): the lines still to be drawn
   take the game's new value through the render camera, and the restore keeps the game's value. */
static void reshift_hscroll(const Planes &p)
{
    if (!s_shifted || !s_hs_touched) return;
    s_hs_touched = 0;
    for (int i = 0; i < 0x400; i += 2) {
        uint16_t at = (uint16_t)(p.hbase + (uint32_t)i);
        uint16_t wi = (uint16_t)(at >> 1);
        if (s_jstamp[wi] != s_stamp) continue;
        int j = s_jidx[wi];
        if (!s_jtouch[j]) continue;
        uint16_t cur = vram16(at), o = s_jorig[j];
        uint16_t g = (uint16_t)(((s_jtouch[j] & 1) ? cur : o) & 0xFF00) | (uint16_t)(((s_jtouch[j] & 2) ? cur : o) & 0x00FF);
        s_jorig[j] = g;
        s_jtouch[j] = 0;
        s_hs_orig[i] = (uint8_t)(g >> 8);
        s_hs_orig[i + 1] = (uint8_t)g;
        pw16(at, (uint16_t)((g & 0xFC00) | ((g - g_ws_view.shift) & 0x3FF)));
    }
}

static void unshift_vram(void)
{
    if (s_nmirror) { spr_host_set_extra(nullptr, 0); s_nmirror = 0; }
    spr_host_set_view(nullptr);   /* the host table as the game made it again */
    journal_restore();
    s_nspr = 0;
    s_shifted = 0;
    s_hs_saved = 0;
}

/* ---- the checks (E3) ------------------------------------------------------------------------- */

static std::string s_first_terrain, s_first_lost, s_first_hud;
static const bool s_check_all = getenv("BR_WS_CHECK_ALL") && *getenv("BR_WS_CHECK_ALL") == '1';   /* the control */

static void note_error(std::string &first, const char *fmt, ...)
{
    if (!first.empty()) return;
    char b[256];
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(b, sizeof b, fmt, ap);
    va_end(ap);
    first = std::string(s_screen_id) + " frame " + std::to_string((unsigned long long)s_frame) + ": " + b;
}

/* The name-table word the streamer writes for map tile (tx, ty): its block's two longs of cells
   (Layer_QueueColumn: the block word is a byte offset into the block tiles; the row's two cells a
   long plus the layer's tile word twice). */
static uint16_t layer_word(const Layer &l, int tx, int ty)
{
    int w2 = l.width * 2, h2 = l.height * 2;
    tx = ((tx % w2) + w2) % w2;
    ty = ((ty % h2) + h2) % h2;
    uint16_t off = (uint16_t)((uint32_t)(ty >> 1) * l.map_row_bytes + (uint32_t)(tx >> 1) * 2u);
    uint16_t block = m16(l.map + (uint32_t)(int32_t)(int16_t)off);
    uint32_t pair = l.tiles + (uint32_t)(int32_t)(int16_t)block + (uint32_t)(ty & 1) * 4u;
    uint32_t cells = m32(pair) + l.cell_add;
    return (tx & 1) ? (uint16_t)cells : (uint16_t)(cells >> 16);
}

/* The plane x the layer puts at screen x 0: its block column block_col is at plane cell col, and
   the view's left is x - block_col x 16 past it. The game keeps a block of slack (block_col is
   (x >> 4) - 1: 16..31 px past it; at the map's left end block_col wraps to width - 1), so the
   x & 15 this once used was 16 px short, no line ever matched it and the streamed margins went
   unchecked (ws-ctl, 2026-10-30: streamed_frames counts the checks that saw a line). */
#define STREAM_LEFT 5    /* the streamer's margin blocks each side of the game's 22 (stream_margins) */
#define STREAM_RIGHT 5

static int layer_plane_x0(const Planes &p, const Layer &l)
{
    int W = l.width * 16;
    int d = ((l.x - l.block_col * 16) % W + W) % W;
    return (l.col * 8 + d) & (p.wcells * 8 - 1);
}

/* Every cell the wide columns show on plane `plane` (each line: its H-scroll and V-scroll), against
   the block map (the streamed layer's plane) or against what the game drew (any other plane: a
   cell still holding the clear tile $05FC, VDP_ClearPlanes' and Op6_ClearPlane's, was never drawn) */
static void check_plane(const Planes &p, const Layer &l, int plane, int m, int shift, int bar_l, int bar_r)
{
    static uint8_t seen[128][128];
    memset(seen, 0, sizeof seen);
    int x0 = -m + shift + bar_l, x1 = 320 + m + shift - bar_r;
    int pw = p.wcells * 8, ph = p.hcells * 8;
    bool streamed = l.valid && l.plane == plane;
    int lx0 = streamed ? layer_plane_x0(p, l) : 0, lines = 0, apart = 0;
    for (int y = 0; y < 224; y++) {
        int hs = hscroll_at(p, plane, y);
        /* a line shows the streamed layer at its place where its scroll puts the layer's view left
           (layer_plane_x0) at screen x 0: every cell it shows is checked. A line scrolled apart (the
           flight's wobble, $2300: every line a few px off the layer's x; a parallax band) is checked
           only in the plane's cells the streamed window keeps (in_window): the rest of the plane
           holds what no one streams (check_whole judges the planes drawn whole). */
        bool at = !streamed || ((-hs - lx0) & (pw - 1)) == 0;
        if (at) lines++;
        else apart++;
        for (int x = x0; x < x1; x++) {
            if (x == 0 && !s_check_all) x = 320;
            if (x >= x1) break;
            int vs = vscroll_at(p, plane, x);
            int px = (x - hs) & (pw - 1), py = (y + vs) & (ph - 1);
            seen[py >> 3][px >> 3] |= at ? 1 : 2;
        }
    }
    if (!streamed) return;   /* a plane drawn whole: check_whole */
    if (lines || apart) g_enh.ws.streamed_frames++;
    g_enh.ws.streamed_lines += (uint64_t)lines;
    g_enh.ws.streamed_lines_apart += (uint64_t)apart;
    int rows_kept = 2 * s16at(RAM_LAYER_A + 0x40);   /* the block rows stream_margins writes, from l.row */
    for (int r = 0; r < p.hcells; r++)
        for (int c = 0; c < p.wcells; c++) {
            if (!seen[r][c]) continue;
            int relc = (c - l.col) & (p.wcells - 1);
            /* the streamer's cells: block columns -STREAM_LEFT..-1 and 22..21 + STREAM_RIGHT of its rows (the game's
               own 0..21 hold what the game drew, which is not always the map: the flight $2300 starts
               on the cleared plane, $05FC where the map has $031A, until its scrolling restreams each
               column, ~340 frames) */
            bool in_window = ((relc >= 44 && relc < 2 * (22 + STREAM_RIGHT)) || relc >= p.wcells - 2 * STREAM_LEFT) &&
                             ((r - l.row) & (p.hcells - 1)) < rows_kept;
            if (seen[r][c] == 2 && !in_window) continue;   /* only lines scrolled apart show it */
            g_enh.ws.terrain_checks++;
            uint16_t got = vram16(p.base[plane] + (uint32_t)(r * p.wcells + c) * 2u);
            uint16_t want;
            bool bad;
            if (streamed) {
                int rel = relc;
                if (rel >= p.wcells - 2 * STREAM_LEFT) rel -= p.wcells;   /* the left margin: the streamer's blocks -5..-1 */
                if (rel < 0) g_enh.ws.streamed_cells_left++;
                else if (rel >= 44) g_enh.ws.streamed_cells_right++;   /* past the game's 22 blocks */
                int rely = (r - l.row) & (p.hcells - 1);
                if (rely >= p.hcells - 8 && p.hcells > 32) rely -= p.hcells;
                want = layer_word(l, l.block_col * 2 + rel, l.block_row * 2 + rely);
                bad = got != want;
            } else {
                want = 0x05FC;
                bad = got == 0x05FC;
            }
            if (bad) {
                g_enh.ws.terrain_errors++;
                note_error(s_first_terrain, "plane %c cell (%d,%d) vram %04X %s %04X", plane ? 'B' : 'A', c, r, got,
                           streamed ? "map" : "clear", want);
            }
        }
}

/* The planes drawn whole: every 8-px column of every line the wide columns show (outside the bars)
   shows something drawn on one plane, or the 4:3 edge of that line shows nothing either (the
   backdrop goes on). */
static void check_whole(const Planes &p, int m, int shift, int bar_l, int bar_r)
{
    int x0 = -m + shift + bar_l, x1 = 320 + m + shift - bar_r;
    for (int y = 0; y < 224; y++) {
        int edge_l = pixel_drawn(p, 0, y), edge_r = pixel_drawn(p, 319, y);
        for (int x = x0; x < x1; x += 8) {
            if (x >= 0 && x < 320) { x = 312; continue; }
            g_enh.ws.terrain_checks++;
            if (!pixel_drawn(p, x, y) && (x < 0 ? edge_l : edge_r)) {
                g_enh.ws.terrain_errors++;
                note_error(s_first_terrain, "nothing drawn at x %d line %d (planes A %04X B %04X)", x, y,
                           cell_at(p, 0, x, y), cell_at(p, 1, x, y));
            }
        }
    }
}

/* The street's margins ($1100, every wide gameplay frame): each margin pixel of the drawn frame
   against the model of the facade going on past the 4:3 view (street_px: plane A as drawn, plane
   B at the line's own plane x where it is inside the line's 4:3 range, else one segment on or back,
   the signs in the states the game gives them; composited by priority, shadowed where no plane
   pixel is high, the backdrop where both are clear). The pixels a sprite drew (the layer's opaque
   columns before the fold) and, in shadow/highlight mode, every pixel a palette-3 sprite's box
   covers (its operators shade what is under them) are left out, counted. The model's words must
   be the game's: a line whose plane B scroll word is not the replica's is an error. Coverage in
   the ws JSON's `street`: frames, pixels compared, folded (of them, where the model folds),
   sprite_pixels (left out), errors (also terrain errors). The control BR_WS_PLANT_STREET=1 (no
   fold: the view the user saw) or =n (the fold n px off its period) must fail. */
static std::string s_first_street;

static uint8_t s_st_op[224][GVDP_MAX_WIDTH];

static void st_op_box(int x, int y, int size, int fx0)
{
    int w = ((size >> 2) & 3) * 8 + 8, h = (size & 3) * 8 + 8;
    for (int yy = y; yy < y + h; yy++) {
        if (yy < 0 || yy >= 224) continue;
        for (int xx = x; xx < x + w; xx++) {
            int ox = xx + fx0;
            if (ox >= 0 && ox < GVDP_MAX_WIDTH) s_st_op[yy][ox] = 1;
        }
    }
}

/* The fire's lines (every wide gameplay frame of $1100 with the fire burning or stopped): each
   line's plane A word as drawn against the model (fire_now: SwayFireLines' arithmetic on the RAM as
   it stands; past the game's cut, the sway drawn on and clamped past the wide edge). Before the cut
   the drawn words are the game's own, so the replica is held to them; past it the view's. A line
   off the model is an error (fire_errors, terrain errors too), and check_street's pixel model takes
   plane A at the model's words on these lines, so a fire left frozen in a margin is a pixel error
   wherever it shows. Coverage: ws JSON `street.fire`. Control BR_WS_PLANT_FIRE=1 (no view words:
   the frozen fire the user saw) must fail. */
static std::string s_first_fire;

static void check_fire(const Planes &p)
{
    if (!s_fire.valid) return;
    g_enh.ws.fire_frames++;
    if (s_fire.cut) g_enh.ws.fire_continued++;
    for (int y = 0; y < s_fire.n; y++) {
        g_enh.ws.fire_lines++;
        int drawn = hscroll_at(p, 0, y);
        if (((drawn - s_fire.want[y]) & 0x3FF) == 0) continue;
        g_enh.ws.fire_errors++;
        g_enh.ws.terrain_errors++;
        note_error(s_first_fire, "frame %llu line %d: plane A scroll %d, the fire's %d (%s)", (unsigned long long)s_frame, y,
                   drawn, s_fire.want[y] & 0x3FF, s_fire.cut ? "past the cut" : "the game's");
        note_error(s_first_terrain, "fire line %d: plane A scroll %d, the model %d", y, drawn, s_fire.want[y] & 0x3FF);
    }
}

static void check_street(const Planes &p, int m, int shift)
{
    if (!s_st.valid) return;
    check_fire(p);
    g_enh.ws.street_frames++;
    const GVDP *v = vdp();
    for (int y = 0; y < 224; y++)
        if (((hscroll_at(p, 1, y) - s_st.game[y]) & 0x3FF) != 0) {
            g_enh.ws.street_errors++;
            g_enh.ws.terrain_errors++;
            note_error(s_first_street, "line %d: plane B scroll %d, the street's replica %d", y, hscroll_at(p, 1, y), s_st.game[y]);
            note_error(s_first_terrain, "street line %d: plane B scroll %d, the replica %d", y, hscroll_at(p, 1, y), s_st.game[y]);
        }
    int stride, w, h;
    const uint32_t *fb = nm_framebuffer(&stride, &w, &h);
    const uint32_t *pal = nm_palette();
    int fx0 = g_ws_view.centre_x0;   /* frame x of screen x 0 */
    if (s_st.sh) {   /* the operators' boxes: the table's palette-3 sprites, the host table's */
        memset(s_st_op, 0, sizeof s_st_op);
        uint32_t sat = (uint32_t)(v->reg[5] & 0x7E) << 9;
        int link = 0;
        for (int n = 0; n < 80; n++) {
            uint32_t e = sat + (uint32_t)link * 8u;
            if ((vram16(e + 4u) >> 13 & 3) == 3)
                st_op_box((vram16(e + 6u) & 0x1FF) - 128, (vram16(e) & 0x1FF) - 128, v->vram[(uint16_t)(e + 2u)], fx0);
            link = v->vram[(uint16_t)(e + 3)] & 0x7F;
            if (!link || link >= 80) break;
        }
        const SprHostPiece *hp;
        unsigned nh = spr_host_drawn(&hp);
        for (unsigned k = 0; k < nh; k++)
            if ((hp[k].attr >> 13 & 3) == 3) st_op_box((hp[k].x & 0x1FF) - 128, (hp[k].y & 0x1FF) - 128, hp[k].size, fx0);
    }
    int x0 = -m + shift, x1 = 320 + m + shift;
    for (int y = 0; y < 224 && y < h; y++) {
        /* the fire's lines: plane A where the model puts the fire (check_fire), not as drawn */
        int hs_a = s_fire.valid && y < s_fire.n ? (s_fire.want[y] & 0x3FF) : hscroll_at(p, 0, y);
        int hs_b = hscroll_at(p, 1, y);
        const uint32_t *row = fb + (size_t)y * (size_t)stride;
        for (int x = x0; x < x1; x++) {
            if (x == 0) { x = 319; continue; }
            int ox = x + fx0;
            if (ox < 0 || ox >= w || ox >= GVDP_MAX_WIDTH) continue;
            if (s_st_sprite[y][ox] || (s_st.sh && s_st_op[y][ox])) { g_enh.ws.street_sprite_pixels++; continue; }
            StreetPx s = street_px(v, hs_a, hs_b, x, y);
            int want = s.idx + (s_st.sh && !s.hi_present ? GVDP_PALETTE_SHADOW : 0);
            g_enh.ws.street_pixels++;
            g_enh.ws.terrain_checks++;
            if (s.fold) g_enh.ws.street_folded++;
            if (row[ox] != pal[want]) {
                g_enh.ws.street_errors++;
                g_enh.ws.terrain_errors++;
                note_error(s_first_street, "x %d line %d: %08X, the street's %08X (CRAM %d%s)", x, y, row[ox], pal[want],
                           s.idx, s.fold ? (s.fold < 0 ? ", folded left" : ", folded right") : "");
                note_error(s_first_terrain, "street x %d line %d: %08X, the model's %08X", x, y, row[ox], pal[want]);
            }
        }
    }
}

/* lost: a sprite the wide view shows whose x the VDP's 9 bits can't hold; the HUD: each panel
   piece at its side's edge (x moved by the margin from the game's) */
static void check_sprites(int m, bool hud_hidden)
{
    for (int k = 0; k < s_nspr; k++) {
        int want = s_spr_want[k];
        uint8_t size = vdp()->vram[(uint16_t)(s_spr_at[k] + 2u)];
        int w = ((size >> 2) + 1) * 8;
        int vl = 128 - m, vr = 448 + m;
        if ((want < 0 || want > 0x1FF) && want + w > vl && want < vr) {
            g_enh.ws.sprites_lost++;
            note_error(s_first_lost, "sprite at %04X x %d", s_spr_at[k], want - 128);
        }
    }
    /* the HUD: the x the frame was drawn with (VRAM) against the game's (kept from before the move) */
    /* every item: the panels, the boss meter and the GO! sign (not the cover: no HUD) */
    bool bad = false, seen[6] = {};
    for (int k = 0; k < s_nspr; k++) {
        int side = s_spr_side[k];
        if (!side || side == 3) continue;
        seen[side] = true;
        int game_x = s_spr_x[k] & 0x3FF, drawn = vram16(s_spr_at[k] + 6u) & 0x3FF;
        if (((drawn - game_x - hud_move(side, m)) & 0x3FF) != 0) {
            bad = true;
            note_error(s_first_hud, "HUD sprite (item %d) at %04X x %d, the game's %d", side, s_spr_at[k], drawn - 128,
                       game_x - 128);
        }
    }
    for (int i = 0; i < 6; i++) if (seen[i]) g_enh.ws.hud_frames[i]++;
    if (bad || hud_hidden) g_enh.ws.hud_misplaced++;   /* a piece the bars hid is not at its place either */
}

/* ---- the HUD over the bars (ws-hudbars, 2026-10-30) -------------------------------------------
   The bars (shift_for: past a streamed map's ends; content_bars: past a picture drawn whole) are
   painted over the finished frame, and the HUD is pinned to the frame's edges (hud_move), so where
   a screen has bars the pinned pieces were drawn under them (the lift $2100, bars 40 | 40: player
   1's panel 23 px hidden, player 2's 40). As X-Men 2 does (its widescreen.cpp bars_now /
   ws_mask_line: "the HUD's sprites stay on top of the bars"), the bars leave every pixel a pinned
   piece draws: the panels, the boss meter and the GO! sign keep one place at the frame's edges
   whether the screen has bars or not (a bar that comes and goes, content_bars' per-frame test,
   never moves them), and the HUD sits in the black band. X-Men 2 keeps each piece's rectangle;
   here only its opaque pixels, so the bar stays black around the glyphs (no rectangle of whatever
   the planes hold past the map). Presentation only: the frame's pixels, after it is drawn.
   Control: BR_WS_PLANT_HUD_BARS=1 paints the bars over everything again (the view before this). */

WsBars g_ws_bars;
static const bool s_plant_hud_bars = getenv("BR_WS_PLANT_HUD_BARS") && *getenv("BR_WS_PLANT_HUD_BARS") == '1';

/* Every opaque pixel of each pinned HUD piece as this frame draws it, from VRAM's table (after
   shift_vram: the x the frame is drawn with) and the pieces' tiles: fn(item, frame x, line, CRAM
   index). Frame x = VDP x - 128 + m (the canvas puts screen x 0 at m). */
template <class F>
static void hud_pixels(int m, F fn)
{
    GVDP *v = vdp();
    uint32_t sat = (uint32_t)(v->reg[5] & 0x7E) << 9;
    int link = 0;
    for (int n = 0; n < 80; n++) {
        uint32_t e = sat + (uint32_t)link * 8u;
        uint16_t attr = vram16(e + 4u);
        int side = hud_side((unsigned)link, attr);
        if (side && side != 3) {
            uint8_t size = v->vram[(uint16_t)(e + 2u)];
            int wc = ((size >> 2) & 3) + 1, hc = (size & 3) + 1;
            int fx0 = (int)(vram16(e + 6u) & 0x1FF) - 128 + m, y0 = (int)(vram16(e) & 0x1FF) - 128;
            int pal = (attr >> 13) & 3;
            bool hf = (attr & 0x800) != 0, vf = (attr & 0x1000) != 0;
            for (int py = 0; py < hc * 8; py++) {
                int line = y0 + py;
                if (line < 0 || line >= WS_BAR_ROWS) continue;
                int sy = vf ? hc * 8 - 1 - py : py;
                for (int px = 0; px < wc * 8; px++) {
                    int sx = hf ? wc * 8 - 1 - px : px;
                    uint32_t tile = ((uint32_t)(attr & 0x7FF) + (uint32_t)((sx >> 3) * hc + (sy >> 3))) & 0x7FFu;
                    uint8_t b = v->vram[(uint16_t)(tile * 32u + (uint32_t)(sy & 7) * 4u + (uint32_t)(sx & 7) / 2u)];
                    int idx = (sx & 1) ? (b & 15) : (b >> 4);
                    if (idx) fn(side, fx0 + px, line, pal * 16 + idx);
                }
            }
        }
        link = v->vram[(uint16_t)(e + 3)] & 0x7F;
        if (!link || link >= 80) break;
    }
}

/* ws_frame_setup, after shift_vram: the bars of this frame and the HUD's pixels inside them */
static void bars_keep(int m)
{
    WsBars &b = g_ws_bars;
    if (b.nkeep) memset(b.keep, 0, sizeof b.keep);
    b.nkeep = 0;
    b.bar_l = g_ws_view.bar_l;
    b.bar_r = g_ws_view.bar_r;
    b.w = 320 + 2 * WS_MARGIN;
    if (!m || (!b.bar_l && !b.bar_r)) return;
    hud_pixels(m, [&](int, int fx, int line, int) {
        uint8_t *k = nullptr;
        if (fx >= 0 && fx < b.bar_l && fx < WS_BAR_MAX) k = &b.keep[line][0][fx];
        else if (fx >= b.w - b.bar_r && fx < b.w && b.w - 1 - fx < WS_BAR_MAX) k = &b.keep[line][1][b.w - 1 - fx];
        if (k && !*k) { *k = 1; b.nkeep++; }
    });
}

/* The bars: black past the picture's ends, the pinned HUD's pixels left (the frame's own) */
extern "C" void ws_bars_paint(const WsBars *b, uint32_t *fb, int stride, int w, int h)
{
    if (!b->bar_l && !b->bar_r) return;
    bool keep = b->nkeep && !s_plant_hud_bars && w == b->w;
    for (int y = 0; y < h; y++) {
        uint32_t *row = fb + (size_t)y * (size_t)stride;
        int line = h > 256 ? y >> 1 : y;
        bool k = keep && line < WS_BAR_ROWS;
        for (int x = 0; x < b->bar_l && x < w; x++)
            if (!(k && x < WS_BAR_MAX && b->keep[line][0][x])) row[x] = 0xFF000000u;
        for (int x = w - b->bar_r; x < w; x++) {
            if (x < 0) continue;
            int r = w - 1 - x;
            if (!(k && r < WS_BAR_MAX && b->keep[line][1][r])) row[x] = 0xFF000000u;
        }
    }
}

/* The check (wide gameplay frames with a bar): every opaque pixel of a pinned piece inside a bar,
   walked again from VRAM as the frame was drawn, is the frame's pixel before the bars and after
   them; one painted over is hidden (hud_hidden_*, and the frame is hud_misplaced). Per screen and
   item in the ws JSON's `hud_bars`. Also counted: the frame not showing the piece's own colour
   there before the bars (hud_bar_unmatched, a diagnostic of the walk: 0 where no sprite is in
   front of the HUD). */
struct HudBarPx { int16_t x, y; uint8_t item, idx; uint32_t was; };
static std::vector<HudBarPx> s_hbp;

static void hud_bars_before(int m, const uint32_t *fb, int stride, int w, int h)
{
    s_hbp.clear();
    int bl = g_ws_view.bar_l, br = g_ws_view.bar_r;
    if (!m || (!bl && !br) || h > 256) return;
    hud_pixels(m, [&](int item, int fx, int line, int idx) {
        if (fx < 0 || fx >= w || line >= h) return;
        if (fx >= bl && fx < w - br) return;
        s_hbp.push_back({ (int16_t)fx, (int16_t)line, (uint8_t)item, (uint8_t)idx, fb[(size_t)line * (size_t)stride + (size_t)fx] });
    });
}

static bool hud_bars_after(const uint32_t *fb, int stride, int w)
{
    if (s_hbp.empty()) return false;
    EnhWs &ws = g_enh.ws;
    const uint32_t *pal = nm_palette();
    struct Seen { uint64_t px, hid; int out; } seen[6] = {};
    int bl = g_ws_view.bar_l, br = g_ws_view.bar_r;
    for (const HudBarPx &p : s_hbp) {
        Seen &s = seen[p.item];
        s.px++;
        int out = p.x < bl ? bl - p.x : p.x - (w - br) + 1;   /* px from the bar's inner edge */
        if (out > s.out) s.out = out;
        if (p.was != pal[p.idx]) ws.hud_bar_unmatched++;
        if (fb[(size_t)p.y * (size_t)stride + (size_t)p.x] != p.was) {
            s.hid++;
            note_error(s_first_hud, "HUD item %d: x %d line %d painted over by the %s bar (%d | %d)", p.item, p.x,
                       p.y, p.x < bl ? "left" : "right", bl, br);
        }
    }
    ws.hud_bar_frames++;
    ws.hud_bar_pixels += s_hbp.size();
    bool hidden = false;
    for (int item = 1; item < 6; item++) {
        Seen &s = seen[item];
        if (!s.px) continue;
        ws.hud_hidden_pixels += s.hid;
        hidden |= s.hid != 0;
        int i = 0;
        while (i < ws.nhud_bars && !(ws.hud_bars[i].item == item && !strcmp(ws.hud_bars[i].id, s_screen_id))) i++;
        if (i == ws.nhud_bars) {
            if (ws.nhud_bars >= ENH_HUD_BARS) continue;
            EnhHudBar &n = ws.hud_bars[ws.nhud_bars++];
            memset(&n, 0, sizeof n);
            snprintf(n.id, sizeof n.id, "%s", s_screen_id);
            n.item = item;
        }
        EnhHudBar &e = ws.hud_bars[i];
        e.frames++;
        e.pixels += s.px;
        e.hidden += s.hid;
        if (s.out > e.cols_max) e.cols_max = s.out;
    }
    if (hidden) ws.hud_hidden_frames++;
    s_hbp.clear();
    return hidden;
}

static std::string hud_bars_json(void)
{
    const EnhWs &w = g_enh.ws;
    auto u = [](uint64_t v) { return std::to_string((unsigned long long)v); };
    static const char *const names[6] = { "", "panel_p1", "panel_p2", "", "boss_meter", "go_sign" };
    std::string o = "\"hud_bars\": {\"frames\": " + u(w.hud_bar_frames) + ", \"pixels\": " + u(w.hud_bar_pixels) +
                    ", \"hidden_frames\": " + u(w.hud_hidden_frames) + ", \"hidden_pixels\": " + u(w.hud_hidden_pixels) +
                    ", \"unmatched\": " + u(w.hud_bar_unmatched) + ", \"by\": [";
    for (int i = 0; i < w.nhud_bars; i++) {
        const EnhHudBar &e = w.hud_bars[i];
        o += std::string(i ? ", " : "") + "{\"screen\": \"" + e.id + "\", \"item\": \"" + names[e.item] +
             "\", \"frames\": " + u(e.frames) + ", \"pixels\": " + u(e.pixels) + ", \"hidden\": " + u(e.hidden) +
             ", \"px_into_bar\": " + std::to_string(e.cols_max) + "}";
    }
    return o + "]}";
}

/* unlimited_sprites' host table, on every frame drawn with widescreen on: each sprite the engine drew
   (spr_host_drawn, what gvdp_set_host_sat was given) against the game's (spr_host_shown) moved as a
   VDP table sprite with that attribute and x is moved (the render camera's -shift, the HUD's pin, x 0
   kept): host_sprites_misplaced. On gameplay frames, as the table's (check_sprites): a moved one the
   wide view shows outside the 9-bit x is lost. */
static std::string s_first_host;

static void check_host(int m, int shift, int gameplay)
{
    const SprHostPiece *game, *drawn;
    unsigned ng = spr_host_shown(&game), nd = spr_host_drawn(&drawn);
    if (!spr_host_on()) return;
    if (!ng && !nd) return;
    if (shift) g_enh.ws.host_shift_frames++;
    unsigned nm = s_nmirror, n = ng + nm > nd ? ng + nm : nd;
    if (gvdp_host_sat_count() != nd) {
        g_enh.ws.host_sprites_misplaced += n;
        note_error(s_first_host, "the engine draws %u host sprites, %u sent", gvdp_host_sat_count(), nd);
        return;
    }
    for (unsigned k = 0; k < n; k++) {
        g_enh.ws.host_sprites_checked++;
        if (k >= ng && k < ng + nm && k < nd) {   /* the mirror: drawn where its VDP table twin is drawn */
            const SprHostPiece &d = drawn[k];
            uint32_t t = s_mirror_twin[k - ng];
            if (((d.x ^ vram16(t + 6u)) & 0x3FF) || d.y != vram16(t) || d.attr != vram16(t + 4u)) {
                g_enh.ws.host_sprites_misplaced++;
                note_error(s_first_host, "mirror %u x %d, its table twin at %04X x %d (shift %d)", k - ng,
                           (d.x & 0x3FF) - 128, t, (vram16(t + 6u) & 0x3FF) - 128, shift);
            }
            continue;
        }
        if (k >= ng + nm || k >= nd) {
            g_enh.ws.host_sprites_misplaced++;
            note_error(s_first_host, "host sprite %u: %u drawn, %u in the game's table", k, nd, ng);
            continue;
        }
        const SprHostPiece &g = game[k], &d = drawn[k];
        int side = hud_side_attr(g.attr);
        int want = (int)(g.x & 0x3FF) + sprite_move(side, g.x, m, shift);
        if ((((int)(d.x & 0x3FF) - want) & 0x3FF) || d.y != g.y || d.size != g.size || d.attr != g.attr) {
            g_enh.ws.host_sprites_misplaced++;
            note_error(s_first_host, "host sprite %u x %d, the game's %d moved %d (shift %d)", k, (d.x & 0x3FF) - 128,
                       (g.x & 0x3FF) - 128, want - (int)(g.x & 0x3FF), shift);
        }
        int w = ((g.size >> 2) + 1) * 8;
        int vl = 128 - m, vr = 448 + m;
        if (gameplay && (want < 0 || want > 0x1FF) && want + w > vl && want < vr) {
            g_enh.ws.sprites_lost++;
            note_error(s_first_lost, "host sprite %u x %d", k, want - 128);
        }
    }
}

/* ---- screenshots (diagnosing: BR_WS_SHOT=DIR, one PPM per screen id at its BR_WS_SHOT_AT'th frame) */

static void shot(void)
{
    static const char *dir = getenv("BR_WS_SHOT");
    if (!dir || !*dir) return;
    static int at = getenv("BR_WS_SHOT_AT") ? atoi(getenv("BR_WS_SHOT_AT")) : 120;
    /* BR_WS_SHOT_EVERY=k: every k'th frame from the at'th on; BR_WS_SHOT_SCREEN=id: that screen only */
    static int every = getenv("BR_WS_SHOT_EVERY") ? atoi(getenv("BR_WS_SHOT_EVERY")) : 0;
    static const char *only = getenv("BR_WS_SHOT_SCREEN");
    static char last[24];
    static int n;
    if (strcmp(last, s_screen_id)) { snprintf(last, sizeof last, "%s", s_screen_id); n = 0; }
    if (only && *only && strcmp(only, s_screen_id)) return;
    ++n;
    if (every > 0 ? (n < at || (n - at) % every) : n != at) return;
    int stride, w, h;
    const uint32_t *fb = nm_framebuffer(&stride, &w, &h);
    char path[700];
    snprintf(path, sizeof path, "%s/ws_%s_%d_%llu.ppm", dir, s_screen_id, (int)ws_getpid(), (unsigned long long)s_frame);
    FILE *f = fopen(path, "wb");
    if (!f) return;
    fprintf(f, "P6\n%d %d\n255\n", w, h);
    for (int y = 0; y < h; y++)
        for (int x = 0; x < w; x++) {
            uint32_t c = fb[(size_t)y * (size_t)stride + (size_t)x];
            uint8_t rgb[3] = { (uint8_t)(c >> 16), (uint8_t)(c >> 8), (uint8_t)c };
            fwrite(rgb, 1, 3, f);
        }
    fclose(f);
    static const bool vram = getenv("BR_WS_SHOT_VRAM") && *getenv("BR_WS_SHOT_VRAM") == '1';   /* + VRAM, VSRAM, regs */
    if (!vram) return;
    snprintf(path, sizeof path, "%s/ws_%s_%d_%llu.vram", dir, s_screen_id, (int)ws_getpid(), (unsigned long long)s_frame);
    if (FILE *g = fopen(path, "wb")) {
        fwrite(vdp()->vram, 1, 0x10000, g);
        fwrite(vdp()->vsram, sizeof vdp()->vsram[0], 40, g);
        fwrite(vdp()->reg, 1, 24, g);
        fwrite(vdp()->cram, sizeof vdp()->cram[0], 64, g);
        fclose(g);
    }
}

/* ---- the streamer, widened ------------------------------------------------------------------
   The game streams 22 block columns of g_LayerA (rel 0..21 from its block_col: 320 px and a
   block). The wide view (and the render camera's shift) shows up to 5 more left and 5 more right:
   before each drawn frame they are written straight into the plane's name table, as
   Layer_QueueColumn would write them (the same walk: the block row, a block a map row down, the
   map's rewind at its height; the plane's row wrap), so the game's tile queue, its RAM and its
   timing stay the original's. Columns 0..21 stay the game's own. The right needs 5 where the
   view starts left of the map (the flight's entry, $2400: layer x -340..0): the render camera
   stops at +40 while the view's left is 16..31 px into block 0, so the view's x 399 is up to
   400 + 31 - 352 = 79 px past block 21 (block 26). 22 + 5 + 5 = 32 blocks fill a 64-cell plane
   with no overlap. (STREAM_LEFT, STREAM_RIGHT: above check_plane, which knows the window too) */

static void stream_margins(const Planes &p, const Layer &l)
{
    if (!l.valid) return;
    s_jnext_kind = 1;
    uint32_t L = RAM_LAYER_A;
    int blocks_y = s16at(L + 0x40), map_bytes = m16(L + 0x2C);
    uint32_t base = p.base[l.plane];
    static const int plant = getenv("BR_WS_PLANT_COLUMN") ? atoi(getenv("BR_WS_PLANT_COLUMN")) : 0;   /* control */
    for (int rel = -STREAM_LEFT; rel < 22 + STREAM_RIGHT; rel++) {
        if (rel == 0) rel = 22;
        int bc = l.block_col + rel;
        while (bc < 0) bc += l.width;
        while (bc >= l.width) bc -= l.width;
        int cell = (l.col + 2 * rel) & (p.wcells - 1);
        uint32_t at = l.map + (uint32_t)(int32_t)(int16_t)(uint16_t)((uint32_t)l.block_row * l.map_row_bytes + (uint32_t)bc * 2u);
        int left = l.height - l.block_row, r = l.row;
        for (int b = 0; b < blocks_y; b++) {
            uint16_t block = m16(at);
            uint32_t pair = l.tiles + (uint32_t)(int32_t)(int16_t)block;
            for (int k = 0; k < 2; k++) {
                uint32_t cells = m32(pair + 4u * (uint32_t)k) + l.cell_add;
                if (plant && rel == plant && b == 2) cells ^= 0x00010001u;
                uint32_t row_at = base + (uint32_t)(((r + k) & (p.hcells - 1)) * p.wcells) * 2u;
                pw16(row_at + (uint32_t)cell * 2u, (uint16_t)(cells >> 16));
                pw16(row_at + (uint32_t)((cell + 1) & (p.wcells - 1)) * 2u, (uint16_t)cells);
            }
            r = (r + 2) & (p.hcells - 1);
            at += l.map_row_bytes;
            if (--left == 0) { at -= (uint32_t)map_bytes; left = l.height; }
        }
    }
    s_jnext_kind = 0;
    s_stream_plane = l.plane;
    s_streamed = 1;
}

/* a journaled word's bytes the game has not written inside the window */
static void jput(int j, uint16_t w)
{
    uint16_t a = s_jaddr[j];
    if (!(s_jtouch[j] & 1)) put8(a, (uint8_t)(w >> 8));
    if (!(s_jtouch[j] & 2)) put8((uint16_t)(a + 1), (uint8_t)w);
}

/* the streamed cells given back put back to the view's words (after a line, at the frame's end) */
static void restream(int keep_line)
{
    int n = 0;
    for (int k = 0; k < s_ngiven; k++) {
        int j = s_given[k];
        if (keep_line && s_jline[j] == keep_line) { s_given[n++] = (uint16_t)j; continue; }
        jput(j, s_jnew[j]);
    }
    s_ngiven = n;
}

/* Before each line is drawn: a streamed margin cell this line's 4:3 view shows holds the game's
   word for that line (and the view's again on the next line that doesn't show it). On a line whose
   scroll shows the layer at its place the margins are outside the 4:3 view by construction; a line
   scrolled apart (a parallax band, the flight's per-line floor) can show those plane columns in the
   middle of the picture, and there the 4:3 frame shows the game's cells (E3: the centre equals the
   4:3 run's). With the line's own H-scroll (the game's, before the render camera) and V-scroll
   (live: the H-int's raster). */
static void centre_reclaim(const Planes &p, int y)
{
    if (!s_streamed) return;
    uint16_t mark = (uint16_t)(y + 1);
    int plane = s_stream_plane;
    int pw = p.wcells * 8, ph = p.hcells * 8;
    int idx = p.hmode == 0 ? 0 : p.hmode == 2 ? (y & ~7) : y;
    uint32_t off = (uint32_t)idx * 4u + (plane ? 2u : 0u);
    /* the game's H-scroll word: its journaled value under the render camera (kept current by
       reshift_hscroll), else VRAM as it stands (an H-int may have rewritten it) */
    int hs = (s_shifted ? (s_hs_orig[off] << 8 | s_hs_orig[off + 1]) : vram16(p.hbase + off)) & 0x3FF;
    for (int x = 0; x < 320;) {
        int px = (x - hs) & (pw - 1);
        int vs = vscroll_at(p, plane, x);
        int py = (y + vs) & (ph - 1);
        uint16_t at = (uint16_t)(p.base[plane] + (uint32_t)((py >> 3) * p.wcells + (px >> 3)) * 2u);
        uint16_t wi = (uint16_t)(at >> 1);
        if (s_jstamp[wi] == s_stamp) {
            int j = s_jidx[wi];
            if (s_jkind[j] == 1 && s_jline[j] != mark) {
                bool given = y > 0 && s_jline[j] == y;   /* given back on the line before: still in s_given */
                s_jline[j] = mark;
                if (!given) {
                    jput(j, s_jorig[j]);
                    s_given[s_ngiven++] = (uint16_t)j;
                    s_reclaimed++;
                }
            }
        }
        int to_cell = 8 - (px & 7), to_col = 16 - (x & 15);   /* the next cell, or the next V-scroll column */
        x += p.vcol && to_col < to_cell ? to_col : to_cell;
    }
    restream(mark);   /* the cells the line before needed and this one doesn't */
}

/* ---- the centre (E3's fb_centre), in either view ----------------------------------------------
   Set in every run alike, widescreen or not, from the game's state: on gameplay frames of a scene
   with the x 0-15 cover (g_SceneFixedSpr) the centre skips x 0-15 (the cover goes to the wide
   left edge, the picture under it shows), and every HUD piece (the panels, the boss meter, the GO!
   sign) is a rectangle hashed as zero both where the 4:3 view draws it and where the wide view pins
   it (hud_move, in centre coordinates of the wide view's render camera; one rectangle when the two
   are the same place: the boss meter without a render camera shift). */
static void centre_exceptions(int shift)
{
    g_enh_centre_skip = 0;
    g_enh_centre_nrects = 0;
    if (!is_gameplay()) return;
    if (m8(RAM_FIXED_SPR)) g_enh_centre_skip = 16;
    GVDP *v = vdp();
    uint32_t sat = (uint32_t)(v->reg[5] & 0x7E) << 9;
    int link = 0;
    for (int n = 0; n < 80; n++) {
        uint32_t e = sat + (uint32_t)link * 8u;
        int side = hud_side((unsigned)link, vram16(e + 4u));
        if (side && side != 3 && g_enh_centre_nrects + 2 <= ENH_CENTRE_RECTS) {
            uint8_t size = v->vram[(uint16_t)(e + 2u)];
            int x = (vram16(e + 6u) & 0x1FF) - 128, y = (vram16(e) & 0x1FF) - 128;
            int w = ((size >> 2) + 1) * 8, h = ((size & 3) + 1) * 8;
            int pinned = x + hud_move(side, WS_MARGIN) + shift;
            g_enh_centre_rects[g_enh_centre_nrects++] = { x, y, x + w, y + h };
            if (pinned != x) g_enh_centre_rects[g_enh_centre_nrects++] = { pinned, y, pinned + w, y + h };
        } else if (side && side != 3) {
            g_enh.ws.centre_rects_full++;   /* past ENH_CENTRE_RECTS: the centre check would see the HUD */
        }
        link = v->vram[(uint16_t)(e + 3)] & 0x7F;
        if (!link || link >= 80) break;
    }
}

/* ---- per frame ----------------------------------------------------------------------------- */

static int s_renderer_wide;
static Planes s_planes;

extern "C" void ws_power_on(void)
{
    memset(&s_saved, 0, sizeof s_saved);
    memset(&g_ws_view, 0, sizeof g_ws_view);
    s_ext_sync = ~0ull;
    hud_tables();
}

static void play_frame(void);   /* R20's measurement (below ws_act) */

/* Before the frame is drawn: the renderer's margins for this screen, the render camera, the HUD. */
extern "C" void ws_frame_setup(void)
{
    screen_now();
    g_ws_view.gameplay = is_gameplay();
    s_planes = planes_now();
    Layer l = layer_now(s_planes);
    int bl, br;
    /* the render camera of a wide view of this frame: the centre's exceptions need it in the 4:3 run too */
    int shift = shift_for(s_screen_wide ? WS_MARGIN : 0, s_planes, l, &bl, &br);
    centre_exceptions(shift);
    if (!g_opt.widescreen) {
        s_st.valid = s_st.on = s_fire.valid = 0;
        if (s_renderer_wide) {
            gvdp_set_ws_canvas(0);
            gvdp_set_ws_extra(0);
            gvdp_clear_ws_vscroll_columns();
            s_renderer_wide = 0;
        }
        int gp = g_ws_view.gameplay;
        memset(&g_ws_view, 0, sizeof g_ws_view);
        g_ws_view.gameplay = gp;
        g_enh_centre_x0 = -1;
        return;
    }
    g_enh.ws.active = 1;
    play_frame();   /* before the window: the game's VRAM, the view's statics of the last frame unused */
    journal_begin();
    g_ws_window = 1;
    s_renderer_wide = 1;
    gvdp_set_ws_canvas(320 + 2 * WS_MARGIN);
    gvdp_set_ws_bar_black(1);
    int m = s_screen_wide ? WS_MARGIN : 0;
    gvdp_set_ws_extra(m);
    street_now(s_planes, m, &s_saved.street);   /* the street's fold, before the bars judge its picture */
    fire_now(s_planes, m);                      /* its fire's lines past the game's cut (journaled) */
    if (m && !l.valid && g_ws_view.gameplay) {   /* drawn whole: the picture's ends are the map's */
        content_bars(s_planes, m, &bl, &br);
        s_rx_l = -m + bl;
        s_rx_r = 320 + m - br;
    }
    g_ws_view.wide = s_screen_wide;
    g_ws_view.margin = m;
    g_ws_view.shift = shift;
    g_ws_view.bar_l = bl;
    g_ws_view.bar_r = br;
    g_ws_view.centre_x0 = m ? m - shift : WS_MARGIN;
    g_enh_centre_x0 = g_ws_view.centre_x0;
    if (m) stream_margins(s_planes, l);
    shift_vram(m, shift, s_planes);
    bars_keep(m);   /* the HUD's pixels over this frame's bars (the drawn x: after shift_vram) */
}


/* R12, per line after the H-int (the raster rewrites VSRAM between lines): with 2-cell column
   V-scroll the margin columns -3..-1 and 20..22 take the game's columns widened. Every column
   scene's columns are a line across the screen: the lift's bands ($2100: its edges equal, so the
   margins carry the edge value on), the spotted room's shear ($3300, 16x the wobble a column), the
   cat head's sway ($3101: a ramp of 16x the slope a column; its bulge reaches columns 1-18 only)
   and every uniform H-int rewrite (the lift's floor split): the margins continue the line through
   columns 0 and 19, rounded to the pixel (the game's 16.16 ramps round the same way within 1 px). */
static uint16_t s_vs_last[40];
static int s_vs_filled;

extern "C" void ws_line(int line)
{
    (void)line;
    if (!g_opt.widescreen || !g_ws_view.margin) {
        if (s_vs_filled) { gvdp_clear_ws_vscroll_columns(); memset(s_col_set, 0, sizeof s_col_set); s_vs_filled = 0; }
        return;
    }
    reshift_hscroll(s_planes);
    centre_reclaim(s_planes, line);
    const GVDP *v = vdp();
    if (!(v->reg[11] & 4)) return;
    if (s_vs_filled && !memcmp(s_vs_last, v->vsram, sizeof s_vs_last)) return;
    memcpy(s_vs_last, v->vsram, sizeof s_vs_last);
    s_vs_filled = 1;
    static const int plant = getenv("BR_WS_PLANT_VCOL") ? 1 : 0;   /* the control: one margin column left at the clamp */
    for (int k = 1; k <= 3; k++) {
        uint16_t val[2][2];
        for (int pl = 0; pl < 2; pl++) {
            int c0 = v->vsram[pl] & 0x7FF, c19 = v->vsram[38 + pl] & 0x7FF;
            int d = c19 - c0;
            if (d > 0x400) d -= 0x800;
            if (d < -0x400) d += 0x800;
            int step_num = d;   /* per column: d / 19 */
            int lo = c0 - (k * step_num + (step_num >= 0 ? 9 : -9)) / 19;
            int hi = c19 + (k * step_num + (step_num >= 0 ? 9 : -9)) / 19;
            val[0][pl] = (uint16_t)(lo & 0x7FF);
            val[1][pl] = (uint16_t)(hi & 0x7FF);
        }
        int cl = -k, cr = 19 + k;
        if (!(plant && k == 3)) {
            gvdp_set_ws_vscroll_column(cl, val[0][0], val[0][1]);
            s_col_set[cl - GVDP_WS_VCOL_FIRST] = 1;
            s_col_val[cl - GVDP_WS_VCOL_FIRST][0] = val[0][0];
            s_col_val[cl - GVDP_WS_VCOL_FIRST][1] = val[0][1];
        }
        gvdp_set_ws_vscroll_column(cr, val[1][0], val[1][1]);
        s_col_set[cr - GVDP_WS_VCOL_FIRST] = 1;
        s_col_val[cr - GVDP_WS_VCOL_FIRST][0] = val[1][0];
        s_col_val[cr - GVDP_WS_VCOL_FIRST][1] = val[1][1];
    }
}

/* After it: the bars, the measurements on gameplay frames, then VRAM as the game left it. */
extern "C" void ws_frame_check(void)
{
    restream(0);   /* the checks see the margins the view streamed */
    dump_frame();
    if (g_opt.widescreen) {
        int stride, w, h;
        uint32_t *fb = (uint32_t *)nm_framebuffer(&stride, &w, &h);
        bool checked = g_ws_view.gameplay && g_ws_view.margin;
        if (checked) hud_bars_before(g_ws_view.margin, fb, stride, w, h);
        ws_bars_paint(&g_ws_bars, fb, stride, w, h);   /* the bars: black past the map's ends (R16), the HUD over them */
        bool hud_hidden = checked && hud_bars_after(fb, stride, w);
        enh_ws_screen(s_screen_id, s_screen_wide ? "wide" : "4:3");
        if (g_ws_view.gameplay) {
            g_enh.ws.gameplay_frames++;
            int m = g_ws_view.margin;
            if (m) {
                Layer l = layer_now(s_planes);
                check_plane(s_planes, l, 0, m, g_ws_view.shift, g_ws_view.bar_l, g_ws_view.bar_r);
                check_plane(s_planes, l, 1, m, g_ws_view.shift, g_ws_view.bar_l, g_ws_view.bar_r);
                check_whole(s_planes, m, g_ws_view.shift, g_ws_view.bar_l, g_ws_view.bar_r);
                check_street(s_planes, m, g_ws_view.shift);
                if ((g_ws_view.bar_l || g_ws_view.bar_r) && s_rx_r - s_rx_l >= 320 + 2 * m) g_enh.ws.bars_unneeded++;
            }
            check_sprites(m, hud_hidden);
        }
        check_host(g_ws_view.margin, g_ws_view.margin ? g_ws_view.shift : 0, g_ws_view.gameplay);
        shot();
    }
    g_ws_window = 0;
    unshift_vram();
    s_frame++;
}

/* the gameplay mode (R4, widescreen_gameplay=1): objects activate, spawn and die across the wide
   view: 40 on a wide screen, 0 otherwise. Since R20 the windows (Trigger_RunX, the a4 kill windows,
   Op2_Native's kill boxes...) widen by ws_win_left / ws_win_right, the view's own margins, which are
   this mid-level and 80 / 0 at a streamed map's ends (BR_WS_PLAY_OFF bit 2: this again) */
extern "C" int ws_act(void)
{
    if (!g_opt.widescreen || !g_opt.widescreen_gameplay) return 0;
    screen_now();
    return s_screen_wide ? WS_MARGIN : 0;
}

/* ---- R20: the gameplay mode's wide view, for the game's own decisions ------------------------
   With widescreen_gameplay=1 on a wide screen the game places its screen-relative spawns for the
   wide view's edge and keeps the players inside the wide view rather than the 4:3 columns
   (dk_ws.h ws_place_x; 004B74 the area, 0105AE / 002FF6 the edge tests kept with it). The wide view
   is what the frame shows: [left, right) in the 4:3 view's screen x = the render camera's window
   (shift_for: a streamed map's ends move it up to 40 px, so one side shows up to 80) less the bars
   (content_bars: a picture drawn whole that ends inside a margin, as 3101's arena). Computed from
   the machine's state (registers, VRAM, RAM), once per sync interval (every decision of an interval
   gets the first answer), never written anywhere: a branch restored from a snapshot recomputes it
   (ws_saved_put) and decides as the run it came from. The bars' V-scroll takes the margin columns
   at the clamp (s_vs_logic): ws_line's fills are the last drawn frame's, host state. */
static int s_ext_l = 0, s_ext_r = 320;

static void compute_extent(int *l, int *r)
{
    int keep_l = s_rx_l, keep_r = s_rx_r;   /* shift_for's statics belong to the frame being drawn */
    Planes p = planes_now();
    Layer lay = layer_now(p);
    int bl, br;
    int s = shift_for(WS_MARGIN, p, lay, &bl, &br);
    if (!lay.valid && is_gameplay()) {
        s_vs_logic = 1;
        content_bars(p, WS_MARGIN, &bl, &br);
        s_vs_logic = 0;
    }
    s_rx_l = keep_l;
    s_rx_r = keep_r;
    *l = -WS_MARGIN + s + bl;
    *r = 320 + WS_MARGIN + s - br;
}

static void view_extent(int *l, int *r)
{
    *l = 0;
    *r = 320;
    if (!g_opt.widescreen || !g_opt.widescreen_gameplay) return;
    uint64_t k = nm_sync_count();
    if (k != s_ext_sync) {   /* only the game's own calls fill it: a drawn frame never does (play_frame) */
        s_ext_sync = k;
        s_ext_l = 0;
        s_ext_r = 320;
        if (ws_act()) compute_extent(&s_ext_l, &s_ext_r);   /* a wide screen: the a4 kill ops ask per object */
    }
    *l = s_ext_l;
    *r = s_ext_r;
}

/* The controls (BR_WS_PLAY_OFF): bit 0 the spawns placed for 4:3, bit 1 the area kept at 4:3, bit 2
   the windows at WS_ACT's 40 a side (7: the gameplay mode before R20); the measurement still sees
   the wide view. */
static int play_off(void)
{
    static const int v = getenv("BR_WS_PLAY_OFF") ? atoi(getenv("BR_WS_PLAY_OFF")) : 0;
    return v;
}

extern "C" int ws_place_left(void)  { int l, r; view_extent(&l, &r); return (play_off() & 1) ? 0 : -l; }
extern "C" int ws_place_right(void) { int l, r; view_extent(&l, &r); return (play_off() & 1) ? 0 : r - 320; }
extern "C" int ws_area_left(void)   { int l, r; view_extent(&l, &r); return (play_off() & 2) ? 0 : -l; }
extern "C" int ws_area_right(void)  { int l, r; view_extent(&l, &r); return (play_off() & 2) ? 0 : r - 320; }
extern "C" int ws_win_left(void)    { int l, r; view_extent(&l, &r); return (play_off() & 4) ? ws_act() : -l; }
extern "C" int ws_win_right(void)   { int l, r; view_extent(&l, &r); return (play_off() & 4) ? ws_act() : r - 320; }

/* ---- R20's measurement: the placed objects followed, the players' x against the view --------- */

static FILE *s_play_log;

static FILE *play_log(void)
{
    static int opened;
    if (!opened) {
        opened = 1;
        const char *p = getenv("BR_WS_PLAY_LOG");
        if (p && *p) {   /* one file per process, as BR_WS_DUMP */
            char path[600];
            snprintf(path, sizeof path, "%s.%d", p, (int)ws_getpid());
            s_play_log = fopen(path, "w");
            if (s_play_log)
                fprintf(s_play_log, "# place: sync section kind what sx moved obj serial locked\n"
                                    "# end: sync section kind what sx0 moved frames in_view off_max minx maxx lastx ever_in why "
                                    "first_in locked0 camera\n");
        }
    }
    return s_play_log;
}

static uint16_t obj_serial(uint32_t obj) { return m16((obj & 0xFFFFFFu) + 0x56u); }
static int obj_alive(const WsTracked &t)
{
    uint32_t o = t.obj & 0xFFFFFFu;   /* a freed block given to another object keeps no serial of its own */
    return m32(o) == t.handler && m16(o + 4u) != 0 && obj_serial(t.obj) == t.serial;
}

static void play_end(WsTracked &t, const char *why)
{
    EnhWs &w = g_enh.ws;
    w.play_ended++;
    if (!t.ever_in) w.play_never_in++;
    if (FILE *f = play_log())
        fprintf(f, "end %llu %04X %d %06X %d %d %u %u %u %d %d %d %d %s %u %u %d\n", (unsigned long long)nm_sync_count(),
                t.section, t.kind, t.what, t.sx0, t.moved, t.frames, t.in_view, t.off_max, t.minx, t.maxx, t.lastx,
                t.ever_in, why, t.first_in, t.locked0, s16at(RAM_CAMERA_X));
}

extern "C" void ws_note_place(int kind, int sx, int moved, uint32_t obj, uint32_t what)
{
    if (!g_opt.widescreen || !ws_act()) return;
    if (kind == 5) {   /* WS_PLACE_WAVE_WORLD: where a world-placed entry lands, for the log only */
        if (FILE *f = play_log())
            fprintf(f, "world %llu %04X %d %06X %d 0 %06X %04X %d\n", (unsigned long long)nm_sync_count(),
                    m16(RAM_SECTION), kind, what & 0xFFFFFFu, sx, obj & 0xFFFFFFu, obj ? obj_serial(obj) : 0,
                    m32(RAM_LOCK_X) != 0);
        return;
    }
    EnhWs &w = g_enh.ws;
    if (moved) w.placed_moved++;
    else if (sx >= 0 && sx < 320) w.placed_kept++;
    else w.placed_unmoved++;   /* outside 4:3, not moved: the control (BR_WS_PLAY_OFF) */
    int locked = m32(RAM_LOCK_X) != 0;
    if (FILE *f = play_log())
        fprintf(f, "place %llu %04X %d %06X %d %d %06X %04X %d\n", (unsigned long long)nm_sync_count(), m16(RAM_SECTION),
                kind, what & 0xFFFFFFu, sx, moved, obj & 0xFFFFFFu, obj ? obj_serial(obj) : 0, locked);
    if (!obj || (sx >= 0 && sx < 320)) return;   /* followed: the ones placed outside the 4:3 view */
    for (uint32_t i = 0; i < s_saved.ntracked; i++)   /* the same block again: its old object went */
        if (s_saved.tracked[i].obj == obj) { play_end(s_saved.tracked[i], "reused"); s_saved.tracked[i] = s_saved.tracked[--s_saved.ntracked]; break; }
    if (s_saved.ntracked >= WS_TRACK_MAX) { w.play_untracked++; return; }
    WsTracked &t = s_saved.tracked[s_saved.ntracked++];
    memset(&t, 0, sizeof t);
    t.obj = obj;
    t.handler = m32(obj & 0xFFFFFFu);
    t.what = what & 0xFFFFFFu;
    t.serial = obj_serial(obj);
    t.section = m16(RAM_SECTION);
    t.kind = (uint8_t)kind;
    t.sx0 = (int16_t)(sx + moved);
    t.moved = (int16_t)moved;
    t.minx = t.maxx = t.lastx = t.sx0;
    t.locked0 = (uint8_t)locked;
    t.born = (uint32_t)nm_sync_count();
}

/* once a drawn gameplay frame in the gameplay mode: the followed objects' screen x against the wide
   view, and each player in play: in a margin, past a lock, outside the view */
static void play_frame(void)
{
    if (!ws_act() || !is_gameplay()) return;
    int l, r;
    compute_extent(&l, &r);   /* not view_extent: a drawn frame must not decide what the game's cache holds */
    int cam = s16at(RAM_CAMERA_X);
    int locked = m32(RAM_LOCK_X) != 0;
    EnhWs &w = g_enh.ws;
    if (locked) {   /* the arenas: how long each lock holds (a fight that cannot end holds it) */
        w.locked_frames++;
        if (++s_saved.lock_run == 3600) w.lock_long++;   /* a minute */
    } else s_saved.lock_run = 0;
    for (uint32_t i = 0; i < 2; i++) {
        uint32_t rec = 0xFFF650u + 0x40u * i;
        if (!(m8(rec + 0x25) & 1)) continue;
        int sx = s16at(rec + 0x2E) - cam;
        if (sx < l || sx >= r) w.player_offview_frames++;
        else if (sx < 0 || sx >= 320) {
            w.player_margin_frames++;
            if (locked) w.player_margin_locked++;
        }
    }
    for (uint32_t i = 0; i < s_saved.ntracked;) {
        WsTracked &t = s_saved.tracked[i];
        if (!obj_alive(t)) {
            play_end(t, "gone");
            s_saved.tracked[i] = s_saved.tracked[--s_saved.ntracked];
            continue;
        }
        int sx = s16at((t.obj & 0xFFFFFFu) + 0x3Eu) - cam;
        static const uint32_t trace = getenv("BR_WS_PLAY_TRACE") ? (uint32_t)strtoul(getenv("BR_WS_PLAY_TRACE"), nullptr, 16) : 0;
        if (trace && t.what == trace)   /* one spawner's objects frame by frame: screen x, y, the camera, the lock */
            if (FILE *f = play_log())
                fprintf(f, "tr %llu %04X %d %d %d %d %d\n", (unsigned long long)nm_sync_count(), t.serial, sx,
                        s16at((t.obj & 0xFFFFFFu) + 0x42u) - s16at(RAM_CAMERA_X + 4u), cam, locked, t.frames);
        t.frames++;
        t.lastx = (int16_t)sx;
        if (sx < t.minx) t.minx = (int16_t)sx;
        if (sx > t.maxx) t.maxx = (int16_t)sx;
        if (sx >= l && sx < r) {
            if (!t.ever_in) t.first_in = t.frames;
            t.in_view++;
            t.ever_in = 1;
            t.off_run = 0;
        }
        else if (++t.off_run > t.off_max) {
            t.off_max = t.off_run;
            if (t.off_max == 600) w.play_offview_long++;   /* 10 s alive outside the wide view */
        }
        i++;
    }
}

static std::string play_json(void)
{
    const EnhWs &w = g_enh.ws;
    auto u = [](uint64_t v) { return std::to_string((unsigned long long)v); };
    return "\"play\": {\"placed_moved\": " + u(w.placed_moved) + ", \"placed_kept\": " + u(w.placed_kept) +
           ", \"placed_unmoved\": " + u(w.placed_unmoved) + ", \"player_margin_frames\": " + u(w.player_margin_frames) +
           ", \"player_margin_locked\": " + u(w.player_margin_locked) + ", \"player_offview_frames\": " +
           u(w.player_offview_frames) + ", \"followed_ended\": " + u(w.play_ended) + ", \"followed_never_in\": " +
           u(w.play_never_in) + ", \"followed_offview_long\": " + u(w.play_offview_long) + ", \"untracked\": " +
           u(w.play_untracked) + ", \"locked_frames\": " + u(w.locked_frames) + ", \"lock_long\": " + u(w.lock_long) + "}";
}

std::string ws_json_extra(void)
{
    return "\"first_terrain_error\": \"" + s_first_terrain + "\", \"first_sprite_lost\": \"" + s_first_lost +
           "\", \"first_hud_misplaced\": \"" + s_first_hud + "\", \"first_host_misplaced\": \"" + s_first_host +
           "\", \"hud_frames\": {\"panel_p1\": " +
           std::to_string((unsigned long long)g_enh.ws.hud_frames[1]) + ", \"panel_p2\": " +
           std::to_string((unsigned long long)g_enh.ws.hud_frames[2]) + ", \"boss_meter\": " +
           std::to_string((unsigned long long)g_enh.ws.hud_frames[WS_HUD_METER]) + ", \"go_sign\": " +
           std::to_string((unsigned long long)g_enh.ws.hud_frames[WS_HUD_SIGN]) + "}, \"centre_rects_full\": " +
           std::to_string((unsigned long long)g_enh.ws.centre_rects_full) + ", \"margin_cells_given_back\": " +
           std::to_string((unsigned long long)s_reclaimed) + ", \"street\": {\"frames\": " +
           std::to_string((unsigned long long)g_enh.ws.street_frames) + ", \"pixels\": " +
           std::to_string((unsigned long long)g_enh.ws.street_pixels) + ", \"folded\": " +
           std::to_string((unsigned long long)g_enh.ws.street_folded) + ", \"sprite_pixels\": " +
           std::to_string((unsigned long long)g_enh.ws.street_sprite_pixels) + ", \"painted\": " +
           std::to_string((unsigned long long)s_st_painted) + ", \"errors\": " +
           std::to_string((unsigned long long)g_enh.ws.street_errors) + ", \"first_error\": \"" + s_first_street +
           "\", \"fire\": {\"frames\": " + std::to_string((unsigned long long)g_enh.ws.fire_frames) +
           ", \"continued\": " + std::to_string((unsigned long long)g_enh.ws.fire_continued) +
           ", \"lines\": " + std::to_string((unsigned long long)g_enh.ws.fire_lines) +
           ", \"errors\": " + std::to_string((unsigned long long)g_enh.ws.fire_errors) +
           ", \"first_error\": \"" + s_first_fire + "\"}}, " +
           play_json() + ", " + hud_bars_json() + ", " + ws_draw_json();
}
