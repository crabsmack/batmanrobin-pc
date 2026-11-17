#include "game.h"

#if DK_MODE_NATIVE
#include "native/present.h"   /* high_fps's sprite tags (phase 6 R3) */
#endif

/* Spr_BuildSAT ($007B72-$007D5D).
 *
 * The original has four copies of the piece loop (plain, H flip, V flip, both: the flips only
 * pick which half of each offset long is used) and keeps everything in registers, which the
 * measured contract hands back to the caller. The C runs one loop on locals named for those
 * registers. */

static uint32_t sext16(uint16_t w) { return (uint32_t)(int32_t)(int16_t)w; }
static uint16_t lo(uint32_t r) { return (uint16_t)r; }
static void set_lo(uint32_t *r, uint16_t v) { *r = (*r & 0xFFFF0000u) | v; }
static uint32_t swap_halves(uint32_t v) { return (v << 16) | (v >> 16); }

template <class T> static T *at(uint32_t a)
{
    PTR(T) p;
    p.v = a;
    return p;
}

static constexpr uint16_t SAT_MAX_Y = 0x100;   /* pieces at y/x >= these after the offset are dropped */
static constexpr uint16_t SAT_MAX_X = 0x160;
static constexpr uint16_t SAT_BIAS = 0x60;     /* then the VDP's +$80 minus the 32-pixel margin */
static constexpr uint16_t FRONT_LIST_EMPTY = 0x9E16;   /* g_SprFrontEnd with no front entry */
static constexpr uint32_t FRONT_LIST_FIRST = 0xFFFF9E04u;   /* the first front node */

/* the registers the walk keeps */
struct SatRegs {
    uint32_t d0, d1, d2, d3, d4, d5, d6, d7;
    uint32_t a1, a2;
};

/* one frame's pieces (d7 + 1 of them from `piece`), clipped to the screen; true when the 80th
 * sprite was written (the dbeq's exit) */
static bool emit_pieces(SatRegs &r, const SprPiece *piece)
{
    bool hflip = (r.d6 & SPR_ATTR_HFLIP) != 0;
    bool vflip = (r.d6 & SPR_ATTR_VFLIP) != 0;
    for (;;) {
        const SprPiece *p = piece++;
        r.d0 = ((uint32_t)(uint16_t)p->y_vflip << 16) | (uint16_t)p->y;
        r.d1 = ((uint32_t)(uint16_t)p->x_hflip << 16) | (uint16_t)p->x;
        set_lo(&r.d2, (uint16_t)((uint16_t)(int16_t)(int8_t)(uint8_t)r.d2 | p->size));   /* ext.w / or.w */
        r.d3 = (r.d3 & ~0xFFu) | p->tile_slot;
        r.d6 = (r.d6 & ~0xFFu) | p->tile_add;
        if (vflip)
            r.d0 = swap_halves(r.d0);
        if (hflip)
            r.d1 = swap_halves(r.d1);
        set_lo(&r.d0, (uint16_t)(lo(r.d0) + lo(r.d5)));
        if (lo(r.d0) < SAT_MAX_Y) {
            set_lo(&r.d1, (uint16_t)(lo(r.d1) + lo(r.d4)));
            if (lo(r.d1) < SAT_MAX_X) {
                set_lo(&r.d0, (uint16_t)(lo(r.d0) + SAT_BIAS));
                set_lo(&r.d1, (uint16_t)(lo(r.d1) + SAT_BIAS));
                set_lo(&r.d2, (uint16_t)(lo(r.d2) + 1));
                SprSatEntry *e = at<SprSatEntry>(r.a1);
                e->y = lo(r.d0);
                e->size = (uint8_t)(lo(r.d2) >> 8);      /* the piece size; link = the new sprite count */
                e->link = (uint8_t)r.d2;
                const u16 *tile = (const u16 *)((const u8 *)at<ArtDesc>(r.a2)->tiles + (int16_t)lo(r.d3));
                set_lo(&r.d0, (uint16_t)(*tile + lo(r.d6)));
                e->attr = lo(r.d0);
                e->x = lo(r.d1);
                r.a1 += sizeof(SprSatEntry);
                if ((uint8_t)r.d2 == SPR_MAX_SPRITES)
                    return true;
            }
        }
        r.d7 = (r.d7 & 0xFFFF0000u) | (uint16_t)(r.d7 - 1);
        if (lo(r.d7) == 0xFFFF)
            return false;
    }
}

