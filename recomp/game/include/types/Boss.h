/* types/Boss.h - the bosses' records (docs/NOTES.md "Cat-head boss"). */
#pragma once
#include "dk_mem.h"

/* g_SceneVars in the cat-head arena (scene $03FFBE, section $3101): the head's position for the
   face picture and the raster driver's values for the three rasters (curtain, sway, fall) */
typedef struct CatHeadArena {
    u32 raster;            /* +$00: the raster routine Boss_CatHead_FrameScroll calls each frame
                              (Boss_CatHead_SwayRaster / CurtainRaster / FallRaster), 0 = none */
    s16 curtain_h;         /* +$04: the curtain's height in lines (Boss_CatHead_DriverCurtainY) */
    s16 fall;              /* +$06: -(the driver's y): how far the face plate still stands (DriverFallY) */
    s16 head_x;            /* +$08: the head object's x (Boss_CatHead_PublishPos) */
    s16 head_y;            /* +$0A: and its y */
    s32 bulge;             /* +$0C: the sway raster's bulge (the driver's 16.16 y >> 4) */
    s16 shear;             /* +$10: the sway's slope (the driver's 16.16 x >> 6, low word) */
    s16 pivot_x;           /* +$12: the sway's pivot column x ($A0) */
    s16 pivot_y;           /* +$14: and pivot line ($68) */
    s16 unk_16;            /* +$16: read as the low word of the long at +$14 (Boss_CatHead_SwayPoint) */
} CatHeadArena;
static_assert(offsetof(CatHeadArena, curtain_h) == 0x04, "CatHeadArena.curtain_h");
static_assert(offsetof(CatHeadArena, fall) == 0x06, "CatHeadArena.fall");
static_assert(offsetof(CatHeadArena, head_x) == 0x08, "CatHeadArena.head_x");
static_assert(offsetof(CatHeadArena, bulge) == 0x0C, "CatHeadArena.bulge");
static_assert(offsetof(CatHeadArena, shear) == 0x10, "CatHeadArena.shear");
static_assert(offsetof(CatHeadArena, pivot_x) == 0x12, "CatHeadArena.pivot_x");
static_assert(offsetof(CatHeadArena, pivot_y) == 0x14, "CatHeadArena.pivot_y");
static_assert(offsetof(CatHeadArena, unk_16) == 0x16, "CatHeadArena.unk_16");

static inline CatHeadArena *cat_head_arena(void) { return (CatHeadArena *)g_SceneVars; }
