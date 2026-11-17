/* ws_draw.cpp - the widescreen view's margin draws (phase 6, R11, E3): ws_draw.h says what calls
 * what. The culled draws of the frame being built, the margin pieces waiting for the SAT's DMA and
 * the ones shown are host state (in branch snapshots); the counters are reported with the ws group.
 * Nothing here writes the game's memory or the VDP: the host sprite layer is the engine's scratch
 * line, filled only where no SAT sprite drew and only outside the 4:3 view's columns. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#ifdef _WIN32
#include <process.h>
#define wsd_getpid _getpid
#else
#include <unistd.h>
#define wsd_getpid getpid
#endif

extern "C" {
#include "video/genesis_machine.h"
#include "video/genesis_vdp.h"
}
#include "native_machine.h"
#include "options.h"
#include "widescreen.h"
#include "ws_draw.h"

static uint8_t m8(uint32_t a)
{
    a &= 0xFFFFFFu;
    if (a < 0x400000u) return g_rom[a];
    if (a >= 0xFF0000u) return nm_ram_read8(a);
    return 0;
}

static struct WsDrawState {
    SprHostNode nodes[WS_DRAW_NODES];      /* this frame's culled draws (Op2_DrawInScreenBox) */
    uint32_t    owner[WS_DRAW_NODES];      /* the object each was read from (the measurement's) */
    unsigned    nnodes;
    WsDrawPiece pending[WS_DRAW_PIECES];   /* Spr_BuildSAT's, until its table's DMA is queued */
    unsigned    npending;
    unsigned    walk_end;                  /* pending[0, walk_end): the last walk's (and the direct ones before it) */
    WsDrawPiece shown[WS_DRAW_PIECES];     /* drawn with every frame until the next table */
    unsigned    nshown;
    uint8_t     hud_pending[80];           /* the SAT buffer's entries a HUD writer tagged (WS_HUD_*) */
    uint8_t     hud_shown[80];             /* the shown table's */
    int         hud_direct;                /* the Spr_DrawDirect in progress draws this HUD item */
} s;

static struct WsDrawCounts {
    uint64_t nodes;          /* culled draws read (nodes kept for the walk) */
    uint64_t nodes_lost;     /* past WS_DRAW_NODES a frame */
    uint64_t unsupported;    /* culled draws whose draw list holds an op this code does not read */
    uint64_t pieces;         /* margin pieces built (from the clip and from culled draws) */
    uint64_t pieces_lost;    /* past WS_DRAW_PIECES a frame */
    uint64_t pixels;         /* margin pixels the host layer drew (wide gameplay frames and others) */
    uint64_t frames;         /* drawn frames with at least one margin pixel */
} c;
static int s_frame_pixels;

/* controls (diagnosing): BR_WS_DRAW_OFF=1 no margin draws (the view as before R11); BR_WS_DRAW_PLANT=1
   margin pieces moved 48 px into the 4:3 view and drawn there too (the centre check must catch it) */
static const bool s_off = getenv("BR_WS_DRAW_OFF") && *getenv("BR_WS_DRAW_OFF") == '1';
static const bool s_plant = getenv("BR_WS_DRAW_PLANT") && *getenv("BR_WS_DRAW_PLANT") == '1';

extern "C" int ws_draw_on(void) { return g_opt.widescreen != 0 && !s_off; }
/* the 400-px view is 40 px per side, and the render camera moves it up to 40 more one way */
extern "C" int ws_draw_reach(void) { return 2 * WS_MARGIN; }

/* Spr_ResetList: the display list and the SAT buffer start again (g_SATPtr = g_SATBuffer) */
extern "C" void ws_draw_list_reset(void)
{
    s.nnodes = 0;
    memset(s.hud_pending, 0, sizeof s.hud_pending);
}

/* ---- the HUD items written straight into the SAT buffer (ws_draw.h) ------------------------- */

#define SAT_BUFFER 0x9B56u   /* g_SATBuffer's RAM word address (80 x 8 bytes) */

