/* types/Stage3.h - stage 3's records (docs/NOTES.md "Hatter road", "Mad Hatter boss", "Stage 3 section
 * routines"): the Hatter's little top hat and the road's per-scene variables. */
#pragma once
#include "dk_mem.h"

/* the little top hat that pops out of the Hatter's big hat (a4 object of Scr4_03D908, a Vm4Obj):
   Boss_Hatter_LittleHatSetOwner stores its owner where a shot keeps its damage */
typedef struct LittleHat {
    u8  unk_00[0x12];
    s16 owner;             /* +$12: handle of the Hatter boss object (a Vm2Obj) it follows */
} LittleHat;
static_assert(offsetof(LittleHat, owner) == 0x12, "LittleHat.owner");

/* a thrown toy figure (a4 object of Scr4_03D9BC, a Vm4Obj): the fields Boss_Hatter_ThrownToyFrame sets */
typedef struct ThrownToy {
    u8  unk_00[0x1E];
    u16 frame;             /* +$1E: the sprite frame wanted (Vm4Obj.frame) */
    u8  unk_20[0x02];
    u8  attr;              /* +$22: the sprite attributes' high byte (priority, palette, flips) */
} ThrownToy;
static_assert(offsetof(ThrownToy, frame) == 0x1E, "ThrownToy.frame");
static_assert(offsetof(ThrownToy, attr) == 0x22, "ThrownToy.attr");

/* one rotation step of the toy figure's frame tables (Tbl_036DA8, Tbl_ToyFigureChargedFrames: 60
   steps, 4 bytes each) */
typedef struct ToyFrameStep {
    u16 frame;             /* +$00: the frame */
    u8  attr_mask;         /* +$02: ANDed with $E0 into the attributes' high byte */
    u8  flip;              /* +$03: then XORed in (the flips that make 60 steps of 16 frames) */
} ToyFrameStep;
static_assert(sizeof(ToyFrameStep) == 4, "ToyFrameStep");

/* one checker row of the Hatter road as Stage3_DrawRoad leaves it for the road objects (8 bytes,
   from g_SceneVars+$12, nearest first) */
typedef struct RoadRow {
    s16 line;              /* +$00: its screen line (relative to the road origin g_SceneVars2+$3C) */
    s16 clip_line;         /* +$02: a row whose line is below this is hidden behind a crest */
    s16 x_offset;          /* +$04: the road's sideways offset at this row (integer part) */
    s16 scale;             /* +$06: its perspective scale (x' = (x + x_offset) * scale / 4096) */
} RoadRow;
static_assert(sizeof(RoadRow) == 8, "RoadRow");

/* g_SceneVars on the Hatter road (section $3401) */
typedef struct HatterRoadScene {
    s16 swirl;             /* +$00: handle of the road-end swirl (Stage3_RunRoadEndSwirl), 0 = none */
    u16 travel;            /* +$02: the road schedule's travel within a checker row (mod $80) */
    u16 depth;             /* +$04: the schedule's gosub depth (bytes, 4 a level) */
    u8  loop;              /* +$06: the schedule's loop count (a byte: Boss_Hatter_RoadCmdSetLoop / Loop) */
    u8  unk_07;
    u32 stack[2];          /* +$08: the schedule's gosub return pointers */
    u8  wait;              /* +$10: checker rows the schedule still waits (counted down, ends at -1) */
    u8  unk_11;
    RoadRow rows[19];      /* +$12: the drawn rows, nearest first (+$A2 = the farthest) */
} HatterRoadScene;
static_assert(offsetof(HatterRoadScene, travel) == 0x02, "HatterRoadScene.travel");
static_assert(offsetof(HatterRoadScene, depth) == 0x04, "HatterRoadScene.depth");
static_assert(offsetof(HatterRoadScene, loop) == 0x06, "HatterRoadScene.loop");
static_assert(offsetof(HatterRoadScene, stack) == 0x08, "HatterRoadScene.stack");
static_assert(offsetof(HatterRoadScene, wait) == 0x10, "HatterRoadScene.wait");
static_assert(offsetof(HatterRoadScene, rows) == 0x12, "HatterRoadScene.rows");
static_assert(offsetof(HatterRoadScene, rows) + 18 * sizeof(RoadRow) == 0xA2, "HatterRoadScene.rows[18]");

