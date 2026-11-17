#include "game.h"

/* TextMap_CmdMenuItem - text-map command $EA: a menu item: runs one of two sub-lists, the
 * highlighted one while the item is the cursor's and the 8-frame blink phase is on, else the
 * normal one; their TextMap_CmdReturn comes back after the operands.
 * In:  list = the item index, the normal sub-list's word offset, the highlighted one's
 * Out: the return point (past the 3 words) pushed; d0 low word = the item index; then
 *      TextMap_CmdGoto on the chosen offset
 * Evidence: pea 6(a0) / move.w (a0)+,d0 / cmp.w (g_MenuCursor).w,d0 / bne TextMap_CmdGoto / btst
 *           #3,(g_FrameCounter+3).w / beq / addq.l #2,a0; options items '00EA 0000 001E 0036';
 *           scenario-options and 4 runs
 */
void TextMap_CmdMenuItem(uint32_t *d0, uint16_t *d6, uint16_t *d7, const u8 **list, uint32_t *a1, uint32_t *a2,
                         uint32_t *a3, uint32_t *a4, uint32_t *a5, uint32_t *a6)
{
    dk::push_long(dk::addr(*list + 6));
    uint16_t item = *(const u16 *)*list;
    *list += 2;
    *d0 = (*d0 & 0xFFFF0000u) | item;
    if (item == g_MenuCursor && (g_FrameCounter & 0x08u))
        *list += 2;                          /* the highlighted sub-list's offset */
    dk::jump(at_TextMap_CmdGoto);
}