extern "C" void ws_hud_direct_begin(int kind) { s.hud_direct = kind; }
extern "C" int ws_hud_direct_kind(void) { return s.hud_direct; }
extern "C" void ws_hud_direct_end(void) { s.hud_direct = 0; }

extern "C" void ws_hud_tag(uint16_t sat_from, uint16_t sat_to, int kind)
{
    for (unsigned a = sat_from; a + 8u <= sat_to; a += 8u) {
        if (a < SAT_BUFFER) continue;
        unsigned n = (a - SAT_BUFFER) / 8u;
        if (n < 80) s.hud_pending[n] = (uint8_t)kind;
    }
}

extern "C" int ws_hud_shown_kind(unsigned entry) { return entry < 80 ? s.hud_shown[entry] : 0; }

extern "C" void ws_draw_node(int kind, uint32_t frame, int16_t x, int16_t y, uint16_t attr, uint16_t key,
                             uint32_t owner)
{
    if (s.nnodes >= WS_DRAW_NODES) {
        c.nodes_lost++;
        return;
    }
    s.owner[s.nnodes] = owner;
    SprHostNode &n = s.nodes[s.nnodes];
    n.frame = frame;
    n.x = x;
    n.y = y;
    n.attr = attr;
    n.key = key;
    n.kind = (uint8_t)kind;
    n.pad = 1;                              /* a culled draw (spr_host's own nodes have 0) */
    n.seq = (uint16_t)s.nnodes++;
    c.nodes++;
}

extern "C" unsigned ws_draw_nodes(const SprHostNode **nodes)
{
    *nodes = s.nodes;
    return s.nnodes;
}

extern "C" void ws_draw_unsupported(void) { c.unsupported++; }

/* ---- the measurement (BR_WS_DRAW_LOG=DIR: one text file per process) -------------------------
   The kickoff's method on the native runner: every gameplay frame, each object slot that holds a
   new object ("N frame a6 handler serial pc x y section"); every 4th gameplay frame a sample: "S
   frame section camx camy", each object ("O a6 serial x y attr slot..."), each display node the
   walk drew ("D kind owner frame x y resident table_pieces margin_pieces x0 x1 y0 y1" in screen
   coordinates: kind L = the game's list, C = a culled draw read by Op2_DrawInScreenBox's branch). */
static FILE *s_log;
static uint64_t s_gp_frames;
static int s_sample;
static uint32_t s_slot_key[0x10000 / 2];   /* per a6 (even RAM word): handler ^ serial << 16 + 1 */

static uint16_t m16(uint32_t a) { return (uint16_t)(m8(a) << 8 | m8(a + 1)); }
static uint32_t m32(uint32_t a) { return (uint32_t)m16(a) << 16 | m16(a + 2); }

