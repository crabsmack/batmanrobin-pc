/* spr_host.cpp - the host side of the game's sprite display list and table (phase 6, R10, E5):
 * spr_host.h says what calls what. The refused nodes, the host table waiting for the SAT's DMA and
 * the one shown are host state (in branch snapshots); the counters are g_enh.sprites. */
#include <string.h>

extern "C" {
#include "video/genesis_vdp.h"
}

#include "enhance.h"
#include "options.h"
#include "present.h"
#include "spr_host.h"

static_assert(sizeof(SprHostPiece) == sizeof(GVDPHostSat), "SprHostPiece is GVDPHostSat");
static_assert(offsetof(SprHostPiece, attr) == offsetof(GVDPHostSat, attr), "SprHostPiece.attr");
static_assert(offsetof(SprHostPiece, x) == offsetof(GVDPHostSat, x), "SprHostPiece.x");
static_assert(SPR_HOST_PIECES <= GVDP_HOST_SAT_MAX, "the engine's host table");

static struct SprHostState {
    SprHostNode  nodes[SPR_HOST_NODES];   /* this frame's refused nodes, kept (option on) */
    unsigned     nnodes;
    unsigned     refused;                 /* this frame's refusals (the seq) */
    unsigned     lost;                    /* of them, not kept: counted with the frame's table */
    SprHostPiece pending[SPR_HOST_PIECES];   /* Spr_BuildSAT's, until its table's DMA is queued */
    unsigned     npending;
    SprHostPiece shown[SPR_HOST_PIECES];     /* the engine's host table */
    unsigned     nshown;
} s;

/* What the engine draws: the shown table, or widescreen's moved copy of it while a frame is drawn
   (spr_host_set_view). Derived from `s.shown` at every send, so not in the snapshot. */
static SprHostView s_view;
static SprHostPiece s_drawn[SPR_HOST_PIECES], s_extra[SPR_HOST_PIECES], s_in[SPR_HOST_PIECES];
static unsigned s_ndrawn, s_nextra;

static void send(void)
{
    if (!spr_host_on()) return;
    unsigned nx = s_nextra < SPR_HOST_PIECES - s.nshown ? s_nextra : SPR_HOST_PIECES - s.nshown;
    unsigned n = s.nshown + nx;
    const SprHostPiece *in = s.shown;
    if (nx) {   /* the diagnostic pieces after the game's */
        memcpy(s_in, s.shown, s.nshown * sizeof *s.shown);
        memcpy(s_in + s.nshown, s_extra, nx * sizeof *s_extra);
        in = s_in;
    }
    if (s_view && n) s_view(in, s_drawn, n);
    else if (n) memcpy(s_drawn, in, n * sizeof *in);
    s_ndrawn = n;
    gvdp_set_host_sat((const GVDPHostSat *)s_drawn, s_ndrawn);
}

extern "C" unsigned spr_host_set_extra(const SprHostPiece *pieces, unsigned n)
{
    if (n > SPR_HOST_PIECES) n = SPR_HOST_PIECES;
    if (n) memcpy(s_extra, pieces, n * sizeof *pieces);
    s_nextra = n;
    return n < SPR_HOST_PIECES - s.nshown ? n : SPR_HOST_PIECES - s.nshown;
}

extern "C" void spr_host_set_view(SprHostView view)
{
    if (!view && !s_view) return;
    s_view = view;   /* a view set again re-sends too (its frame's shift is new) */
    send();
}

extern "C" unsigned spr_host_drawn(const SprHostPiece **pieces)
{
    *pieces = s_drawn;
    return spr_host_on() ? s_ndrawn : 0;
}

extern "C" void spr_host_redraw(void)
{
    if (spr_host_on()) gvdp_set_host_sat((const GVDPHostSat *)s_drawn, s_ndrawn);
}

/* A frame's drops are counted when its table is built (or its list reset without one), so a
   branch restored at the sync point, which falls between a frame's adds and its table, counts
   that frame's drops whole. */
static void frame_done(unsigned dropped)
{
    g_enh.sprites.list_drops += dropped;
    if (dropped) g_enh.sprites.list_drop_frames++;
    s.lost = 0;
}

extern "C" int spr_host_on(void) { return g_opt.unlimited_sprites != 0; }

extern "C" void spr_host_list_reset(void)
{
    frame_done(s.lost);   /* a frame whose table was never built */
    s.nnodes = 0;
    s.refused = 0;
}

extern "C" void spr_host_list_refused(int kind, uint32_t frame, int16_t x, int16_t y, uint16_t attr, uint16_t key)
{
    uint16_t seq = (uint16_t)s.refused++;
    if (!spr_host_on() || s.nnodes >= SPR_HOST_NODES) {
        s.lost++;
        return;
    }
    SprHostNode &n = s.nodes[s.nnodes++];
    n.frame = frame;
    n.x = x;
    n.y = y;
    n.attr = attr;
    n.key = key;
    n.kind = (uint8_t)kind;
    n.pad = 0;
    n.seq = seq;
    n.owner = hf_spr_current_owner();
}

extern "C" unsigned spr_host_shown(const SprHostPiece **pieces)
{
    *pieces = s.shown;
    return s.nshown;
}

extern "C" unsigned spr_host_nodes(const SprHostNode **nodes)
{
    *nodes = s.nodes;
    return s.nnodes;
}

extern "C" void spr_host_table_built(const SprHostPiece *pieces, unsigned n, unsigned nodes_dropped)
{
    if (n > SPR_HOST_PIECES) n = SPR_HOST_PIECES;   /* the caller counted the rest */
    if (n) memcpy(s.pending, pieces, n * sizeof *pieces);
    s.npending = n;
    frame_done(s.lost + nodes_dropped);
}

extern "C" void spr_host_table_sent(void)
{
    if (!spr_host_on()) return;
    memcpy(s.shown, s.pending, s.npending * sizeof *s.pending);
    s.nshown = s.npending;
    send();
}

extern "C" void spr_host_table_cleared(void)
{
    s.npending = s.nshown = 0;
    send();
}

extern "C" void spr_host_cover(void) { g_enh.sprites.cover_frames++; }

extern "C" size_t spr_host_save(uint8_t *buf, size_t cap)
{
    if (buf && cap >= sizeof s) memcpy(buf, &s, sizeof s);
    return sizeof s;
}

extern "C" void spr_host_load(const uint8_t *buf)
{
    memcpy(&s, buf, sizeof s);
    send();
}

extern "C" void spr_host_power_on(void)
{
    memset(&s, 0, sizeof s);
    s_view = nullptr;
    s_nextra = 0;
    send();
    g_enh.sprites.active = 1;
}
