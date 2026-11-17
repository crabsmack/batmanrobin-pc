#include "game.h"

/* TextMap_CmdEnd - text-map command 0: ends the list: restores the caller's a1-a6 (pushed by
 * TextMap_Run), leaves typewriter mode and returns to TextMap_Run's caller.
 * In:  the six longs TextMap_Run pushed
 * Out: a1-a6 = them; g_TextMapTyping = 0
 * Evidence: movem.l (a7)+,a1-a6 / sf.b (g_TextMapTyping).w / rts; offset $0000 ends every list in
 *           Tbl_0498EA; 9 runs
 */
void TextMap_CmdEnd(uint32_t *a1, uint32_t *a2, uint32_t *a3, uint32_t *a4, uint32_t *a5, uint32_t *a6)
{
    *a1 = dk::pop_long();
    *a2 = dk::pop_long();
    *a3 = dk::pop_long();
    *a4 = dk::pop_long();
    *a5 = dk::pop_long();
    *a6 = dk::pop_long();
    g_TextMapTyping = 0;
}