#if DK_MODE_NATIVE
/* ---- Phase 6 (ruling R10, criterion E5): the display list without the game's 80-entry caps ----
 * Native only (R2: the emulated modes compile none of this; verify keeps proving the C above).
 * After the original walk has written the table, the list is walked again with the nodes the
 * 80-node cap refused this frame (Spr_Add* keep them on the host, spr_host.h) merged in where a
 * list without the cap would hold them:
 *   - front nodes (Spr_AddFront) after the list's front entries, in their order;
 *   - sorted nodes (Spr_AddSorted, key k > 0) after every listed entry of key >= k (the walk
 *     breaks at the first key <= k, an equal key chains after its last entry), before the first
 *     key node below k; among themselves by key, descending (unsigned, as cmp.w / bcc), then order;
 *   - back nodes (Spr_AddBack) and key-0 sorted ones (the tail sentinel's chain) after everything.
 * Every refused node comes after every listed one (the cap only refuses once the 80 are used), so
 * these rules are the uncapped list's order. Each node's pieces are made as emit_pieces makes them
 * (the same clip: SAT_MAX_Y / SAT_MAX_X / SAT_BIAS, the same flips, tile and attribute), the
 * pieces in walk order are the table without the 80-sprite cap: with unlimited_sprites on, the
 * first ones fill the VDP table's list entries (as many as the original wrote: the counts, the
 * link bytes, g_SATPtr and every register stay the original's), the rest go to the engine's host
 * table (drawn after the chain's end, so in the same order behind them). With it off nothing is
 * written; the walk counts the nodes the caps dropped (the reference's list_drops).
 * A check on every frame with no refused node: the walk's first pieces must be exactly what the
 * original wrote, entry for entry (else a fatal), which proves this walk against the C above.
 * Widescreen (R11): the original walk keeps SAT_MAX_X (the 4:3 table); this walk sends the pieces
 * the wide view adds to ws_draw (the margin pieces, below), never to the table or w.piece. */

static constexpr uint16_t SORT_HEAD = 0x9DE0;   /* g_SprSortHead */
static constexpr uint16_t SORT_TAIL = 0x9DF2;   /* its tail sentinel (key 0) */
static constexpr uint16_t FRONT_START = 0x9E16;  /* g_SprFrontEnd with no front entry */
static constexpr unsigned WALK_PIECES = SPR_MAX_SPRITES + SPR_HOST_PIECES;

struct WalkPiece {
    SprHostPiece sat;   /* y, size (the piece's word's high byte), attr, x */
    uint16_t size_word; /* the piece's size word (its low byte or'd into the link by the original) */
    uint16_t node;      /* the node it belongs to (walk order) */
};

static constexpr unsigned WALK_NODES = 2 * SPR_NODES + SPR_HOST_NODES;

struct Walk {
    WalkPiece piece[WALK_PIECES];
    unsigned n;              /* pieces made (n > WALK_PIECES: the rest not kept) */
    uint16_t node;          /* nodes walked */
    uint16_t off_x, off_y;   /* g_SprOffsetX / Y */
    HfWalkNode tag[WALK_NODES];   /* high_fps: each node's entry or owner, and place (hf_spr_walk2) */
};

static Walk s_walk;

