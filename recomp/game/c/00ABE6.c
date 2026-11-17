#include "game.h"

/* Pal_LoadLine1 - copies a 16-colour palette into line 1 of g_PalSource and flags the line for
 * Fade_Step to rebuild.
 * In:  src (a0) = 16 CRAM words (ROM or RAM)
 * Out: g_PalSource1 ($E0F2), g_Pal1Reload = $FF; src += 32, a1 = the end of the line; ccr of the
 *      last move.l (st.b leaves the condition codes)
 * Evidence: lea g_PalSource1,a1 / 8x move.l (a0)+,(a1)+ / st.b g_Pal1Reload / rts; callers
 *           Op2_LoadPalette1, the a6 op $7186, $20DDA, Title_PaletteFadeRun; TAS 967 frames
 */
void Pal_LoadLine1(uint32_t *src, uint32_t *a1, dk::Flags *ccr)
{
    PTR(const u32) at;
    at.v = *src;
    const u32 *from = at;
    u32 *to = (u32 *)g_PalSource1;
    uint32_t last = 0;
    for (int i = 0; i < 8; i++)
        to[i] = last = from[i];
    *src += 32;
    *a1 = ADDR68(to + 8);
    g_Pal1Reload = 0xFF;
    *ccr = dk::Flags::test(last, 4, ccr->x);
}
