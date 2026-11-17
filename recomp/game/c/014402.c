#include "game.h"

/* TextMap_CmdText - text-map command $58: writes a run of characters (a count byte, then the
 * ASCII) as font tiles at the write position, then realigns the list to a word.
 * In:  list = the count byte
 * Out: count = it (a long: moveq then move.b); then TextMap_PutCharsNext (its dbra counts the
 *      first character off)
 * Evidence: moveq #0,d7 / move.b (a0)+,d7 / bra $1442E; '0058 07 OPTIONS' in Tbl_0498EA; 9 runs
 */
void TextMap_CmdText(uint32_t *d0, uint16_t *d6, uint32_t *count, const u8 **list, uint32_t *a1, uint32_t *a2,
                     uint32_t *a3, uint32_t *a4, uint32_t *a5, uint32_t *a6)
{
    *count = *(*list)++;
    dk::jump(at_TextMap_PutCharsNext);
}