/* ---- Phase 6 (ruling R11, criterion E3): the walk's wide clip --------------------------------
 * With widescreen on (ws_draw.h), the pieces SAT_MAX_X drops (screen x >= 320 or < -32) that the
 * wide view can show, and every piece of a culled draw (Op2_DrawInScreenBox's, kept by ws_draw as
 * nodes merged into this walk), are margin pieces: the host draws them outside the 4:3 view's
 * columns. The table, the counts and every register stay the original's: a margin piece never
 * enters w.piece (so the self-check below still compares the 4:3 pieces), only ws_draw's list. */
static bool s_ws_on;
static constexpr int WS_X_MIN = -112, WS_X_END = 400;   /* screen x a margin piece may start at */

/* the art block a non-resident culled frame would be loaded from (Spr_QueueArtDMA's layout: word
   cost, word chunks n, n chunk codes = 4 x tiles, the tiles; chunk k goes to the descriptor's
   block k): the 68000 address of the piece's first tile, 0 when it is not inside its chunk */
static uint32_t art_tile(uint32_t art, unsigned chunk, unsigned rel, unsigned tiles)
{
    const u16 *hdr = at<const u16>(art);
    unsigned n = (uint16_t)hdr[1];
    if (chunk >= n)
        return 0;
    uint32_t words = 0;
    for (unsigned j = 0; j < chunk; j++)
        words += 2u * (uint8_t)((uint16_t)hdr[2 + j] * 2u);   /* the queue's source step (its byte quirk) */
    unsigned in_chunk = (uint16_t)hdr[2 + chunk] / 4u;
    if (rel + tiles > in_chunk)
        return 0;
    return art + 4u + 2u * n + 2u * words + 32u * rel;
}

/* one margin piece at the walk's (px, py): the SAT entry the wide clip would have written; true
   when kept */
static bool ws_piece(uint16_t px, uint16_t py, const SprPiece *p, uint16_t attr, const ArtDesc *d, uint32_t art)
{
    int sx = (int16_t)px - (0x80 - SAT_BIAS), sy = (int16_t)py - (0x80 - SAT_BIAS);
    if (sx < WS_X_MIN || sx >= WS_X_END)
        return false;
    WsDrawPiece m;
    m.x = (int16_t)sx;
    m.y = (int16_t)sy;
    m.size = (uint8_t)((uint16_t)p->size >> 8);
    unsigned tiles = (((m.size >> 2) & 3) + 1) * ((m.size & 3) + 1);
    uint16_t hi = (uint16_t)((attr & 0xFF00u) | (uint8_t)p->tile_add);
    if (d) {
        const u16 *tile = (const u16 *)((const u8 *)d->tiles + (uint8_t)p->tile_slot);
        m.attr = (uint16_t)((uint16_t)*tile + hi);
        m.from_mem = 0;
        m.src = 0;
    } else {
        m.attr = (uint16_t)(attr & 0xF800u);
        m.from_mem = 1;
        m.src = art_tile(art, (uint8_t)p->tile_slot / 2u, (unsigned)(hi & 0x07FFu), tiles);
        if (!m.src) {
            ws_draw_unsupported();
            return false;
        }
    }
    ws_draw_piece(&m);
    return true;
}

/* one node's pieces (emit_pieces' arithmetic on plain values); hn: a kept or culled node (a culled
   one, R11, draws only margin pieces); entry: a listed node's entry address (high_fps's tags) */
