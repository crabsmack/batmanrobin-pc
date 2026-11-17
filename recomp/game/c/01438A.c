#include "game.h"

/* TextMap_RunInline - a2 native: runs the text-map command list named by the script's next dc.l
 * (falls into TextMap_Run, the next file).
 * In:  script = at the dc.l (the list's address)
 * Out: list = it, script past the long; then TextMap_Run
 * Evidence: movea.l (a2)+,a0, then TextMap_Run at $1438C; dc.l $0001438A in the ending scripts
 *           ($4A60A, $4A750, $4A784 ...); scenario-ending
 */
void TextMap_RunInline(uint32_t *d0, uint16_t *d6, uint16_t *d7, uint32_t *list, uint32_t *a1, const u32 **script,
                       uint32_t *a3, uint32_t *a4, uint32_t *a5, uint32_t *a6)
{
    *list = *(*script)++;
    dk::jump(at_TextMap_Run);
}
