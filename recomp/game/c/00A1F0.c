#include "game.h"

/* BgTask_Start - starts the LZ decompressor as the background task unless one is active: saves a
 * context whose PC is Lz_Decompress, whose SR is the caller's and whose registers carry its
 * arguments, for BgTask_Resume's fake RTE.
 * In:  a0 = packed data, a1 = destination, d1, a3 (saved with them); g_BgTaskActive
 * Out: a task active: d0 unchanged, flags of tst.b g_BgTaskActive (Z clear); else
 *      g_BgTaskActive = $FF, a4 = Lz_Decompress, g_BgTaskContext = d0 (low word: SR), d1, a0, a1,
 *      a3, a4; d0 = 0, flags of moveq #0 (Z set)
 * Evidence: tst.b / bne / st.b g_BgTaskActive / lea Lz_Decompress,a4 / move.w sr,d0 / movem.l
 *           d0-d2/d6-d7/a0-a4,g_BgTaskContext / moveq #0,d0; ruling 2 (dkit.toml dont_care): d0's
 *           high word, the saved CCR, d2, d6, d7 and a2 in the context are never read back, so
 *           the C leaves those bytes alone; the SR's system byte is live (the resume's RTE)
 */
void BgTask_Start(uint32_t *d0, uint32_t d1, uint32_t a0, uint32_t a1, uint32_t a3, uint32_t *a4, dk::Flags *ccr)
{
    if (g_BgTaskActive) {
        *ccr = dk::Flags::test(g_BgTaskActive, 1, ccr->x);
        return;
    }
    g_BgTaskActive = 0xFF;
    *a4 = at_Lz_Decompress;
    *d0 = (*d0 & 0xFFFF0000u) | dk::status_register();   /* move.w sr,d0 */
    BgTaskContext *ctx = (BgTaskContext *)g_BgTaskContext;
    ctx->d0 = *d0;
    ctx->d1 = d1;
    ctx->a0 = a0;
    ctx->a1 = a1;
    ctx->a3 = a3;
    ctx->a4 = *a4;
    *d0 = 0;
    *ccr = dk::Flags::test(0, 4, ccr->x);   /* moveq #0,d0 */
}
