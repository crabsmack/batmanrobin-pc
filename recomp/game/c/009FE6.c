#include "game.h"

/* Scene_RunList - runs the scene routines of a 0-terminated list of longs in turn, while
 * g_SceneResult stays 0 (a scene that ends with a result stops the list).
 * In:  list (a0) = the routines; the registers pass through to them
 * Out: as the routines hand the registers back
 * Evidence: move.l (a0)+,d0 / beq / pea (a0) / movea.l d0,a0 / jsr (a0) / movea.l (a7)+,a0 / cmpi.w
 *           #0,g_SceneResult / beq; lists Tbl_00A136, Tbl_00A152 (Main_*), Tbl_00A166-Tbl_00A18A
 *           (Stage_Intro*). The list pointer is kept on the stack across the call, as the asm does.
 */
void Scene_RunList(uint32_t *d1, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, const u32 *list, uint32_t *a1,
                   uint32_t *a2, uint32_t *a3, uint32_t *a4, uint32_t *a5, uint32_t *a6)
{
    for (;;) {
        uint32_t routine = *list++;
        if (routine == 0)
            return;
        dk::push_long(dk::addr(list));            /* pea (a0) */
        SceneFn(routine, d1, d3, d4, d5, d6, a1, a2, a3, a4, a5, a6);
        PTR(const u32) saved;                     /* movea.l (a7)+,a0 */
        saved.v = dk::pop_long();
        list = saved;
        if (g_SceneResult != 0)
            return;
    }
}
