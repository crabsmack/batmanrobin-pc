#include "game.h"

/* VM6_Run - runs an a6 loader script right now (a scene setup): jumps to its first op; the ops
 * chain until one parks the script for Frame_End (a background decompression, a V-int job, a
 * queued DMA) or Op6_End returns to the caller.
 * In:  a6 = the script; every other register passes on to the ops (natives read and write them)
 * Out: a6 past the handler word; the op, jumped to; returns when the script parks or ends
 * Evidence: movea.w (a6)+,a0 / jmp (a0); jsr from 39 scene and section setups (Stage1_RunStreet
 *           with Tbl_01FAB2 `7108 0000 0005 E63C 7604 ...`, $1FC50, $32F14); 40 corpus runs
 */
void VM6_Run(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6,
             uint32_t *d7, uint32_t *a1, uint32_t *a2, uint32_t *a3, uint32_t *a4, uint32_t *a5, uint32_t *a6)
{
    (void)d0, (void)d1, (void)d2, (void)d3, (void)d4, (void)d5, (void)d6, (void)d7;
    (void)a1, (void)a2, (void)a3, (void)a4, (void)a5;
    PTR(const u16) script;
    script.v = *a6;
    uint32_t op = dk::code((int16_t)(uint16_t)*script++);
    *a6 = script.raw();
    dk::jump(op);
}