/* g_SceneVars2 on the Hatter road: the road's state (Stage3_DrawRoad, Boss_Hatter_SteerRoad) */
typedef struct HatterRoadVars {
    u8  unk_00[0x08];
    s32 speed;             /* +$08: road speed (16.16; eased by Boss_Hatter_SteerRoad) */
    s16 step;              /* +$0C: this frame's speed, integer part (what the objects move by) */
    u16 phase;             /* +$0E: distance mod $80: the checker phase */
    u16 distance;          /* +$10: distance travelled (integer, wraps) */
    s32 scroll_x;          /* +$12: sideways scroll, -(camera x * 9/8) (16.16) */
    s32 bend;              /* +$16: the road's bend (16.16, +-24 px; the steerer's x / 16) */
    s16 horizon;           /* +$1A: the horizon line, $64 + g_ShakeY */
    s32 pitch;             /* +$1C: the road's pitch (16.16, -$1E0..$28; the steerer's y / 16) */
    u16 colour;            /* +$20: the checker colour's V-scroll offset, $100 toggled per row */
    s16 line;              /* +$22: the line the rows have reached (drawing up from the bottom) */
    u16 depth;             /* +$24: the previous row's depth factor */
    s32 pitch_acc;         /* +$26: the pitch's accumulated slope */
    s32 height;            /* +$2A: the road height ramp (16.16) */
    s32 bend_acc;          /* +$2E: the bend's accumulated slope */
    s32 x_offset;          /* +$32: the current row's sideways offset (16.16) */
    s32 x_scaled;          /* +$36: the previous row's offset times its depth */
    s16 origin_x;          /* +$3A: the screen origin of the road objects ($C0 - g_SprOffsetX) */
    s16 origin_y;          /* +$3C: and its y ($1F - g_SprOffsetY) */
    u16 spin;              /* +$3E: thrown toys' rotation step (+4 a frame, mod $F0) */
} HatterRoadVars;
static_assert(offsetof(HatterRoadVars, speed) == 0x08, "HatterRoadVars.speed");
static_assert(offsetof(HatterRoadVars, step) == 0x0C, "HatterRoadVars.step");
static_assert(offsetof(HatterRoadVars, distance) == 0x10, "HatterRoadVars.distance");
static_assert(offsetof(HatterRoadVars, scroll_x) == 0x12, "HatterRoadVars.scroll_x");
static_assert(offsetof(HatterRoadVars, bend) == 0x16, "HatterRoadVars.bend");
static_assert(offsetof(HatterRoadVars, horizon) == 0x1A, "HatterRoadVars.horizon");
static_assert(offsetof(HatterRoadVars, pitch) == 0x1C, "HatterRoadVars.pitch");
static_assert(offsetof(HatterRoadVars, colour) == 0x20, "HatterRoadVars.colour");
static_assert(offsetof(HatterRoadVars, pitch_acc) == 0x26, "HatterRoadVars.pitch_acc");
static_assert(offsetof(HatterRoadVars, height) == 0x2A, "HatterRoadVars.height");
static_assert(offsetof(HatterRoadVars, x_offset) == 0x32, "HatterRoadVars.x_offset");
static_assert(offsetof(HatterRoadVars, x_scaled) == 0x36, "HatterRoadVars.x_scaled");
static_assert(offsetof(HatterRoadVars, origin_x) == 0x3A, "HatterRoadVars.origin_x");
static_assert(offsetof(HatterRoadVars, spin) == 0x3E, "HatterRoadVars.spin");
static_assert(sizeof(HatterRoadVars) == 0x40, "HatterRoadVars");