static void walk_node(Walk &w, int16_t nx, int16_t ny, uint16_t attr, uint32_t frame_addr,
                      const SprHostNode *hn = nullptr, uint16_t entry = 0)
{
    bool culled = hn && hn->pad == 1;
    uint16_t node = w.node++;
    if (node < WALK_NODES)
        w.tag[node] = { entry, hn && !culled ? hn->owner : 0u, (int16_t)(nx + w.off_x), (int16_t)(ny + w.off_y) };
    const SprFrame *frame = at<const SprFrame>(frame_addr);
    uint16_t desc = *at<const u16>(sext16(frame->art_handle));
    bool log = s_ws_on && ws_draw_logging();
    if (desc == 0 && !culled) {
        if (log)
            ws_draw_log_node(hn, frame_addr, nx, ny, 0, 0, 0, 0, 0, 0, 0);
        return;                                       /* the art is not in VRAM */
    }
    const ArtDesc *d = desc ? at<const ArtDesc>(sext16(desc)) : nullptr;
    const u16 *list = at<const u16>(frame->pieces);
    unsigned count = (unsigned)(uint16_t)list[0] + 1;
    const SprPiece *p = (const SprPiece *)(list + 1);
    uint16_t x = (uint16_t)(nx + w.off_x), y = (uint16_t)(ny + w.off_y);
    int n_table = 0, n_margin = 0, x0 = 9999, x1 = -9999, y0 = 9999, y1 = -9999;
    for (unsigned i = 0; i < count; i++, p++) {
        uint16_t oy = (attr & SPR_ATTR_VFLIP) ? (uint16_t)(int16_t)p->y_vflip : (uint16_t)(int16_t)p->y;
        uint16_t py = (uint16_t)(oy + y);
        if (py >= SAT_MAX_Y)
            continue;
        uint16_t ox = (attr & SPR_ATTR_HFLIP) ? (uint16_t)(int16_t)p->x_hflip : (uint16_t)(int16_t)p->x;
        uint16_t px = (uint16_t)(ox + x);
        if (log) {
            int sx = (int16_t)px - (0x80 - SAT_BIAS), sy = (int16_t)py - (0x80 - SAT_BIAS);
            int size = (uint16_t)p->size >> 8;
            x0 = sx < x0 ? sx : x0;
            y0 = sy < y0 ? sy : y0;
            int ex = sx + (((size >> 2) & 3) + 1) * 8, ey = sy + ((size & 3) + 1) * 8;
            x1 = ex > x1 ? ex : x1;
            y1 = ey > y1 ? ey : y1;
        }
        if (culled || px >= SAT_MAX_X) {
            if (s_ws_on && ws_piece(px, py, p, attr, d, frame->art))
                n_margin++;
            continue;
        }
        n_table++;
        if (w.n < WALK_PIECES) {
            const u16 *tile = (const u16 *)((const u8 *)d->tiles + (uint8_t)p->tile_slot);
            uint16_t size_word = (uint16_t)p->size;
            WalkPiece &o = w.piece[w.n];
            o.sat.y = (uint16_t)(py + SAT_BIAS);
            o.sat.size = (uint8_t)(size_word >> 8);
            o.sat.link = 0;
            o.sat.attr = (uint16_t)((uint16_t)*tile + ((attr & 0xFF00u) | (uint8_t)p->tile_add));
            o.sat.x = (uint16_t)(px + SAT_BIAS);
            o.size_word = size_word;
            o.node = node;
        }
        w.n++;
    }
    if (log)
        ws_draw_log_node(hn, frame_addr, nx, ny, desc != 0, n_table, n_margin, x0, x1, y0, y1);
}

static void walk_entry(Walk &w, uint16_t entry)
{
    const SprEntry *e = spr_entry(entry);
    walk_node(w, e->x, e->y, e->attr, e->frame.v, nullptr, entry);
}

/* a kept node (spr_host's: pad 0) or a culled draw (ws_draw's, R11: pad 1) */
static void walk_kept(Walk &w, const SprHostNode *k)
{
    walk_node(w, k->x, k->y, k->attr, k->frame, k);
}

/* the merged walk: the listed entries (as the original walks them), the kept nodes and (with
   widescreen) the culled draws, the culled ones after the kept ones of their place */
