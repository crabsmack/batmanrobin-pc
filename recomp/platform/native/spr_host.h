/* spr_host.h - the host side of the game's sprite display list and table in the native port
 * (phase 6, ruling R10, criterion E5: docs/phases/P6-enhance.md, docs/NOTES.md "Phase 6: unlimited
 * sprites"). Included only by native branches of the game's C (`#if DK_MODE_NATIVE`):
 *   Spr_ResetList  (007A08.c)  spr_host_list_reset      a new frame's list
 *   Spr_AddSorted  (007A4A.c)  spr_host_list_refused    a node the 80-entry cap refused
 *   Spr_AddBack    (007B1C.c)    "
 *   Spr_AddFront   (007B50.c)    "
 *   Spr_BuildSAT   (007B72.c)  spr_host_nodes, spr_host_table_built: the merged walk's pieces past
 *                              the VDP's table, and the frame's drop counts
 *   Spr_QueueSATDMA (0079C0.c) spr_host_table_sent      the host table goes with the SAT's DMA
 *   VDP_ClearSAT                spr_host_table_cleared   (a new scene: no host sprites)
 *   Spr_MaskLeftColumn (00F808.c) spr_host_cover        the x 0-15 cover drawn (a counter)
 *
 * With `unlimited_sprites` off this only counts (g_enh.sprites.list_drops / list_drop_frames /
 * cover_frames, the reference's numbers) and keeps nothing: the game runs as phase 5's port. With
 * it on, a refused node is kept here, never in work RAM (the game's list, its counters and every
 * register its Spr_Add* hands back are exactly the off run's), and Spr_BuildSAT draws the kept
 * nodes in the order a list without the cap would have them: the first pieces in the VDP's table
 * (as many entries as the off run's table has), the rest in the engine's host table
 * (gvdp_set_host_sat, drawn after the SAT chain's end).
 *
 * Host state, not game state: in branch snapshots (machine.cpp: spr_host_save / spr_host_load)
 * because the sync point ($9BF0, Frame_End's entry) falls between the list's adds and its table.
 */
#pragma once
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

enum { SPR_HOST_FRONT = 0, SPR_HOST_SORTED = 1, SPR_HOST_BACK = 2 };

/* a node the game's 80-entry cap refused: what its Spr_Add* was given */
typedef struct SprHostNode {
    uint32_t frame;     /* a0: the 10-byte frame-table entry (68000 address) */
    int16_t  x, y;      /* d4, d5 */
    uint16_t attr;      /* d6 */
    uint16_t key;       /* d7 (Spr_AddSorted's depth key; 0 for the others) */
    uint8_t  kind;      /* SPR_HOST_FRONT / SORTED / BACK */
    uint8_t  pad;
    uint16_t seq;       /* the refusal's order in the frame */
    uint32_t owner;     /* high_fps's sprite tag: the object whose code ran (present.h hf_spr_owner) */
} SprHostNode;

/* one sprite in the VDP's SAT format (the engine's GVDPHostSat) */
typedef struct SprHostPiece {
    uint16_t y;         /* y + 128 */
    uint8_t  size;      /* (w-1) << 2 | (h-1) */
    uint8_t  link;      /* ignored */
    uint16_t attr;
    uint16_t x;         /* x + 128 */
} SprHostPiece;

#define SPR_HOST_NODES 256    /* refused nodes kept a frame (past: counted as list drops) */
#define SPR_HOST_PIECES 256   /* host-table sprites (GVDP_HOST_SAT_MAX; past: counted) */

int  spr_host_on(void);                              /* g_opt.unlimited_sprites */
void spr_host_list_reset(void);
void spr_host_list_refused(int kind, uint32_t frame, int16_t x, int16_t y, uint16_t attr, uint16_t key);
unsigned spr_host_nodes(const SprHostNode **nodes);  /* this frame's kept nodes, in refusal order */
/* Spr_BuildSAT's result: `pieces` past the VDP's table (option on), and the display-list nodes
   the 80-entry table dropped this frame (the cap, or past the host's limits) */
void spr_host_table_built(const SprHostPiece *pieces, unsigned n, unsigned nodes_dropped);
void spr_host_table_sent(void);
unsigned spr_host_shown(const SprHostPiece **pieces);   /* the host table as the game made it (its SAT x) */
void spr_host_table_cleared(void);

/* Widescreen's render camera (widescreen.cpp, ws-host 2026-10-30): while a frame is drawn the
   engine draws the host table moved as the VDP table's sprites are (-shift, the HUD's pins, a VDP-x-0
   sprite left alone). `view` makes the drawn copy from the shown one; every send to the engine goes
   through it (a table sent inside the window too). Null: the engine draws the shown table. */
typedef void (*SprHostView)(const SprHostPiece *in, SprHostPiece *out, unsigned n);
void spr_host_set_view(SprHostView view);   /* re-sends the table through it */
unsigned spr_host_drawn(const SprHostPiece **pieces);   /* what the engine draws now (high_fps's capture) */
void spr_host_redraw(void);                  /* the engine's host table put back to the drawn one (present.cpp) */
/* Diagnostic pieces drawn after the shown table, through the view (widescreen.cpp's
   BR_WS_HOST_MIRROR: copies of the VDP table's sprites, each with a known twin); n = 0 clears.
   Takes effect at the next send (spr_host_set_view); returns how many fit (SPR_HOST_PIECES in all). */
unsigned spr_host_set_extra(const SprHostPiece *pieces, unsigned n);
void spr_host_cover(void);

/* branch snapshots */
size_t spr_host_save(uint8_t *buf, size_t cap);      /* buf null: the size */
void   spr_host_load(const uint8_t *buf);
void   spr_host_power_on(void);                      /* machine.cpp nm_power_on: empty, counters active */

#ifdef __cplusplus
}
#endif
