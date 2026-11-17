#include "game.h"

/* Op2_GotoVector2Plus - a2 op: the script goes on at script vector 2 plus a word displacement (an
 * entry inside the block the vector points to).
 * In:  script = word displacement (bytes, signed); self->vector[2]
 * Out: script = vector 2 + displacement, past its handler word; NEXT there
 * Evidence: movea.w (a2),a2 / adda.l $3A(a6),a2 / NEXT; the never-run $3B5A/$3B68 after it are
 *           the gosub forms for vectors 0/1; 16 corpus runs; $1B05C `3B50 0008`
 */
void Op2_GotoVector2Plus(const u16 **script, Vm2Obj *self)
{
    int16_t displacement = (int16_t)(uint16_t)**script;
    *script = (const u16 *)((const u8 *)(const u16 *)self->vector[2] + displacement);
    dk::jump(dk::code((int16_t)(uint16_t)*(*script)++));
}
