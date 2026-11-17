#include "game.h"

/* the raster driver's a2 script (ROM data right after this file) */
static constexpr uint32_t Scr2_03B956 = 0x03B956u;

enum {
    CATHEAD_FACE_X = 0xA0,   /* the head position at which the face picture is unscrolled */
    CATHEAD_FACE_Y = 0x68,
};

/* Boss_CatHead_FrameScroll - per-frame scroll hook of the cat-head arena: scrolls plane A (the face
 * picture) by the head's offset from ($A0, $68) so the face follows the head object, then runs
 * the current raster routine, if any
 * In:  the arena's head_x/head_y (g_SceneVars+8/+$A) and raster (+0); g_ViewX/g_ViewY
 * Out: g_HScrollA = head_x - $A0 + view x, g_VScrollA = $68 - head_y + view y; then a jump into
 *      the raster routine (Boss_CatHead_SwayRaster / CurtainRaster / FallRaster), or rts
 * Evidence: move.w (g_SceneVars+8).w,d0 / subi.w #$A0 / add.w (g_ViewX).w / move.w d0,(g_HScrollA).w,
 *           the same for y; move.l (g_SceneVars).w,d0 / beq / movea.l d0,a0 / jmp (a0); jsr from
 *           the arena scene loop $03FFBE ($040008); TAS f87285-f90392 (3108 frames)
 */
void Boss_CatHead_FrameScroll(void)
{
    const CatHeadArena *arena = cat_head_arena();
    g_HScrollA = (uint16_t)(arena->head_x - CATHEAD_FACE_X + (uint16_t)(g_ViewX >> 16));
    g_VScrollA = (uint16_t)(CATHEAD_FACE_Y - arena->head_y + (uint16_t)(g_ViewY >> 16));
    uint32_t raster = arena->raster;
    if (raster == 0)
        return;
    dk::jump(dk::code(raster));
}

/* Boss_CatHead_SpawnRasterDriver - spawns the cat head's raster driver (unlinked child $2E): an
 * invisible object whose position the Boss_CatHead_Driver* natives copy into the arena variables
 * for the curtain, sway and fall rasters
 * In:  d4/d5 = position (from Op2_SpawnChildOrWait)
 * Out: script (a2) = Scr2_03B956, then Obj_SpawnBossUnlinked's results
 * Evidence: lea Scr2_03B956,a2 / jmp Obj_SpawnBossUnlinked; Op2_SpawnChildOrWait $2E dc.l $03B94A
 *           at $03B22C; RAM f89640/f89880: the driver's y 192/91 matches the curtain's lower edge.
 *           a2 an output (measured as a clobber): Obj_SpawnBossUnlinked, jumped to, takes the
 *           script in it
 */
void Boss_CatHead_SpawnRasterDriver(uint16_t x, uint16_t y, uint32_t *d7, uint32_t a4, uint32_t *a6,
                                    uint32_t *script)
{
    *script = Scr2_03B956;
    dk::jump(at_Obj_SpawnBossUnlinked);
}
