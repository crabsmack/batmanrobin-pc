/* present.cpp - high_fps (docs/phases/P6-enhance.md R3, E4): the frames between two logic frames.
 *
 * The logic stays the 60 Hz frame-synchronous machine. Runner frame k's picture R_k is drawn at its
 * V-int and shown at k x T on the present clock (enhance.cpp), as the 60 Hz build shows it; the
 * clock's ticks strictly between (k-1) x T and k x T show R_k's video state drawn again with its
 * scroll and its sprites moved back toward R_(k-1) by 1 - alpha. R_k is made before them (the
 * logic of runner frame k is done when they are drawn), so no frame waits for anything: the frames
 * in between only show earlier what R_k shows.
 *
 * The capture (machine.cpp, rendered displays with high_fps on): the video model as the display
 * starts (after the V-int's DMA), then for each line the VSRAM, the registers and the palette it is
 * drawn with (the H-int raster rewrites VSRAM and registers between lines: $2200's split, the
 * 224-line V-scroll tables, the lift's columns). Two captures are kept: R_k's and R_(k-1)'s (the
 * last display of each runner frame).
 *
 * An interpolated frame: a copy of R_k's video model, each line drawn with
 *   - its H-scroll words (full, cell or line mode, planes A and B: the camera and the per-line
 *     parallax tables g_HScrollLinesA/B the V-int DMAs) and its VSRAM words (full, the H-int's
 *     per-line V-scroll from g_HIntTblBack, the 2-cell columns) moved from R_(k-1)'s same line,
 *     modulo the plane's size; a step over 64 px is a cut and snaps, as does a line whose plane
 *     bases, scroll modes or size differ between the two;
 *   - the sprites the display list's walk wrote, each moved by its node's step from R_(k-1) (the
 *     node of the same object; present.h's owner tags); a step over 48 px snaps;
 *   - not moved: the sprites written directly (the HUD panels, the boss meter, GO!, the x 0-15
 *     cover, the stage-1 fire, the stage-3 reveal), unmatched sprites, the palettes, the tiles.
 * VRAM, VSRAM and the registers of the machine are never touched (the copy is drawn and dropped).
 *
 * HF_SELFTEST=1: each interpolated frame is first drawn with nothing moved and compared with R_k
 * (the machine's own picture of the same state): the capture's proof. The count goes to stderr at
 * exit.
 */
#include <atomic>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include <unordered_map>
#include <iterator>

extern "C" {
#include "video/genesis_machine.h"
}
#include "enhance.h"
#include "native_machine.h"
#include "options.h"
#include "present.h"
#include "spr_host.h"
#include "widescreen.h"

extern "C" const uint32_t *nm_palette(void);   /* machine.cpp: the output palette (ARGB) */

#define SNAP_SCROLL 64        /* px: a scroll step this big is a cut, not motion */
#define SNAP_SPRITE 48        /* px */
#define MAX_LINES GVDP_MAX_HEIGHT
#define SAT_N 80
#define HOST_N SPR_HOST_PIECES   /* unlimited_sprites: the engine's host table past the SAT */
#define ALL_N (SAT_N + HOST_N)   /* tags: [0, 80) the SAT's entries, [80, ...) the host table's */
#define WS_COLS (GVDP_WS_VCOL_LAST - GVDP_WS_VCOL_FIRST + 1)

/* ---- the captures ---------------------------------------------------------------------------- */

struct LineRec {
    uint16_t vsram[GVDP_VSRAM_ENTRIES];
    uint8_t  reg[GVDP_NUM_REGISTERS];
    uint16_t pal;             /* index into Cap::pals */
    uint8_t  spr_edge[64];    /* the renderer's carried sprite state before the line (a line drawn */
    uint8_t  spr_mask_carry;  /*   again from here equals R_k's when nothing on it moved) */
    uint16_t hs[2];           /* the H-scroll words (planes A, B) the line reads, as VRAM held them then */
    uint8_t  ws_any;          /* widescreen R12: margin columns filled for this line (ws_line) */
    uint8_t  ws_set[WS_COLS];
    uint16_t ws_val[WS_COLS][2];
};

struct SprTag {               /* one SAT entry the display list's walk wrote */
    uint32_t owner;           /* the object (0: unknown) */
    uint16_t nth;             /* the owner's nth node this frame */
    int16_t  ax, ay;          /* the node's x, y */
    uint8_t  valid;
    uint8_t  why;             /* STEP_*: whether the step below holds */
    int16_t  dx, dy;          /* the node's step from R_(k-1) (px a frame) */
};
enum { STEP_NEW = 0, STEP_OK, STEP_SNAP, STEP_UNOWNED };

struct Cap {
    int      valid;
    uint64_t frame;           /* hf_frame_done count when it was drawn */
    uint16_t section;
    int      gameplay;
    int      lines;
    GVDP     base;
    LineRec  line[MAX_LINES];
    std::vector<uint16_t> cram;           /* each palette's CRAM (64 words), to see a change */
    std::vector<uint32_t> pals;           /* each palette's output colours (256) */
    SprTag   tag[ALL_N];
    int      ws_any;                      /* a line has widescreen margin columns (LineRec::ws_*) */
    WsBars   bars;                        /* widescreen R16: the black bars ws_frame_check paints, the HUD over them */
    struct VDelta { uint16_t line, addr; uint8_t b; };
    std::vector<VDelta> vd;               /* host VRAM writes during the display (hf_display_vram), in order */
    unsigned nhost;                       /* the host table the display drew (unlimited_sprites) */
    SprHostPiece host[HOST_N];
};

