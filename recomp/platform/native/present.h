/* present.h - high_fps (phase 6 R3, E4): the interpolator and what feeds it (present.cpp).
 *
 *   machine.cpp   each rendered display, with high_fps on: hf_display_begin before its first line,
 *                 hf_display_line before each line is drawn (the VSRAM, the registers and the
 *                 palette the line is drawn with, after the H-ints before it), hf_display_end;
 *                 hf_invalidate when a branch snapshot is restored (no previous frame then)
 *   runner.cpp    hf_init once (registers the interpolator with enhance.cpp's present clock);
 *                 hf_frame_done after each runner frame is presented (its last display is R_(k-1)
 *                 for the next); played runs call hf_interpolate for the frames in between
 *   the game's C  sprite owner tags (native branches, R2): hf_spr_* below
 *
 * Nothing here is game state: the captures are copies, the interpolated picture is drawn from a
 * copy of the video model, and the game never reads any of it.
 */
#pragma once

#include <stdint.h>

#include "spr_host.h"

#ifdef __cplusplus
extern "C" {
#endif

void hf_display_begin(void);
void hf_display_line(int line);
void hf_display_end(void);
void hf_display_vram(uint16_t addr);      /* a host write into VRAM while the display is drawn (widescreen.cpp's
                                             put8: the view's per-line writes), kept for the line about to be drawn */
void hf_invalidate(void);

void hf_init(void);
void hf_frame_done(void);
int  hf_interpolate(uint64_t k, double alpha, uint32_t *fb, int stride, int w, int h);

/* Sprite owner tags (native branches of the game's C): hf_spr_owner names the object whose code
   runs (Obj_RunList); hf_spr_add names each display-list entry Spr_AddSorted / Spr_AddBack /
   Spr_AddFront writes (the address Spr_BuildSAT's walk reads it from), the running object's;
   Spr_BuildSAT's walk then names, before a node's pieces are written, the node's entry, its x and
   y as the walk reads them (before g_SprOffsetX/Y) and the SAT index its first piece takes. */
void hf_spr_node(uint16_t entry, int16_t x, int16_t y, int first_index);
void hf_spr_walk_end(int count);          /* the walk's end: the sprite count it leaves */
void hf_spr_owner(uint32_t object);       /* 0: no object's code runs */
void hf_spr_add(uint16_t entry);
uint32_t hf_spr_current_owner(void);      /* spr_host.cpp: a refused node's owner (unlimited_sprites) */

/* unlimited_sprites: Spr_BuildSAT's native second walk (the uncapped order, the host table past
   the SAT) names its pieces again: each node (a listed entry's address as hf_spr_node has it, or 0
   and a kept node's owner; x, y with g_SprOffsetX/Y added) and each piece's node, the table's
   `ntable` from SAT index `first`, then the host table's `nhost` pieces. */
typedef struct HfWalkNode {
    uint16_t entry;
    uint32_t owner;
    int16_t  x, y;
} HfWalkNode;
void hf_spr_walk2(const HfWalkNode *nodes, unsigned nnodes, const uint16_t *piece_node, unsigned first,
                  unsigned ntable, const SprHostPiece *host, unsigned nhost);

#ifdef __cplusplus
}
#endif