static void walk_list(Walk &w)
{
    const SprHostNode *kept, *culled;
    unsigned nkept = spr_host_nodes(&kept);
    unsigned nculled = s_ws_on ? ws_draw_nodes(&culled) : 0;
    const SprHostNode *front[SPR_HOST_NODES + WS_DRAW_NODES], *sorted[SPR_HOST_NODES + WS_DRAW_NODES],
        *back[SPR_HOST_NODES + WS_DRAW_NODES];
    unsigned nf = 0, ns = 0, nb = 0;
    for (unsigned i = 0; i < nkept + nculled; i++) {
        const SprHostNode *k = i < nkept ? &kept[i] : &culled[i - nkept];
        if (k->kind == SPR_HOST_FRONT)
            front[nf++] = k;
        else if (k->kind == SPR_HOST_SORTED && k->key != 0)
            sorted[ns++] = k;
        else
            back[nb++] = k;
    }
    for (unsigned i = 1; i < ns; i++)                 /* by key descending, then order (stable) */
        for (unsigned j = i; j > 0 && sorted[j - 1]->key < sorted[j]->key; j--) {
            const SprHostNode *t = sorted[j];
            sorted[j] = sorted[j - 1];
            sorted[j - 1] = t;
        }
    /* the key nodes, head->next to the tail sentinel */
    uint16_t key_base[SPR_NODES + 2], key_val[SPR_NODES + 2];
    unsigned nkeys = 0;
    for (uint16_t k = spr_node(SORT_HEAD)->next;; k = spr_node(k)->next) {
        if (nkeys == SPR_NODES + 2)
            dk::fatal("Spr_BuildSAT (native): the key-node chain does not reach the tail");
        key_base[nkeys] = k;
        key_val[nkeys++] = spr_node(k)->key;
        if (k == SORT_TAIL)
            break;
    }

    /* the front entries (contiguous from $9E04 + 6, 18 bytes apart), then the kept front nodes */
    for (uint16_t base = (uint16_t)(FRONT_START - sizeof(SprKeyNode)); base + sizeof(SprKeyNode) < g_SprFrontEnd;
         base = (uint16_t)(base + sizeof(SprKeyNode)))
        walk_entry(w, (uint16_t)(base + 6));
    for (unsigned i = 0; i < nf; i++)
        walk_kept(w, front[i]);
    /* the sorted list from the head's link, as the original walks it */
    unsigned si = 0;
    for (uint16_t base = g_SprSortFirst, steps = 0;; steps++) {
        if (steps > 2 * SPR_NODES + 4)
            dk::fatal("Spr_BuildSAT (native): the sorted list does not end");
        for (unsigned k = 0; k < nkeys; k++)
            if (key_base[k] == base) {                /* a key node: the kept ones above its key first */
                while (si < ns && sorted[si]->key > key_val[k]) {
                    walk_kept(w, sorted[si]);
                    si++;
                }
                break;
            }
        const SprEntry *e = spr_entry((uint16_t)(base + 6));
        if ((uint16_t)e->link >= 0x10000 - 6)
            break;                                    /* the end slot */
        walk_entry(w, (uint16_t)(base + 6));
        base = e->link;
    }
    for (; si < ns; si++)
        walk_kept(w, sorted[si]);
    for (unsigned i = 0; i < nb; i++)
        walk_kept(w, back[i]);
}

/* the distinct nodes with a piece at index >= from */
static unsigned nodes_from(const Walk &w, unsigned from)
{
    unsigned n = 0;
    int last = -1;
    for (unsigned i = from; i < w.n && i < WALK_PIECES; i++)
        if (w.piece[i].node != last) {
            last = w.piece[i].node;
            n++;
        }
    if (w.n > WALK_PIECES)
        n++;                                          /* at least one more past what was kept */
    return n;
}