static Cap *s_cap[2];
static int s_latest = -1;     /* the last display's capture */
static int s_prev = -1;       /* the last display of the previous runner frame (R_(k-1)) */
static int s_writing = -1;
static uint64_t s_frames;     /* hf_frame_done calls */

static int s_line_next;       /* the display's next line to be captured (hf_display_vram's line) */

static uint8_t ram8(uint32_t a) { return nm_ram_read8(0xFF0000u | (a & 0xFFFFu)); }
static int w16(const uint8_t *vram, uint32_t a);
static uint32_t hs_base(const uint8_t *reg);
static int hs_index(const uint8_t *reg, int line);

extern "C" void hf_display_begin(void)
{
    if (!g_opt.high_fps) { s_writing = -1; return; }
    if (!s_cap[0]) { s_cap[0] = new Cap(); s_cap[1] = new Cap(); }
    s_writing = s_prev == 0 ? 1 : 0;
    Cap &c = *s_cap[s_writing];
    c.valid = 0;
    c.lines = 0;
    memcpy(&c.base, &g_machine.vdp, sizeof c.base);
    c.nhost = 0;
    c.ws_any = 0;
    c.vd.clear();
    s_line_next = 0;
    if (spr_host_on()) {
        const SprHostPiece *hp;
        c.nhost = spr_host_drawn(&hp);   /* as the engine draws it (widescreen: moved by the render camera) */
        if (c.nhost > HOST_N) c.nhost = HOST_N;
        memcpy(c.host, hp, c.nhost * sizeof *hp);
    }
    c.cram.clear();
    c.pals.clear();
}

extern "C" void hf_display_line(int line)
{
    if (s_writing < 0 || line < 0 || line >= MAX_LINES) return;
    Cap &c = *s_cap[s_writing];
    const GVDP *v = &g_machine.vdp;
    LineRec &r = c.line[line];
    memcpy(r.vsram, v->vsram, sizeof r.vsram);
    memcpy(r.reg, v->reg, sizeof r.reg);
    memcpy(r.spr_edge, v->spr_edge, sizeof r.spr_edge);
    r.spr_mask_carry = v->spr_mask_carry;
    uint32_t ha = hs_base(v->reg) + (uint32_t)hs_index(v->reg, line) * 4;
    r.hs[0] = (uint16_t)w16(v->vram, ha);
    r.hs[1] = (uint16_t)w16(v->vram, ha + 2);
    const int *ws_set;
    const uint16_t (*ws_val)[2];
    int nws = ws_margin_columns(&ws_set, &ws_val);
    r.ws_any = 0;
    for (int i = 0; i < WS_COLS; i++) {
        r.ws_set[i] = (uint8_t)(i < nws && ws_set[i] ? 1 : 0);
        r.ws_val[i][0] = r.ws_set[i] ? ws_val[i][0] : 0;
        r.ws_val[i][1] = r.ws_set[i] ? ws_val[i][1] : 0;
        r.ws_any |= r.ws_set[i];
    }
    if (r.ws_any) c.ws_any = 1;
    size_t np = c.cram.size() / GVDP_CRAM_ENTRIES;
    if (!np || memcmp(&c.cram[(np - 1) * GVDP_CRAM_ENTRIES], v->cram, sizeof v->cram)) {
        c.cram.insert(c.cram.end(), v->cram, v->cram + GVDP_CRAM_ENTRIES);
        const uint32_t *p = nm_palette();
        c.pals.insert(c.pals.end(), p, p + GVDP_OUTPUT_PALETTE_SIZE);
        np++;
    }
    r.pal = (uint16_t)(np - 1);
    if (line + 1 > c.lines) c.lines = line + 1;
    s_line_next = line + 1;
}

extern "C" void hf_display_vram(uint16_t addr)
{
    if (s_writing < 0) return;
    s_cap[s_writing]->vd.push_back({ (uint16_t)s_line_next, addr, g_machine.vdp.vram[addr] });
}

/* ---- sprite tags (the game's native branches) ------------------------------------------------ */

/* Obj_RunList announces each object before its code runs (hf_spr_owner); Spr_AddSorted,
   Spr_AddBack and Spr_AddFront name each display-list entry they write (hf_spr_add: the address
   Spr_BuildSAT's walk reads it from), which is then the running object's. The walk names each
   node's entry, its x and y and the SAT index of its first piece (hf_spr_node); hf_spr_walk_end
   closes it. */
#define RAM_SAT_BUF    0x9B56u   /* g_SATBuffer: 80 x 8 bytes, DMA'd to the VDP's table at the V-int */
#define RAM_SPR_OFFX   0x9DD8u   /* g_SprOffsetX */
#define RAM_SPR_OFFY   0x9DDAu   /* g_SprOffsetY */

namespace {
struct Add { uint16_t entry; uint32_t owner; };
struct Node { uint32_t owner; uint16_t nth; int16_t ax, ay; int first; };
}
static uint32_t s_owner;
static uint64_t s_dbg[9];   /* unowned nodes, walks, displays, tagged, gameplay displays, tagged, displays with
                               a host table, its tags held, frames in between moving host sprites (HF_SELFTEST) */
static std::vector<Add> s_adds;
static std::vector<Add> s_adds_walk;       /* the adds the last walk read (hf_spr_walk2 names its entries) */
static std::vector<Node> s_nodes;
static SprTag s_pending[ALL_N];
static int s_pending_from, s_pending_to;   /* the walk's SAT indices [from, to) */
static uint8_t s_pending_sat[SAT_N * 8];   /* g_SATBuffer as the walk left it */
static unsigned s_pending_nhost;           /* the host table hf_spr_walk2 named (unlimited_sprites) */
static SprHostPiece s_pending_host[HOST_N];

