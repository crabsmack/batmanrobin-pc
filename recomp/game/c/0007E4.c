#include "game.h"

/* Op2_SkipWord - a2 op, also the shared not-taken tail of the conditional branch ops: skips one
 * script word (the branch offset), then NEXT: the following word names the next op's handler.
 * In:  script = the word to skip
 * Out: script past the handler word; next = the handler (the ops read it back), jumped to
 * Evidence: addq.l #2,a2 / movea.w (a2)+,a0 / jmp (a0); the not-taken path of Op2_TickTimer0Branch
 *           ($EC0), $F9A, $1074 and $146C; runs on 86k TAS frames
 */
void Op2_SkipWord(uint32_t *next, const u16 **script)
{
    (*script)++;
    *next = dk::code((int16_t)(uint16_t)*(*script)++);
    dk::jump(*next);
}
