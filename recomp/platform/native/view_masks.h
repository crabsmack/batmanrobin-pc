/* view_masks.h - what `--game-rows` leaves out of the game's state (docs/phases/P6-enhance.md E3
 * and E5: "work RAM less the drawing state the widened culls change, each masked range listed
 * with its readers"). Included once, by enhance.cpp; the widescreen and sprite workers add rows.
 *
 * The game's state at a sync point (game.csv's game_crc32) is the CRC-32 of: work RAM with the
 * sync rows' masks (the stack area, [recomp] dont_care) and every ENH_RAM row below zeroed; VRAM
 * with the plane A, plane B and window name tables and the sprite attribute table (from the VDP's
 * registers at that sync point) and every ENH_VRAM row below zeroed; CRAM; VSRAM. A masked range
 * that fed the game would show in the rest, so the comparison checks the masks too.
 *
 * Each row: the space, [lo, hi) as offsets ($FF0000-based for RAM, byte addresses for VRAM), what
 * it holds, and every reader of it (functions by address and name) with why none of them decides
 * the game. Every row applies to every --game-rows run, whatever the options: both sides of a
 * comparison mask the same ranges.
 */
#pragma once

enum { ENH_RAM = 0, ENH_VRAM = 1 };

typedef struct EnhMask {
    int space;
    uint32_t lo, hi;
    const char *what;
    const char *readers;
} EnhMask;

static const EnhMask k_view_masks[] = {
    /* E5 (R10, sprites-p6 2026-10-30): with unlimited_sprites a refused display-list node is kept
       on the host, never in RAM (the list, its counters and the registers Spr_Add* return are the
       off run's), and only the table's list entries change: rewritten in the uncapped list's
       order (their count and link bytes kept; the rest go to the engine's host table). */
    { ENH_RAM, 0x9B56, 0x9DD6,
      "g_SATBuffer: the sprite table built each frame (80 x 8 bytes; with unlimited_sprites its list "
      "entries in the uncapped order)",
      "no game code reads it: written at g_SATPtr by 007B72 Spr_BuildSAT, 007D5E Spr_DrawDirect / 007D76 "
      "Spr_EmitClipped, 00F808 Spr_MaskLeftColumn, 01191C HUD_DrawPanelSprites, 011C5E BossMeter_DrawSprite, "
      "011A1C HUD_UpdateGoSign, 015940 Debug_DrawHexBytes, 03CEF8 Stage3_RevealDrawTile; 0079C0 "
      "Spr_QueueSATDMA clears the last link byte and queues it as a DMA source to the VDP's sprite table "
      "(VRAM, masked by game_crc32 itself); the count g_SprCount / g_SprCountLo and g_SATPtr are not masked" },
    { ENH_RAM, 0, 0, nullptr, nullptr }   /* end */
};