static uint16_t ram16(uint32_t a) { return (uint16_t)(ram8(a) << 8 | ram8(a + 1)); }

extern "C" void hf_spr_owner(uint32_t object) { s_owner = object; }
extern "C" uint32_t hf_spr_current_owner(void) { return s_owner; }

extern "C" void hf_spr_add(uint16_t entry)
{
    if (!g_opt.high_fps) return;
    if (s_adds.size() > 4096) s_adds.clear();   /* no walk to close them (a scene without one) */
    s_adds.push_back({ entry, s_owner });
}

static uint32_t owner_in(const std::vector<Add> &adds, uint16_t entry)
{
    for (size_t i = adds.size(); i-- > 0;)
        if (adds[i].entry == entry) return adds[i].owner;
    return 0;
}
static uint32_t owner_of(uint16_t entry) { return owner_in(s_adds, entry); }

extern "C" void hf_spr_node(uint16_t entry, int16_t x, int16_t y, int first_index)
{
    if (!g_opt.high_fps) return;
    if (s_nodes.empty()) s_pending_from = first_index;
    uint32_t owner = owner_of(entry);
    s_dbg[0] += owner == 0;
    uint16_t nth = 0;
    for (const Node &n : s_nodes) nth += n.owner == owner ? 1 : 0;
    s_nodes.push_back({ owner, nth, (int16_t)(x + (int16_t)ram16(RAM_SPR_OFFX)), (int16_t)(y + (int16_t)ram16(RAM_SPR_OFFY)),
                        first_index });
}

extern "C" void hf_spr_walk_end(int count)
{
    if (!g_opt.high_fps) return;
    s_dbg[1]++;
    memset(s_pending, 0, sizeof s_pending);
    s_pending_to = s_nodes.empty() ? 0 : count < SAT_N ? count : SAT_N;
    for (size_t i = 0; i < s_nodes.size(); i++) {
        int to = i + 1 < s_nodes.size() ? s_nodes[i + 1].first : s_pending_to;
        for (int j = s_nodes[i].first; j < to && j < SAT_N; j++)
            if (j >= 0) s_pending[j] = { s_nodes[i].owner, s_nodes[i].nth, s_nodes[i].ax, s_nodes[i].ay, 1 };
    }
    for (int i = 0; i < SAT_N * 8; i++) s_pending_sat[i] = ram8(RAM_SAT_BUF + (uint32_t)i);
    s_pending_nhost = 0;
    s_nodes.clear();
    s_adds_walk.swap(s_adds);
    s_adds.clear();
}

/* unlimited_sprites: Spr_BuildSAT's native second walk (007B72.c native_table) wrote the table in
   the uncapped order and the pieces past it into the host table; its pieces' tags replace the
   original walk's. nodes: the walk's nodes in order (a listed entry's address, or a kept node's
   owner); piece_node: each piece's node, the table's `ntable` from SAT index `first`, then the
   host table's `nhost`. */
extern "C" void hf_spr_walk2(const HfWalkNode *nodes, unsigned nnodes, const uint16_t *piece_node, unsigned first,
                             unsigned ntable, const SprHostPiece *host, unsigned nhost)
{
    if (!g_opt.high_fps) return;
    static std::vector<uint32_t> owner;
    static std::vector<uint16_t> nth;
    owner.resize(nnodes);
    nth.resize(nnodes);
    for (unsigned i = 0; i < nnodes; i++) {
        owner[i] = nodes[i].entry ? owner_in(s_adds_walk, nodes[i].entry) : nodes[i].owner;
        uint16_t n = 0;
        for (unsigned j = 0; j < i; j++) n += owner[j] == owner[i] ? 1 : 0;
        nth[i] = n;
    }
    memset(s_pending, 0, sizeof s_pending);
    if (nhost > HOST_N) nhost = HOST_N;
    for (unsigned j = 0; j < ntable + nhost; j++) {
        unsigned slot = j < ntable ? first + j : SAT_N + (j - ntable);
        if ((j < ntable && slot >= SAT_N) || piece_node[j] >= nnodes) continue;
        const HfWalkNode &n = nodes[piece_node[j]];
        s_pending[slot] = { owner[piece_node[j]], nth[piece_node[j]], n.x, n.y, 1 };
    }
    s_pending_from = (int)first;
    s_pending_to = (int)(first + ntable < SAT_N ? first + ntable : SAT_N);
    for (int i = 0; i < SAT_N * 8; i++) s_pending_sat[i] = ram8(RAM_SAT_BUF + (uint32_t)i);
    s_pending_nhost = nhost;
    if (nhost) memcpy(s_pending_host, host, nhost * sizeof *host);
}

/* Each tagged sprite's step from its object's previous place: the same object's node in R_(k-1)
   (the same place among its nodes, else its nearest), else where the object was last drawn up to
   2 frames back (an object drawn every other frame), per frame; a step over 48 px snaps. Never
   further back: a run drawn from --render-from N-2 interpolates frame N as a run drawn throughout
   does (the latency probe compares the two). */
namespace { struct Seen { int16_t ax, ay; uint64_t frame; }; }
static std::unordered_map<uint64_t, Seen> s_seen;