static void log_frame(void)
{
    static int opened;
    if (!opened) {
        opened = 1;
        const char *dir = getenv("BR_WS_DRAW_LOG");
        if (dir && *dir) {
            char path[700];
            snprintf(path, sizeof path, "%s/wsdraw_%d_%p.txt", dir, (int)wsd_getpid(), (void *)&s);
            s_log = fopen(path, "w");
        }
    }
    s_sample = 0;
    if (!s_log) return;
    uint16_t sec = m16(0xFFF602u);
    int gameplay = sec >= 0x1100 && sec <= 0x4FFF && m8(0xFFF703u) == 0 && m8(0xFFF702u) == 0;
    if (!gameplay) return;
    uint64_t f = s_gp_frames++;
    int camx = (int16_t)m16(0xFFDFC4u), camy = (int16_t)m16(0xFFDFC8u);
    s_sample = (f % 4) == 0;
    if (s_sample) {
        fflush(s_log);
        fprintf(s_log, "S %llu %04X %d %d\n", (unsigned long long)f, sec, camx, camy);
    }
    uint32_t a = m16(0xFFDE8Cu + 6u);
    for (int n = 0; a != 0xDE94u && n < 200 && a < 0xFFF0u; n++) {
        uint32_t a6 = 0xFF0000u | a;
        uint32_t h = m32(a6);
        uint16_t ser = m16(a6 + 0x56u);
        int x = (int16_t)m16(a6 + 0x3Eu), y = (int16_t)m16(a6 + 0x42u);
        uint32_t key = (h ^ ((uint32_t)ser << 16)) + 1u;
        if (h == 0x7D2u) {
            uint32_t a4 = m16(a6 + 0xAu), a5 = 0xFF0000u | m16(a6 + 0xCu);
            if (s_slot_key[(a >> 1) & 0x7FFF] != key)
                fprintf(s_log, "N %llu %04X %X %04X %06X %d %d %04X\n", (unsigned long long)f, a, h, ser,
                        m32(0xFF0000u | (a4 + 0x2Au)) & 0xFFFFFFu, x - camx, y - camy, sec);
            if (s_sample) {
                fprintf(s_log, "O %04X %04X %d %d %04X", a, ser, x, y, m16(a5 + 0x38u));
                unsigned cnt = m16(a5 + 0x14u);
                for (unsigned i = 0; i < cnt && i < 8; i++) fprintf(s_log, " %04X", m16(a5 + 2u * i));
                fputc('\n', s_log);
            }
        }
        s_slot_key[(a >> 1) & 0x7FFF] = key;
        a = m16(a6 + 6u);
    }
}

extern "C" int ws_draw_logging(void) { return s_sample; }

extern "C" void ws_draw_log_node(const SprHostNode *node, uint32_t frame, int16_t x, int16_t y, int resident,
                                 int table, int margin, int x0, int x1, int y0, int y1)
{
    if (!s_log || !s_sample) return;
    int culled = node && node >= s.nodes && node < s.nodes + WS_DRAW_NODES;
    uint32_t owner = culled ? s.owner[node - s.nodes] & 0xFFFFu : 0;
    fprintf(s_log, "D %c %04X %08X %d %d %d %d %d %d %d %d %d\n", culled ? 'C' : 'L', owner, frame, x, y, resident,
            table, margin, x0, x1, y0, y1);
}

extern "C" void ws_draw_begin(void)
{
    /* a new table: the pieces of a walk whose table was never sent go; the direct ones drawn since
       (Spr_EmitClipped, before this walk in the frame) stay, in front of the walk's */
    memmove(s.pending, s.pending + s.walk_end, (s.npending - s.walk_end) * sizeof *s.pending);
    s.npending -= s.walk_end;
    s.walk_end = 0;
    log_frame();
}

extern "C" void ws_draw_piece(const WsDrawPiece *p)
{
    if (s.npending >= WS_DRAW_PIECES) {
        c.pieces_lost++;
        return;
    }
    s.pending[s.npending++] = *p;
    c.pieces++;
}

extern "C" void ws_draw_sent(void)
{
    memcpy(s.shown, s.pending, s.npending * sizeof *s.pending);
    s.nshown = s.npending;
    s.npending = s.walk_end = 0;
    memcpy(s.hud_shown, s.hud_pending, sizeof s.hud_shown);   /* the HUD tags go with the table */
}

extern "C" void ws_draw_walk_end(void) { s.walk_end = s.npending; }

extern "C" void ws_draw_cleared(void)
{
    s.npending = s.nshown = s.walk_end = 0;
    memset(s.hud_shown, 0, sizeof s.hud_shown);
}

/* ---- the host sprite layer: each line of a wide frame ---------------------------------------- */

