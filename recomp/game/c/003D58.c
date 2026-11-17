#include "game.h"

/* Op2_BranchStatGE - the shared tail of the player-stat branch ops: branches (Op2_Goto) when the
 * stat is at least the word operand (signed), else NEXT after the operands.
 * In:  stat = the stat word (Op2_BranchWeaponLvlMaxGE, the never-run stat stubs $3CDC-$3D54);
 *      script = word value, word offset
 * Out: the branch at Op2_Goto with the script on the offset word, else handler = NEXT's
 * Evidence: cmp.w (a2)+,d0 / bge.w Op2_Goto / addq.l #2,a2 / NEXT; reached by bra from $3CEE
 *           (42 corpus runs) and the stubs before it
 */
void Op2_BranchStatGE(uint16_t stat, uint32_t *handler, const u16 **script)
{
    if ((int16_t)stat >= (int16_t)*(*script)++) {
        dk::jump(at_Op2_Goto);
        return;
    }
    (*script)++;                           /* the offset */
    *handler = dk::code((int16_t)(uint16_t)*(*script)++);
    dk::jump(*handler);
}