static void steps(Cap &c)
{
    const Cap *p = s_latest >= 0 && s_cap[s_latest]->valid && s_cap[s_latest]->frame + 1 == c.frame ? s_cap[s_latest]
                 : s_prev >= 0 && s_cap[s_prev]->valid && s_cap[s_prev]->frame + 1 == c.frame ? s_cap[s_prev] : nullptr;
    for (int i = 0; i < ALL_N; i++) {
        SprTag &t = c.tag[i];
        if (!t.valid) continue;
        t.why = STEP_NEW;
        t.dx = t.dy = 0;
        if (!t.owner) { t.why = STEP_UNOWNED; continue; }
        const SprTag *q = nullptr;
        if (p) {
            int best = 1 << 30;
            for (int j = 0; j < ALL_N; j++) {
                const SprTag &u = p->tag[j];
                if (!u.valid || u.owner != t.owner) continue;
                if (u.nth == t.nth) { q = &u; break; }
                int d = abs(t.ax - u.ax) + abs(t.ay - u.ay);
                if (d < best) { best = d; q = &u; }
            }
        }
        int dx = 0, dy = 0, found = 0;
        if (q) { dx = t.ax - q->ax; dy = t.ay - q->ay; found = 1; }
        else {
            auto it = s_seen.find((uint64_t)t.owner << 16 | t.nth);
            if (it != s_seen.end() && it->second.frame < c.frame && c.frame - it->second.frame <= 2) {
                int gap = (int)(c.frame - it->second.frame);
                dx = (t.ax - it->second.ax) / gap;
                dy = (t.ay - it->second.ay) / gap;
                found = 1;
            }
        }
        if (!found) continue;
        t.dx = (int16_t)dx;
        t.dy = (int16_t)dy;
        t.why = dx > SNAP_SPRITE || dx < -SNAP_SPRITE || dy > SNAP_SPRITE || dy < -SNAP_SPRITE ? STEP_SNAP : STEP_OK;
    }
    if (s_seen.size() > 8192)
        for (auto it = s_seen.begin(); it != s_seen.end();) it = c.frame - it->second.frame > 2 ? s_seen.erase(it) : std::next(it);
    for (int i = 0; i < ALL_N; i++)
        if (c.tag[i].valid && c.tag[i].owner) s_seen[(uint64_t)c.tag[i].owner << 16 | c.tag[i].nth] = { c.tag[i].ax, c.tag[i].ay, c.frame };
}

extern "C" void hf_display_end(void)
{
    if (s_writing < 0) return;
    Cap &c = *s_cap[s_writing];
    c.valid = 1;
    c.frame = s_frames;
    c.section = (uint16_t)(ram8(0xF602) << 8 | ram8(0xF603));
    c.gameplay = c.section >= 0x1100u && c.section < 0x5000u && !ram8(0xF703) && !ram8(0xF702);
    /* the walk's tags hold for this display when its table is the one the walk wrote (a lag
       frame's V-int sends none): every tagged entry as the walk left it */
    memset(c.tag, 0, sizeof c.tag);
    uint32_t sb = (uint32_t)(c.base.reg[5] & 0x7F) << 9;
    int same = s_pending_to > 0;
    int skip_x = g_opt.widescreen != 0;   /* widescreen's render camera shifts every x in VRAM for the frame */
    for (int i = s_pending_from; same && i < s_pending_to; i++)
        for (int j = 0; j < 8 && same; j++)   /* the link byte aside: the table's last entry ends the chain */
            same = j == 3 || (skip_x && j >= 6) ||
                   c.base.vram[(sb + (uint32_t)i * 8 + (uint32_t)j) & 0xFFFFu] == s_pending_sat[i * 8 + j];
    s_dbg[2]++;
    s_dbg[3] += same;
    s_dbg[4] += c.gameplay;
    s_dbg[5] += c.gameplay && same;
    if (same) memcpy(c.tag, s_pending, SAT_N * sizeof *c.tag);
    /* the host table's tags (unlimited_sprites): when the display drew the table the walk made */
    if (g_opt.widescreen && (g_ws_bars.bar_l || g_ws_bars.bar_r)) c.bars = g_ws_bars;
    else c.bars.bar_l = c.bars.bar_r = c.bars.nkeep = 0;
    s_dbg[6] += c.nhost != 0;
    int host_same = s_pending_nhost && c.nhost == s_pending_nhost;
    for (unsigned i = 0; host_same && i < c.nhost; i++)   /* the x aside with widescreen, as the table's */
        host_same = c.host[i].y == s_pending_host[i].y && c.host[i].size == s_pending_host[i].size &&
                    c.host[i].attr == s_pending_host[i].attr && (skip_x || c.host[i].x == s_pending_host[i].x);
    if (host_same) {
        memcpy(c.tag + SAT_N, s_pending + SAT_N, c.nhost * sizeof *c.tag);
        s_dbg[7]++;
    }
    steps(c);
    s_latest = s_writing;
    s_writing = -1;
}

extern "C" void hf_invalidate(void)
{
    s_latest = s_prev = s_writing = -1;
    s_adds.clear();   /* the restored frame's list was built in another timeline */
    s_nodes.clear();
    s_seen.clear();
}

extern "C" void hf_frame_done(void)
{
    s_prev = s_latest;
    s_frames++;
}

/* ---- the interpolation ----------------------------------------------------------------------- */

static int w16(const uint8_t *vram, uint32_t a) { return vram[a & 0xFFFFu] << 8 | vram[(a + 1) & 0xFFFFu]; }
static void put16(uint8_t *vram, uint32_t a, int v)
{
    vram[a & 0xFFFFu] = (uint8_t)(v >> 8);
    vram[(a + 1) & 0xFFFFu] = (uint8_t)v;
}

