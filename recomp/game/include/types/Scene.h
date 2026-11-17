/* types/Scene.h - the scene headers (docs/NOTES.md, the scene flow): what Scene_InitCommon reads
 * from a6 for every scene, and what Scene_InitStage reads after it for a gameplay section. */
#pragma once
#include "dk_mem.h"

/* Scene_InitCommon's header, read with (a6)+ */
typedef struct SceneHeader {
    u16 section_id;                /* +0: g_SectionId, 0 = keep it */
    u16 art_blocks;                /* +2: ArtCache_Init's 16-tile VRAM blocks */
    u32 vdp_regs;                  /* +4: 19 VDP register bytes (VDP_LoadRegs) */
    u32 setup;                     /* +8: a routine called once the VDP is set (SceneSetupFn) */
    u16 dma_budget;                /* +$C: g_DMABudget */
    u16 spr_attr;                  /* +$E: g_SceneSprAttr */
    u8 shadow_hl;                  /* +$10: g_SceneShadowHL */
    u8 fixed_spr;                  /* +$11: g_SceneFixedSpr */
} SceneHeader;
static_assert(sizeof(SceneHeader) == 0x12, "SceneHeader");

/* one player's part of a section header: the HUD home and the start position */
typedef struct StageStartPos {
    s16 x;
    s16 y;
} StageStartPos;

/* Scene_InitStage's part, right after the SceneHeader */
typedef struct StageHeader {
    s16 move_left;                 /* +0: g_MoveAreaOfsX */
    s16 move_right;                /* +2: minus the left = g_MoveAreaW */
    s16 move_top;                  /* +4: g_MoveAreaOfsY */
    s16 move_bottom;               /* +6: minus the top = g_MoveAreaH */
    StageStartPos hud[2];          /* +8: each player's HUD home, before the +8/+$D4 offsets */
    StageStartPos start[2];        /* +$10: each player's start offset from the camera */
    u16 join_x;                    /* +$18: g_JoinOfsX */
    u16 join_y;                    /* +$1A: g_JoinOfsY */
    u16 play_mode;                 /* +$1C: g_PlayMode */
    u32 terrain_fn;                /* +$1E: g_TerrainFn */
    u32 terrain_fn_alt;            /* +$22: g_TerrainFnAlt */
    u16 camera_x;                  /* +$26: g_CameraX */
    u16 camera_y;                  /* +$28: g_CameraY */
} StageHeader;
static_assert(sizeof(StageHeader) == 0x2A, "StageHeader");
