#include "game.h"

/* Pal_LoadLine0 - copies a 16-colour palette into line 0 of g_PalSource and flags the line for
 * Fade_Step to rebuild.
 * In:  src (a0) = 16 CRAM words (ROM or RAM)
 * Out: g_PalSource line 0, g_Pal0Reload = $FF; src += 32, a1 = the end of the line; ccr of the
 *      last move.l (st.b leaves the condition codes)
 * Evidence: lea g_PalSource,a1 / 8x move.l (a0)+,(a1)+ / st.b g_Pal0Reload / rts; callers the a6
 *           op $7186, Title_PaletteFadeRun and the stage inits ($1FB3A, $1FECC, $32F14 ...); TAS 25
 *           frames
 */
void Pal_LoadLine0(uint32_t *src, uint32_t *a1, dk::Flags *ccr)
{
    PTR(const u32) at;
    at.v = *src;
    const u32 *from = at;
    u32 *to = (u32 *)g_PalSource;
    uint32_t last = 0;
    for (int i = 0; i < 8; i++)
        to[i] = last = from[i];
    *src += 32;
    *a1 = ADDR68(to + 8);
    g_Pal0Reload = 0xFF;
    *ccr = dk::Flags::test(last, 4, ccr->x);
}