static int plane_px(int code) { return (code & 3) == 1 ? 512 : (code & 3) == 3 ? 1024 : 256; }

/* a wrapped scroll value moved fraction a of the way from p to c (c itself for a cut); *moved is
   set when the result differs from c */
static int lerp_wrap(int p, int c, double a, int mod, int *moved)
{
    int d = (((c - p) % mod) + mod + mod / 2) % mod - mod / 2;
    if (d == 0 || d > SNAP_SCROLL || d < -SNAP_SCROLL) return c;
    int back = (int)((1.0 - a) * d + (d >= 0 ? 0.5 : -0.5));
    if (!back) return c;
    *moved = 1;
    return (((c - back) % mod) + mod) % mod;
}

/* the registers that decide what a line's scroll words mean */
static int same_geometry(const uint8_t *a, const uint8_t *b)
{
    return a[2] == b[2] && a[4] == b[4] && a[11] == b[11] && a[12] == b[12] && a[13] == b[13] && a[16] == b[16] &&
           (a[1] & 0x40) == (b[1] & 0x40);
}

static uint32_t hs_base(const uint8_t *reg) { return (uint32_t)(reg[13] & 0x3F) << 10; }
static int hs_index(const uint8_t *reg, int line)
{
    int m = reg[11] & 3;
    return m == 0 ? 0 : m == 2 ? (line & ~7) : line;
}

static GVDP s_work;
static uint8_t s_idx[GVDP_MAX_WIDTH];
static int s_selftest = -1;
static uint64_t s_self_frames, s_self_bad, s_skip_frames, s_skip_bad;
static uint64_t s_spr_unowned, s_spr_new, s_spr_snap;   /* the walk's sprites not moved, by why */

static void self_report(void)
{
    fprintf(stderr, "[hf] selftest: %llu frames redrawn, %llu differ from R_k; sprites unowned %llu, new %llu, snapped %llu\n",
            (unsigned long long)s_self_frames, (unsigned long long)s_self_bad, (unsigned long long)s_spr_unowned,
            (unsigned long long)s_spr_new, (unsigned long long)s_spr_snap);
    fprintf(stderr, "[hf] selftest: %llu frames in between drawn line by line, %llu differ from the copied-line draw\n",
            (unsigned long long)s_skip_frames, (unsigned long long)s_skip_bad);
    fprintf(stderr, "[hf] tags: unowned nodes %llu, walks %llu, displays %llu (tagged %llu), gameplay %llu (tagged %llu)\n",
            (unsigned long long)s_dbg[0], (unsigned long long)s_dbg[1], (unsigned long long)s_dbg[2],
            (unsigned long long)s_dbg[3], (unsigned long long)s_dbg[4], (unsigned long long)s_dbg[5]);
    fprintf(stderr, "[hf] host table: displays %llu (tagged %llu), frames in between moving host sprites %llu\n",
            (unsigned long long)s_dbg[6], (unsigned long long)s_dbg[7], (unsigned long long)s_dbg[8]);
}

/* The walk's sprites in R_k's table moved back toward R_(k-1) by their node's step (the node of the
   same object, the same nth of its nodes); *n counts the walk's sprites on the chain, *matched those
   moved (or still: a step of 0) from their object's previous place. */

static void move_sprites(const Cap &c, double alpha, GVDP *v, uint64_t *n, uint64_t *matched, uint8_t *dirty)
{
    uint32_t sb = (uint32_t)(c.base.reg[5] & 0x7F) << 9;
    int link = 0;
    for (int count = 0; count < SAT_N; count++) {
        const SprTag &t = c.tag[link];
        if (t.valid) {
            ++*n;
            if (t.why == STEP_UNOWNED) s_spr_unowned += c.gameplay ? 1 : 0;
            else if (t.why == STEP_NEW) s_spr_new += c.gameplay ? 1 : 0;
            else if (t.why == STEP_SNAP) s_spr_snap += c.gameplay ? 1 : 0;
            else {
                ++*matched;
                int bx = (int)((1.0 - alpha) * t.dx + (t.dx >= 0 ? 0.5 : -0.5));
                int by = (int)((1.0 - alpha) * t.dy + (t.dy >= 0 ? 0.5 : -0.5));
                if (bx || by) {
                    uint32_t e = sb + (uint32_t)link * 8;
                    int y = w16(v->vram, e) - by, x = w16(v->vram, e + 6) - bx;
                    int ny = (w16(v->vram, e) & ~0x3FF) | (y & 0x3FF), nx = (w16(v->vram, e + 6) & ~0x3FF) | (x & 0x3FF);
                    put16(v->vram, e, ny);
                    put16(v->vram, e + 6, nx);
                    int hgt = ((c.base.sat[(link * 8 + 2) & 0x3FF] & 3) + 1) * 8;
                    for (int k = 0; k < 2; k++) {
                        int top = ((k ? ny : w16(c.base.vram, e)) & 0x1FF) - 128;
                        for (int yy = top < 0 ? 0 : top; yy < top + hgt && yy < MAX_LINES; yy++) dirty[yy] = 1;
                    }
                    v->sat[(link * 8) & 0x3FF] = (uint8_t)(ny >> 8);
                    v->sat[(link * 8 + 1) & 0x3FF] = (uint8_t)ny;
                }
            }
        }
        int next = c.base.sat[(link * 8 + 3) & 0x3FF] & 0x7F;
        if (next == 0 || next >= SAT_N) break;
        link = next;
    }
}

/* unlimited_sprites: the host table's tagged sprites moved the same way, into s_host_work (drawn
   after the SAT's chain, as the engine draws the host table). Returns whether one moved. */