static void draw_pieces(const GVDP *v, const GVDPSpriteLayer *layer)
{
    int line = layer->line;
    if (line == 0) {
        if (s_frame_pixels) c.frames++;
        s_frame_pixels = 0;
    }
    if (!g_opt.widescreen || !g_ws_view.margin || !s.nshown) return;
    if (v->reg[12] & 0x06) return;          /* interlace mode 2: not a gameplay screen here */
    int total = layer->total;
    int shift = g_ws_view.shift;
    int c0 = layer->offset - shift, c1 = c0 + 320;   /* the 4:3 view's columns: never drawn */
    if (s_plant) c0 = c1 = -1;
    int sh_mode = (v->reg[12] & 0x08) != 0;
    for (unsigned i = 0; i < s.nshown; i++) {
        const WsDrawPiece &p = s.shown[i];
        int hc = (p.size & 3) + 1, wc = ((p.size >> 2) & 3) + 1;
        int dy = line - p.y;
        if (dy < 0 || dy >= hc * 8) continue;
        int x0 = p.x - shift + layer->offset;
        if (s_plant) x0 += p.x >= 160 ? -48 : 48;   /* the control: 48 px into the 4:3 view */
        if (x0 >= total || x0 + wc * 8 <= 0) continue;
        if (x0 >= c0 && x0 + wc * 8 <= c1) continue;
        int hf = (p.attr & 0x0800) != 0, vf = (p.attr & 0x1000) != 0;
        int pal = (p.attr >> 13) & 3, pri = (p.attr & 0x8000) != 0;
        int iy = vf ? hc * 8 - 1 - dy : dy;
        int celly = iy >> 3, fy = iy & 7;
        for (int lx = 0; lx < wc * 8; lx++) {
            int ox = x0 + lx;
            if (ox < 0 || ox >= total || (ox >= c0 && ox < c1) || layer->opaque[ox]) continue;
            int ix = hf ? wc * 8 - 1 - lx : lx;
            int t = (ix >> 3) * hc + celly, fx = ix & 7;
            uint8_t b;
            if (p.from_mem)
                b = m8(p.src + (uint32_t)t * 32u + (uint32_t)fy * 4u + (uint32_t)(fx >> 1));
            else
                b = v->vram[(uint16_t)((((p.attr & 0x7FF) + t) & 0x7FF) * 32 + fy * 4 + (fx >> 1))];
            int nib = (fx & 1) ? (b & 0x0F) : (b >> 4);
            if (!nib) continue;
            if (sh_mode && pal == 3 && nib >= 14) continue;   /* operators: not drawn as colours */
            layer->index[ox] = (uint8_t)(pal * 16 + nib);
            layer->opaque[ox] = 1;
            layer->high[ox] = (uint8_t)pri;
            s_frame_pixels++;
            c.pixels++;
        }
    }
}

/* the margin pieces, then the stage 1 street's folded plane B (widescreen.cpp ws_street_line: behind
   every sprite, so after them) */
static void draw_line(void *user, const GVDP *v, const GVDPSpriteLayer *layer)
{
    (void)user;
    draw_pieces(v, layer);
    ws_street_line(v, layer);
}

/* ---- branch snapshots, power-on ------------------------------------------------------------ */

extern "C" size_t ws_draw_save(uint8_t *buf, size_t cap)
{
    if (buf && cap >= sizeof s) memcpy(buf, &s, sizeof s);
    return sizeof s;
}

extern "C" void ws_draw_load(const uint8_t *buf) { memcpy(&s, buf, sizeof s); }

extern "C" void ws_draw_power_on(void)
{
    memset(&s, 0, sizeof s);
    s_frame_pixels = 0;
    gvdp_set_host_sprites(g_opt.widescreen ? draw_line : nullptr, nullptr);
}

static std::string u64s(uint64_t v) { return std::to_string((unsigned long long)v); }

std::string ws_draw_json(void)
{
    return "\"margin_draws\": {\"culled_nodes\": " + u64s(c.nodes) + ", \"nodes_lost\": " + u64s(c.nodes_lost) +
           ", \"unsupported\": " + u64s(c.unsupported) + ", \"pieces\": " + u64s(c.pieces) +
           ", \"pieces_lost\": " + u64s(c.pieces_lost) + ", \"pixels\": " + u64s(c.pixels) +
           ", \"frames\": " + u64s(c.frames) + "}";
}
