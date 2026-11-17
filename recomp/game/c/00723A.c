#include "game.h"

/* The a6 plane-fill ops: set up a fill of the blank tile $5FC (VM6_SetupClearRect) and hand it to
 * a V-int job (VM6_StartClearJob, the next files), which parks the script until it is done. */

/* set up the fill from d0-d3 (x bytes, row, width, height; the upper words as the asm left them)
   and the script's plane word, then start the job */
static void clear_rect(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *a0, uint32_t *a6)
{
    uint16_t w2 = (uint16_t)*d2, w3 = (uint16_t)*d3;
    VM6_SetupClearRect((uint16_t)*d0, d1, &w2, &w3, a0, a6);
    *d2 = (*d2 & 0xFFFF0000u) | w2;
    *d3 = (*d3 & 0xFFFF0000u) | w3;
    dk::jump(at_VM6_StartClearJob);
}

/* Op6_ClearPlane - a6 op: fills a whole VDP plane with the blank tile $5FC through a V-int job (a
 * few rows per V-int), parking the script until it is done.
 * In:  a6 = script: word address of the plane VRAM variable (g_PlaneAVRAM / g_PlaneBVRAM)
 * Out: d0-d3 = 0 then as VM6_SetupClearRect leaves d1-d3 and a0; VM6_StartClearJob
 * Evidence: moveq #0,d0-d3 / bsr VM6_SetupClearRect / bra VM6_StartClearJob; TAS f77148 only
 */
void Op6_ClearPlane(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *a0, uint32_t *a1,
                    uint32_t *a6)
{
    (void)a1;
    *d0 = *d1 = *d2 = *d3 = 0;
    clear_rect(d0, d1, d2, d3, a0, a6);
}

/* Op6_ClearPlaneRect - a6 op: fills a rectangle of a VDP plane with the blank tile $5FC through a
 * V-int job, parking the script until it is done.
 * In:  a6 = script: words x (bytes), row, width, height (cells; 0 = the plane's), word address of
 *      the plane VRAM variable
 * Out: d0-d3 = the four words sign-extended (movem.w), then as Op6_ClearPlane
 * Evidence: movem.w (a6)+,d0-d3 / bsr VM6_SetupClearRect / bra VM6_StartClearJob; TAS f32080 and
 *           fuzz-30600-s2
 */
void Op6_ClearPlaneRect(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *a0, uint32_t *a1,
                        uint32_t *a6)
{
    (void)a1;
    PTR(const u16) pc;
    pc.v = *a6;
    const u16 *script = pc;
    *d0 = (uint32_t)(int32_t)(int16_t)vm6_word(&script);
    *d1 = (uint32_t)(int32_t)(int16_t)vm6_word(&script);
    *d2 = (uint32_t)(int32_t)(int16_t)vm6_word(&script);
    *d3 = (uint32_t)(int32_t)(int16_t)vm6_word(&script);
    *a6 = ADDR68(script);
    clear_rect(d0, d1, d2, d3, a0, a6);
}