static GVDPHostSat s_host_work[HOST_N];
static int move_host(const Cap &c, double alpha, uint64_t *n, uint64_t *matched, uint8_t *dirty)
{
    int any = 0;
    memcpy(s_host_work, c.host, c.nhost * sizeof *s_host_work);
    for (unsigned i = 0; i < c.nhost; i++) {
        const SprTag &t = c.tag[SAT_N + i];
        if (!t.valid) continue;
        ++*n;
        if (t.why == STEP_UNOWNED) { s_spr_unowned += c.gameplay ? 1 : 0; continue; }
        if (t.why == STEP_NEW) { s_spr_new += c.gameplay ? 1 : 0; continue; }
        if (t.why == STEP_SNAP) { s_spr_snap += c.gameplay ? 1 : 0; continue; }
        ++*matched;
        int bx = (int)((1.0 - alpha) * t.dx + (t.dx >= 0 ? 0.5 : -0.5));
        int by = (int)((1.0 - alpha) * t.dy + (t.dy >= 0 ? 0.5 : -0.5));
        if (!bx && !by) continue;
        GVDPHostSat &e = s_host_work[i];
        e.y = (uint16_t)((e.y & ~0x3FF) | ((e.y - by) & 0x3FF));
        e.x = (uint16_t)((e.x & ~0x3FF) | ((e.x - bx) & 0x3FF));
        int hgt = ((c.host[i].size & 3) + 1) * 8;
        for (int k = 0; k < 2; k++) {
            int top = ((k ? e.y : c.host[i].y) & 0x1FF) - 128;
            for (int yy = top < 0 ? 0 : top; yy < top + hgt && yy < MAX_LINES; yy++) dirty[yy] = 1;
        }
        any = 1;
    }
    return any;
}

/* Draws capture c with the scroll moved toward p (null: nothing moved) into fb. Returns whether any
   per-line scroll value moved (*uses: the frame's per-line scroll differs from line to line, 1 when
   it also moves from R_(k-1), 2 when it does not). */
