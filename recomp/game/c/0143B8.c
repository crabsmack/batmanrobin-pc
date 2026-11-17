#include "game.h"

/* TextMap_CmdGoto - text-map command $E: the list continues at the word offset that follows
 * (from the offset word itself).
 * In:  list = at the offset word
 * Out: list = the offset word + its value; then TextMap_NextCmd
 * Evidence: adda.w (a0),a0 / bra TextMap_NextCmd; also the tail of TextMap_CmdMenuItem and
 *           TextMap_CmdBlinkGoto; scenario-options
 */
void TextMap_CmdGoto(uint32_t *d0, uint16_t *d6, uint16_t *d7, const u8 **list, uint32_t *a1, uint32_t *a2,
                     uint32_t *a3, uint32_t *a4, uint32_t *a5, uint32_t *a6)
{
    *list += (int16_t)(uint16_t)*(const u16 *)*list;
    dk::jump(at_TextMap_NextCmd);
}

/* TextMap_CmdReturn - text-map command $12: back from a sub-list TextMap_CmdMenuItem entered: the
 * list pointer it pushed (past its operands) is popped.
 * In:  the long TextMap_CmdMenuItem pushed
 * Out: list = it; then TextMap_NextCmd
 * Evidence: movea.l (a7)+,a0 / bra TextMap_NextCmd; pairs with the pea 6(a0) of TextMap_CmdMenuItem;
 *           computed entry in scenario-level-skip and 4 other runs
 */
void TextMap_CmdReturn(uint32_t *d0, uint16_t *d6, uint16_t *d7, const u8 **list, uint32_t *a1, uint32_t *a2,
                       uint32_t *a3, uint32_t *a4, uint32_t *a5, uint32_t *a6)
{
    PTR(const u8) back;
    back.v = dk::pop_long();
    *list = back;
    dk::jump(at_TextMap_NextCmd);
}
