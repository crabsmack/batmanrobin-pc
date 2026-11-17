/* ws_draw.h - the widescreen view's margin draws (phase 6, ruling R11, criterion E3:
 * docs/phases/P6-enhance.md, docs/NOTES.md "Phase 6: widescreen", part R11). Included only by
 * native branches of the game's C (`#if DK_MODE_NATIVE`) and the native machine.
 *
 * With widescreen on, the game's draw decisions are widened to the wide view without the game
 * knowing: where a draw test or a clip drops a sprite that the 400-px view would show, the piece
 * is drawn here instead, by the host, into the margins only:
 *   Op2_DrawInScreenBox (004454.c)  ws_draw_node   an a2 object the box culls but the wide box keeps:
 *                                                  its draw list read (nothing run), each frame a node
 *   Spr_ResetList  (007A08.c)       ws_draw_list_reset
 *   Spr_BuildSAT   (007B72.c)       ws_draw_nodes / ws_draw_begin / ws_draw_piece: the walk with the
 *                                   wide clip; the pieces the 4:3 clip drops (and every piece of a
 *                                   culled node) are margin pieces
 *   Spr_EmitClipped (007D76.c)      ws_draw_piece   a direct draw's pieces past g_SprDirectMaxX (the GO! sign)
 *   Spr_QueueSATDMA (0079C0.c)      ws_draw_sent    they show with that table, as the host table does
 *   VDP_ClearSAT  (00ADE8.c)        ws_draw_cleared
 *
 * Budget of their own (R11): a margin piece takes no VRAM, no art cache entry and no DMA. A piece
 * whose art is resident is drawn from the VRAM tiles the 4:3 view uses (read only); one whose art is
 * not (a culled object's frame never loaded) is drawn from the art block in ROM / RAM the game's
 * Spr_QueueArtDMA would copy. Drawn through the engine's host sprite layer (gvdp_set_host_sprites)
 * after every SAT and host-table sprite (behind them), only in the output columns outside the 4:3
 * view: the centre is the 4:3 frame's, the game's state is untouched (no work RAM, no VRAM, no
 * register written), so --game-rows needs no mask for them.
 *
 * Host state, not game state: in branch snapshots (machine.cpp), as spr_host's.
 */
#pragma once
#include <stddef.h>
#include <stdint.h>
#include "spr_host.h"

#ifdef __cplusplus
extern "C" {
#endif

#define WS_DRAW_NODES  128    /* culled draws kept a frame (past: counted in nodes_lost) */
#define WS_DRAW_PIECES 256    /* margin pieces a frame (past: counted in pieces_lost) */

/* one margin piece, in the 4:3 view's screen coordinates */
typedef struct WsDrawPiece {
    int16_t  x, y;        /* top-left (screen x 0 = the 4:3 view's left edge) */
    uint8_t  size;        /* the SAT size byte: (w-1) << 2 | (h-1) */
    uint8_t  from_mem;    /* 0: the pattern is VRAM tile attr & $7FF; 1: at `src` (68000 address) */
    uint16_t attr;        /* the SAT attribute word (priority, palette, flips; the tile when !from_mem) */
    uint32_t src;         /* from_mem: the first tile's 32 bytes, the piece's tiles column-major after it */
} WsDrawPiece;

int  ws_draw_on(void);                       /* g_opt.widescreen */
/* the wide box of a 4:3 box's x range: px the view adds per side (render camera included) */
int  ws_draw_reach(void);

void ws_draw_list_reset(void);
void ws_draw_node(int kind, uint32_t frame, int16_t x, int16_t y, uint16_t attr, uint16_t key, uint32_t owner);
unsigned ws_draw_nodes(const SprHostNode **nodes);   /* this frame's culled draws, in order */
void ws_draw_unsupported(void);              /* a culled draw this code cannot read (a counter) */
/* the measurement (BR_WS_DRAW_LOG): is this table a sample; one node the walk drew */
int  ws_draw_logging(void);
void ws_draw_log_node(const SprHostNode *node, uint32_t frame, int16_t x, int16_t y, int resident, int table,
                      int margin, int x0, int x1, int y0, int y1);

void ws_draw_begin(void);                    /* Spr_BuildSAT: a new walk (the direct pieces since kept) */
void ws_draw_piece(const WsDrawPiece *p);     /* the walk's, or a direct draw's (Spr_EmitClipped) */
void ws_draw_walk_end(void);
void ws_draw_sent(void);
void ws_draw_cleared(void);

/* The HUD items the game writes straight into its SAT buffer that widescreen.cpp pins (ws-tail,
 * NOTES "Phase 6: widescreen", the HUD): the boss meter (BossMeter_DrawSprite 011C5E.c) and the GO!
 * sign (HUD_UpdateGoSign 011A1C.c through Spr_DrawDirect 007D5E.c). Each writer tags the SAT buffer
 * entries it wrote; the tags follow the table as the margin pieces do (Spr_QueueSATDMA: shown,
 * VDP_ClearSAT: none), so the frame drawn from that table knows which VRAM entries they are. Kept
 * in every native run, widescreen on or off: the 4:3 run's centre exceptions read them too. */
#define WS_HUD_METER 4        /* the boss meter: pinned to the screen's centre (between the panels) */
#define WS_HUD_SIGN  5        /* the GO! sign: pinned to the right edge (it slides in from it) */
void ws_hud_direct_begin(int kind);          /* the next Spr_DrawDirect draws this HUD item */
int  ws_hud_direct_kind(void);               /* inside that Spr_DrawDirect: the item (0 none) */
void ws_hud_direct_end(void);
void ws_hud_tag(uint16_t sat_from, uint16_t sat_to, int kind);   /* g_SATPtr before / after the writes */
int  ws_hud_shown_kind(unsigned entry);      /* the shown table's entry (0..79): its HUD item, 0 none */

/* branch snapshots */
size_t ws_draw_save(uint8_t *buf, size_t cap);    /* buf null: the size */
void   ws_draw_load(const uint8_t *buf);
void   ws_draw_power_on(void);

#ifdef __cplusplus
}
#include <string>
std::string ws_draw_json(void);              /* the counters, for ws_json_extra */
#endif