static int draw(const Cap &c, const Cap *p, double alpha, uint32_t *fb, int stride, int w, int h, int *uses,
                int *moved_any, int count, const uint32_t *ref, int ref_stride)
{
    memcpy(&s_work, &c.base, sizeof s_work);
    GVDP *v = &s_work;
    static uint8_t dirty[MAX_LINES];
    memset(dirty, 0, sizeof dirty);
    int host_moved = 0;
    if (count) {   /* the counted frame: sprites matched or not (no previous frame: none matched) */
        uint64_t n = 0, m = 0;
        move_sprites(c, alpha, v, &n, &m, dirty);
        if (c.nhost && move_host(c, alpha, &n, &m, dirty)) {
            host_moved = 1;
            s_dbg[8]++;
        }
        if (c.gameplay) {
            g_enh.present.sprites += n;
            g_enh.present.sprites_matched += m;
        }
    }
    /* the host table this capture drew (moved toward R_(k-1) or not): the machine's own, outside the
       display, is the game's unmoved by widescreen's render camera; put back below */
    const int host_set = spr_host_on();
    if (host_set) gvdp_set_host_sat(host_moved ? s_host_work : (const GVDPHostSat *)c.host, c.nhost);
    int dbl = gvdp_interlace_double(v);
    if (dbl) ref = nullptr;   /* interlace: every line drawn */
    int moved_line = 0;
    int in_sync = 1;     /* the work copy's line-to-line sprite state equals R_k's before this line */
    int need_load = 0;   /* lines were copied: load R_k's state before drawing the next */
    int hs0a = -1, hs0b = -1, vs0[GVDP_VSRAM_ENTRIES], varies = 0;
    int moving = 0;      /* a scroll word differs from R_(k-1)'s on a line of the same geometry (a cut aside) */
    size_t di = 0;       /* c.vd: the host's VRAM writes during the display, applied at their lines */
    for (int line = 0; line < c.lines; line++) {
        const LineRec &cl = c.line[line];
        for (; di < c.vd.size() && c.vd[di].line <= line; di++) {
            v->vram[c.vd[di].addr] = c.vd[di].b;
            gvdp_sat_byte(v, c.vd[di].addr, c.vd[di].b);
        }
        memcpy(v->vsram, cl.vsram, sizeof v->vsram);
        memcpy(v->reg, cl.reg, sizeof v->reg);
        int line_moved = 0;
        uint32_t hb = hs_base(cl.reg);
        int hi = hs_index(cl.reg, line);
        int ca = cl.hs[0], cb = cl.hs[1];   /* as the line was drawn (an H-int may rewrite the table) */
        put16(v->vram, hb + (uint32_t)hi * 4, ca);
        put16(v->vram, hb + (uint32_t)hi * 4 + 2, cb);
        int ncols = (cl.reg[11] & 4) ? GVDP_VSRAM_ENTRIES : 2;
        /* does the frame's scroll differ from line to line (or column to column)? */
        if (line == 0) {
            hs0a = ca & 0x3FF;
            hs0b = cb & 0x3FF;
            for (int i = 0; i < GVDP_VSRAM_ENTRIES; i++) vs0[i] = cl.vsram[i] & 0x3FF;
            if (ncols > 2)
                for (int i = 2; i < ncols; i++) varies |= (cl.vsram[i] & 0x3FF) != vs0[i & 1];
        } else {
            varies |= (ca & 0x3FF) != hs0a || (cb & 0x3FF) != hs0b;
            for (int i = 0; i < ncols; i++) varies |= (cl.vsram[i] & 0x3FF) != vs0[i];
        }
        if (p && line < p->lines && same_geometry(cl.reg, p->line[line].reg)) {
            const LineRec &pl = p->line[line];
            int wpx = plane_px(cl.reg[16]), hpx = plane_px(cl.reg[16] >> 4);
            int m = 0;
            int pa = pl.hs[0], pbv = pl.hs[1];
            int na = lerp_wrap(pa & 0x3FF, ca & 0x3FF, alpha, wpx, &m);
            int nb = lerp_wrap(pbv & 0x3FF, cb & 0x3FF, alpha, wpx, &m);
            put16(v->vram, hb + (uint32_t)hi * 4, m ? na : ca);
            put16(v->vram, hb + (uint32_t)hi * 4 + 2, m ? nb : cb);
            int mv = 0;
            for (int i = 0; i < ncols; i++) {
                int x = lerp_wrap(pl.vsram[i] & 0x3FF, cl.vsram[i] & 0x3FF, alpha, hpx, &mv);
                if (x != (cl.vsram[i] & 0x3FF)) v->vsram[i] = (uint16_t)x;
                moving |= (pl.vsram[i] & 0x3FF) != (cl.vsram[i] & 0x3FF);
            }
            moving |= (pa & 0x3FF) != (ca & 0x3FF) || (pbv & 0x3FF) != (cb & 0x3FF);
            moved_line |= m | mv;
            line_moved = m | mv;
        }
        if (c.ws_any) {   /* widescreen R12: the line's margin columns, moved as its VSRAM columns are */
            const LineRec *pl = p && line < p->lines && same_geometry(cl.reg, p->line[line].reg) ? &p->line[line] : nullptr;
            int hpx = plane_px(cl.reg[16] >> 4), mw = 0;
            for (int i = 0; i < WS_COLS; i++) {
                if (!cl.ws_set[i]) { gvdp_clear_ws_vscroll_column(GVDP_WS_VCOL_FIRST + i); continue; }
                uint16_t ab[2] = { cl.ws_val[i][0], cl.ws_val[i][1] };
                for (int k = 0; pl && pl->ws_set[i] && k < 2; k++) {
                    int mk = 0;
                    int x = lerp_wrap(pl->ws_val[i][k] & 0x3FF, ab[k] & 0x3FF, alpha, hpx, &mk);
                    if (mk) ab[k] = (uint16_t)x;
                    mw |= mk;
                }
                gvdp_set_ws_vscroll_column(GVDP_WS_VCOL_FIRST + i, ab[0], ab[1]);
            }
            moved_line |= mw;
            line_moved |= mw;
        }
        if (ref && in_sync && !line_moved && !dirty[line] && line < h) {
            /* nothing on this line moved and it starts from R_k's state: R_k's line */
            memcpy(fb + (size_t)line * (size_t)stride, ref + (size_t)line * (size_t)ref_stride, (size_t)w * 4);
            need_load = 1;
            continue;
        }
        if (need_load) {
            memcpy(v->spr_edge, cl.spr_edge, sizeof v->spr_edge);
            v->spr_mask_carry = cl.spr_mask_carry;
            need_load = 0;
        }
        const uint32_t *pal = &c.pals[(size_t)cl.pal * GVDP_OUTPUT_PALETTE_SIZE];
        for (int sub = 0; sub <= dbl; sub++) {
            int row = (line << dbl) + sub;
            int n = gvdp_render_scanline(v, row, s_idx);
            if (row >= h) continue;
            uint32_t *out = fb + (size_t)row * (size_t)stride;
            int ww = n < w ? n : w;
            for (int x = 0; x < ww; x++) out[x] = pal[s_idx[x]];
        }
        /* the next line may be copied only when the state carried out of this one is R_k's */
        if (ref && line + 1 < c.lines)
            in_sync = !memcmp(v->spr_edge, c.line[line + 1].spr_edge, sizeof v->spr_edge) &&
                      v->spr_mask_carry == c.line[line + 1].spr_mask_carry;
    }
    if (host_set) spr_host_redraw();
    if (c.bars.bar_l || c.bars.bar_r)   /* widescreen R16: the bars past the map's ends with the pinned HUD over
                                           them, as ws_frame_check paints R_k's (the HUD is not interpolated) */
        ws_bars_paint(&c.bars, fb, stride, w, h);
    if (c.ws_any && c.lines > 0) {   /* the margin columns as the machine's last line left them */
        const LineRec &last = c.line[c.lines - 1];
        for (int i = 0; i < WS_COLS; i++) {
            if (last.ws_set[i]) gvdp_set_ws_vscroll_column(GVDP_WS_VCOL_FIRST + i, last.ws_val[i][0], last.ws_val[i][1]);
            else gvdp_clear_ws_vscroll_column(GVDP_WS_VCOL_FIRST + i);
        }
    }
    /* counted (enh_scroll_frame): per-line scroll with something to interpolate. A frame whose
       per-line words all equal R_(k-1)'s (a locked camera over a fixed raster: the Freeze fight's
       cone spans with no cone drawn) or differ only across a cut shows the same picture either way.
       *uses: 1 per-line scroll that moves (counted), 2 per-line scroll with no motion (reported only:
       enh_scroll_static), 0 none. */
    *uses = varies ? (moving ? 1 : 2) : 0;
    *moved_any = moved_line;
    return moved_line;
}

static std::vector<uint32_t> s_check;

/* HF_DUMP=DIR,FROM,TO: runner frames FROM..TO's frames in between (and R_k, R_(k-1)) as PPM files,
   for looking at what the interpolation did */