/* After the original: `start` = g_SprCount and `sat` = g_SATPtr at its entry. */
static void native_table(uint16_t start, uint16_t sat)
{
    Walk &w = s_walk;
    w.n = 0;
    w.node = 0;
    w.off_x = g_SprOffsetX;
    w.off_y = g_SprOffsetY;
    s_ws_on = ws_draw_on() != 0;                      /* R11: the margin pieces of this table */
    if (s_ws_on)
        ws_draw_begin();
    walk_list(w);
    if (s_ws_on)
        ws_draw_walk_end();
    unsigned first = (uint8_t)start, done = (uint8_t)g_SprCountLo;
    unsigned slots = (int8_t)(uint8_t)start < SPR_MAX_SPRITES ? done - first : 0;   /* the list's entries */
    bool capped = done >= SPR_MAX_SPRITES;
    const SprHostNode *kept;
    unsigned nkept = spr_host_nodes(&kept);
    SprSatEntry *table = at<SprSatEntry>(sext16(sat));
    if (nkept == 0) {
        /* the check: the original's entries are the walk's first pieces */
        bool ok = w.n >= slots && (capped || w.n == slots);
        for (unsigned j = 0; ok && j < slots; j++) {
            const WalkPiece &p = w.piece[j];
            const SprSatEntry &e = table[j];
            ok = (uint16_t)e.y == p.sat.y && (uint8_t)e.size == p.sat.size && (uint16_t)e.attr == p.sat.attr &&
                 (uint16_t)e.x == p.sat.x && (uint8_t)e.link == (uint8_t)(first + j + 1) && (p.size_word & 0xFF) == 0;
        }
        if (!ok)
            dk::fatal("Spr_BuildSAT (native): the uncapped walk differs from the table (%u pieces, %u entries "
                      "from %u)", w.n, slots, first);
    }
    if (!spr_host_on()) {
        spr_host_table_built(nullptr, 0, capped ? nodes_from(w, slots) : 0);
        return;
    }
    if (nkept) {
        /* the list's entries in the uncapped order (their link bytes kept) */
        for (unsigned j = 0; j < slots && j < w.n; j++) {
            const WalkPiece &p = w.piece[j];
            if (p.size_word & 0xFF)
                dk::fatal("Spr_BuildSAT (native): a piece's size word %04X has a low byte", p.size_word);
            table[j].y = p.sat.y;
            table[j].size = p.sat.size;
            table[j].attr = p.sat.attr;
            table[j].x = p.sat.x;
        }
    }
    static SprHostPiece host[SPR_HOST_PIECES];
    unsigned nh = 0;
    for (unsigned j = slots; j < w.n && j < WALK_PIECES && nh < SPR_HOST_PIECES; j++)
        host[nh++] = w.piece[j].sat;
    unsigned lost_from = slots + nh;
    spr_host_table_built(host, nh, lost_from < w.n ? nodes_from(w, lost_from) : 0);
    /* high_fps: the table's and the host table's pieces named by their nodes (the original walk's
       tags no longer describe a table rewritten in the uncapped order) */
    static uint16_t piece_node[WALK_PIECES];
    unsigned nt = slots < w.n ? slots : w.n;
    for (unsigned j = 0; j < nt + nh; j++)
        piece_node[j] = w.piece[j].node;
    hf_spr_walk2(w.tag, w.node < WALK_NODES ? w.node : WALK_NODES, piece_node, first, nt, host, nh);
}
#endif

/* Spr_BuildSAT - walks the front list, then the depth-sorted display list, and writes each
 * entry's frame pieces into the sprite attribute buffer, clipped to the screen, at most 80
 * sprites. Entries whose art is not resident (the handle holds no descriptor) are skipped.
 * In:  the lists Spr_Add* built this frame; g_SprOffsetX/Y added to every x/y; g_SATPtr,
 *      g_SprCount
 * Out: SAT entries appended; g_SprCountLo and g_SATPtr stored back. The registers as the walk
 *      leaves them: d1 = the last piece's x long, d2 = the count word, d3 = the last tile slot,
 *      d4/d5/d6 = the last node's x / y / attribute (sign-extended, offset; d6's low byte the last
 *      piece's), d7 = the piece counter, a1 = the SAT pointer, a2 = the last descriptor (or handle
 *      address), a4/a5 = the offsets, a6 = the node walk pointer (all unchanged at 80 sprites
 *      already)
 * Evidence: movea.w (g_SATPtr).w,a1 / cmpi.b #$50,d2 / bge, the front list's link
 *           move.w a6,-$1E(a0) / lea ($FFFF9E04).w,a6, movem.w (a6)+,d0/d4-d6 / addq.w #6,d0 /
 *           bcc (the end link $FFFF), the four piece loops (cmpi.w #$100 / #$160, addi.w #$60,
 *           move.w $6(a2,d3.w),d0 / add.w d6,d0, cmpi.b #$50,d2 / dbeq); once a frame from $9BF0
 */
