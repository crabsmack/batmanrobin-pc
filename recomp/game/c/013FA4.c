#include "game.h"

/* WeaponDrop_SpawnOnScreen - calls the spawner named by the script's dc.l at a screen position
 * made world-relative (plus the camera's integer x/y). a2-a6 are kept across the spawner (saved
 * on the 68000's stack, as the original's movem).
 * In:  x/y = screen position; script = at the spawner's dc.l; a4/a6 = the director's blocks
 * Out: x/y = world position; script past the long; the spawner's d0/d1/d7 (Obj_SpawnEnemy's)
 * Evidence: movea.l (a2)+,a0 / movem.l a2-a6,-(a7) / add.w (g_CameraX).w,d4 / add.w (g_CameraY).w,d5
 *           / jsr (a0) / movem.l (a7)+,a2-a6; the tail of WeaponDrop_FromTop/FromRight; spawners
 *           Item_SpawnPowerUpFalling/Drifting; 15 corpus runs (continue, game-over, level-skip)
 */
void WeaponDrop_SpawnOnScreen(uint32_t *d0, uint16_t *d1, uint16_t *x, uint16_t *y, uint32_t *d7, const u32 **script,
                              uint32_t a4, uint32_t a6)
{
    uint32_t spawner = *(*script)++;
    /* movem.l a2-a6,-(a7): a3 and a5 are not inputs (nothing reads them back) */
    uint32_t a2 = dk::addr(*script);
    dk::push_long(a6);
    dk::push_long(0);
    dk::push_long(a4);
    dk::push_long(0);
    dk::push_long(a2);
    *x = (uint16_t)(*x + (uint16_t)(g_CameraX >> 16));   /* the 16.16 cameras' integer words */
    *y = (uint16_t)(*y + (uint16_t)(g_CameraY >> 16));
    DropSpawnerFn(spawner, d0, d1, *x, *y, d7, a4, a6, &a2);
    for (int i = 0; i < 5; i++)
        dk::pop_long();                      /* movem.l (a7)+,a2-a6: the entry values stand */
}

/* WeaponDrop_ItemsOut - director native for the op $2D30 switch: 1 while any weapon or power-up
 * pickup is alive (g_WeaponItemCount), else 0.
 * In:  g_WeaponItemCount
 * Out: d0 = 1 (a long), or its low word 0 (the upper word kept: move.w)
 * Evidence: move.w (g_WeaponItemCount).w,d0 / beq / moveq #1,d0; op $2D30 at $13FCC/$13FF4/$1401C/
 *           $1403E/$14060 (the director waits while a pickup is out)
 */
void WeaponDrop_ItemsOut(uint32_t *d0)
{
    uint16_t n = g_WeaponItemCount;
    *d0 = n ? 1u : (*d0 & 0xFFFF0000u);
}
