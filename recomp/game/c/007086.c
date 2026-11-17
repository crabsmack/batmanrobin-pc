#include "game.h"

/* VM6_RunHook - runs the a6 loader script's per-frame resume hook: jumps to g_A6ResumeHook when it
 * is set (VM6_Resume, or a waiting op's own routine), otherwise returns.
 * In:  g_A6ResumeHook = the hook or 0; every register passes on to the hook (the script it resumes
 *      runs on until an op parks or ends, then returns to Frame_End)
 * Out: d0 = the hook; a0 = the hook, jumped to
 * Evidence: move.l (g_A6ResumeHook).w,d0 / bne / rts / movea.l d0,a0 / jmp (a0); only caller
 *           Frame_End; hooks seen $70A8, $70D2, $713E, $72D2, $72F4, $743A, $756E, $7590, $76AE;
 *           169 corpus runs, every TAS frame from 18
 */
void VM6_RunHook(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6,
                 uint32_t *d7, uint32_t *a0, uint32_t *a1, uint32_t *a2, uint32_t *a3, uint32_t *a4, uint32_t *a5,
                 uint32_t *a6)
{
    (void)d1, (void)d2, (void)d3, (void)d4, (void)d5, (void)d6, (void)d7;
    (void)a1, (void)a2, (void)a3, (void)a4, (void)a5, (void)a6;
    *d0 = g_A6ResumeHook;
    if (*d0 == 0)
        return;
    *a0 = *d0;
    dk::jump(*a0);
}