void Spr_BuildSAT(uint32_t *d1, uint16_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint16_t *d7,
                  uint32_t *a1, uint32_t *a2, uint32_t *a4, uint32_t *a5, uint32_t *a6)
{
#if DK_MODE_NATIVE
    uint16_t native_start = g_SprCount, native_sat = g_SATPtr;   /* phase 6: native_table */
#endif
    SatRegs r = { 0, *d1, *d2, *d3, *d4, *d5, *d6, *d7, 0, *a2 };
    r.a1 = sext16(g_SATPtr);
    set_lo(&r.d2, g_SprCount);
    if ((int8_t)(uint8_t)r.d2 < SPR_MAX_SPRITES) {
        *a4 = sext16(g_SprOffsetX);
        *a5 = sext16(g_SprOffsetY);
        r.d3 = 0;
        *a6 = sext16(g_SprSortFirst);
        uint32_t front_end = sext16(g_SprFrontEnd);
        if (lo(front_end) != FRONT_LIST_EMPTY) {
            *at<u16>(front_end - 0x1E) = lo(*a6);    /* the last front node's next = the sorted list */
            *a6 = FRONT_LIST_FIRST;
        }
        *a6 += 6;                                    /* the walk reads each node from its +6 */
        for (;;) {
            const s16 *w = at<const s16>(*a6);
            r.d0 = sext16((uint16_t)w[0]);           /* movem.w (a6)+,d0/d4-d6: next, x, y, attr */
            r.d4 = sext16((uint16_t)w[1]);
            r.d5 = sext16((uint16_t)w[2]);
            r.d6 = sext16((uint16_t)w[3]);
            *a6 += 8;
            bool end = lo(r.d0) >= 0x10000 - 6;      /* addq.w #6,d0 carries: the end link */
            set_lo(&r.d0, (uint16_t)(lo(r.d0) + 6));
            if (end)
                break;
#if DK_MODE_NATIVE
            hf_spr_node((uint16_t)(*a6 - 8), (int16_t)w[1], (int16_t)w[2], (uint8_t)r.d2);
#endif
            const SprFrame *frame = *at<PTR(const SprFrame)>(*a6);
            *a6 = sext16(lo(r.d0));
            set_lo(&r.d4, (uint16_t)(lo(r.d4) + lo(*a4)));
            set_lo(&r.d5, (uint16_t)(lo(r.d5) + lo(*a5)));
            r.a2 = sext16(frame->art_handle);
            set_lo(&r.d0, *at<u16>(r.a2));
            if (lo(r.d0) == 0)
                continue;                            /* the art is not in VRAM */
            r.a2 = sext16(lo(r.d0));
            const u16 *list = at<const u16>(frame->pieces);
            set_lo(&r.d7, *list);
            if (emit_pieces(r, (const SprPiece *)(list + 1)))
                break;
        }
    }
#if DK_MODE_NATIVE
    hf_spr_walk_end((uint8_t)r.d2);
#endif
    g_SprCountLo = (uint8_t)r.d2;
    g_SATPtr = lo(r.a1);
    *d1 = r.d1;
    *d2 = lo(r.d2);
    *d3 = r.d3;
    *d4 = r.d4;
    *d5 = r.d5;
    *d6 = r.d6;
    *d7 = lo(r.d7);
    *a1 = r.a1;
    *a2 = r.a2;
#if DK_MODE_NATIVE
    native_table(native_start, native_sat);   /* phase 6 (R10): the uncapped list, see above */
#endif
}