static inline HatterRoadVars *hatter_road(void) { return (HatterRoadVars *)g_SceneVars2; }
static inline HatterRoadScene *hatter_road_scene(void) { return (HatterRoadScene *)g_SceneVars; }

/* one queued plane-B cell write of the forest-clearing reveal (6 bytes: Stage3_RevealPlaceTile
   queues it, Stage3_RevealVIntFlush sends it) */
typedef struct RevealCellWrite {
    u32 vdp_cmd;           /* +$00: the VDP write command of the cell's name-table word */
    u16 name;              /* +$04: the name-table word (the picture's map word + $637C) */
} RevealCellWrite;
static_assert(sizeof(RevealCellWrite) == 6, "RevealCellWrite");

/* g_SceneVars in the forest clearing while the reveal director assembles plane B's picture */
typedef struct ClearingReveal {
    u16 cursor;            /* +$00: the cell map word Stage3_RevealNextCell tests next (low word of
                              its RAM address in g_SceneWorkBuf: a handle) */
    s16 column;            /* +$02: the cursor's cell column (0-39; -1 before the first) */
    s16 row;               /* +$04: and row (0-21) */
    u16 seed;              /* +$06: the burst spot order: spot = (spot * $555 + 1) mod 64, < 36 */
    s16 vint_obj;          /* +$08: handle of the V-int object (Stage3_RevealVIntFlush) */
    u16 queue_bytes;       /* +$0A: bytes queued in queue (6 a cell) */
    RevealCellWrite queue[1];   /* +$0C: the queued writes (as many as queue_bytes says) */
} ClearingReveal;
static_assert(offsetof(ClearingReveal, seed) == 0x06, "ClearingReveal.seed");
static_assert(offsetof(ClearingReveal, queue_bytes) == 0x0A, "ClearingReveal.queue_bytes");
static_assert(offsetof(ClearingReveal, queue) == 0x0C, "ClearingReveal.queue");

static inline ClearingReveal *clearing_reveal(void) { return (ClearingReveal *)g_SceneVars; }

/* a reveal piece: the a4 object (a Vm4Obj) the piece script Scr4_03CE3E makes for one cell */
typedef struct RevealPiece {
    u8  unk_00[0x12];
    u16 tile;              /* +$12: its sprite tile word (the picture's tile + $4CA; Vm4Obj.damage) */
    u8  unk_14[0x1A];
    s16 x;                 /* +$2E: world x (Vm4Obj.x) */
    u16 cell_col;          /* +$30: its cell column, where the spawn left it (Vm4Obj.x_frac) */
    s16 y;                 /* +$32: world y */
    u16 cell_row;          /* +$34: its cell row (Vm4Obj.y_frac) */
} RevealPiece;
static_assert(offsetof(RevealPiece, tile) == 0x12, "RevealPiece.tile");
static_assert(offsetof(RevealPiece, x) == 0x2E, "RevealPiece.x");
static_assert(offsetof(RevealPiece, cell_row) == 0x34, "RevealPiece.cell_row");

/* g_SceneVars2 in the forest clearing: the palette-line-3 colour walker (Stage3_InitLine3Cycle,
   Stage3_StepLine3Cycle), components at their CRAM bit positions */
typedef struct ClearingColourCycle {
    u8  unk_00[0x22];
    u16 red;               /* +$22: 0-$E */
    u16 green;             /* +$24: 0-$E0 */
    u16 blue;              /* +$26: 0-$E00 */
    u16 red_step;          /* +$28: +-2 */
    u16 green_step;        /* +$2A: +-$20 */
    u16 blue_step;         /* +$2C: +-$200 */
} ClearingColourCycle;
static_assert(offsetof(ClearingColourCycle, red) == 0x22, "ClearingColourCycle.red");
static_assert(offsetof(ClearingColourCycle, blue_step) == 0x2C, "ClearingColourCycle.blue_step");

static inline ClearingColourCycle *clearing_colours(void) { return (ClearingColourCycle *)g_SceneVars2; }