static void ppm(const char *path, const uint32_t *fb, int stride, int w, int h)
{
    FILE *f = fopen(path, "wb");
    if (!f) return;
    fprintf(f, "P6\n%d %d\n255\n", w, h);
    std::vector<uint8_t> row((size_t)w * 3);
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            uint32_t c = fb[(size_t)y * stride + x];
            row[x * 3] = (uint8_t)(c >> 16); row[x * 3 + 1] = (uint8_t)(c >> 8); row[x * 3 + 2] = (uint8_t)c;
        }
        fwrite(row.data(), 1, row.size(), f);
    }
    fclose(f);
}

static void dump(uint64_t k, double alpha, const uint32_t *fb, int stride, int w, int h)
{
    static int init;
    static char dir[400];
    static unsigned long long from, to;
    if (!init) {
        init = 1;
        const char *e = getenv("HF_DUMP");
        if (e && sscanf(e, "%399[^,],%llu,%llu", dir, &from, &to) != 3) dir[0] = 0;
        if (!e) dir[0] = 0;
    }
    if (!dir[0] || k < from || k > to) return;
    char path[512];
    snprintf(path, sizeof path, "%s/f%06llu_%03d.ppm", dir, (unsigned long long)k, (int)(alpha * 1000));
    ppm(path, fb, stride, w, h);
    int sw, fw, fh;
    const uint32_t *ref = nm_framebuffer(&sw, &fw, &fh);
    snprintf(path, sizeof path, "%s/f%06llu_whole.ppm", dir, (unsigned long long)k);
    ppm(path, ref, sw, fw, fh);
}

extern "C" int hf_interpolate(uint64_t k, double alpha, uint32_t *fb, int stride, int w, int h)
{
    (void)k;
    if (s_latest < 0 || s_latest == s_prev || !s_cap[s_latest]->valid) return 0;   /* no new picture: R_(k-1) */
    const Cap &c = *s_cap[s_latest];
    const Cap *p = s_prev >= 0 && s_cap[s_prev]->valid && s_cap[s_prev]->frame + 1 == c.frame ? s_cap[s_prev] : nullptr;
    if (s_selftest < 0) {
        const char *e = getenv("HF_SELFTEST");
        s_selftest = e && *e == '1';
        if (s_selftest) atexit(self_report);
    }
    int uses = 0, moved = 0;
    if (s_selftest) {
        int sw, fw, fh;
        const uint32_t *ref = nm_framebuffer(&sw, &fw, &fh);
        if ((int)s_check.size() < stride * h) s_check.resize((size_t)stride * (size_t)h);
        draw(c, nullptr, 1.0, s_check.data(), stride, w, h, &uses, &moved, 0, nullptr, 0);
        int bad = fw != w || fh != h, bad_y = -1;
        for (int y = 0; y < h && !bad; y++)
            if ((bad = memcmp(s_check.data() + (size_t)y * stride, ref + (size_t)y * sw, (size_t)w * 4) != 0)) bad_y = y;
        s_self_frames++;
        s_self_bad += bad ? 1u : 0u;
        if (bad && s_self_bad <= 20) {
            int x = 0;
            while (bad_y >= 0 && x < w && s_check[(size_t)bad_y * stride + x] == ref[(size_t)bad_y * sw + x]) x++;
            fprintf(stderr, "[hf] selftest: frame %llu (section %04X) differs from R_k first at line %d x %d\n",
                    (unsigned long long)k, c.section, bad_y, x);
        }
    }
    int rs, rw, rh;
    const uint32_t *ref = nm_framebuffer(&rs, &rw, &rh);   /* R_k: the lines nothing moved on */
    if (rw != w || rh != h) ref = nullptr;
    draw(c, p, alpha, fb, stride, w, h, &uses, &moved, 1, ref, rs);
    if (s_selftest && ref) {   /* the copied lines' proof: the same frame drawn line by line */
        if ((int)s_check.size() < stride * h) s_check.resize((size_t)stride * (size_t)h);
        uint64_t keep_n = g_enh.present.sprites, keep_m = g_enh.present.sprites_matched;
        uint64_t keep[4] = { s_spr_unowned, s_spr_new, s_spr_snap, s_dbg[8] };
        int u2, m2;
        draw(c, p, alpha, s_check.data(), stride, w, h, &u2, &m2, 1, nullptr, 0);
        g_enh.present.sprites = keep_n;
        g_enh.present.sprites_matched = keep_m;
        s_spr_unowned = keep[0], s_spr_new = keep[1], s_spr_snap = keep[2], s_dbg[8] = keep[3];
        int bad = 0;
        for (int y = 0; y < h && !bad; y++)
            bad = memcmp(s_check.data() + (size_t)y * stride, fb + (size_t)y * stride, (size_t)w * 4) != 0;
        s_skip_frames++;
        s_skip_bad += bad ? 1u : 0u;
    }
    if (uses == 1) enh_scroll_frame(c.section, moved);
    else if (uses == 2) enh_scroll_static(c.section);
    dump(k, alpha, fb, stride, w, h);
    /* E4's negative control (HF_PLANT_VDP=1): the interpolated draw writes into the machine's VDP;
       the corpus's sync rows must then differ from stepped-gen's */
    static int plant = -1;
    if (plant < 0) { const char *e = getenv("HF_PLANT_VDP"); plant = e && *e == '1'; }
    if (plant) g_machine.vdp.vram[0xFFFF]++;
    return 1;
}

extern "C" void hf_init(void)
{
    enh_set_interpolator(hf_interpolate);
}
