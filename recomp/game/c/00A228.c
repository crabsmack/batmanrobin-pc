#include "game.h"

/* Scene_ResetBufPtrs - points all 16 scene-script buffer pointers (g_BufPtrs) at the start of RAM,
 * the scene decompression buffer g_SceneBuf.
 * In:  none
 * Out: g_BufPtrs[0..15] = g_SceneBuf
 * Evidence: lea g_SceneBuf,a0 / lea g_BufPtrs,a1 / moveq #$F,d7 / move.l a0,(a1)+ / dbra; caller
 *           Scene_InitCommon; the a6 ops $70E6/$7108/$7120/$7148/$7158 index the table by a word
 */
void Scene_ResetBufPtrs(void)
{
    uint32_t buf = dk::addr(g_SceneBuf) & 0xFFFFFFu;   /* lea (g_SceneBuf&$FFFFFF).l: $00FF0000 */
    PTR(u8) *ptrs = (PTR(u8) *)g_BufPtrs;
    for (int i = 0; i < 16; i++)
        ptrs[i].v = buf;
}
