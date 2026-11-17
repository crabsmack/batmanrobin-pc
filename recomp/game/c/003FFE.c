#include "game.h"

/* Op2_SetSprVar0 - a2 op: sets sprite variable 0, one of three words (+$22/$24/$26) the player
 * natives step as animation phases.
 * In:  script = word value
 * Out: spr->var[0]; NEXT
 * Evidence: move.w (a2)+,$22(a5) / NEXT; the never-run ops after it clear / or / and / eor / inc /
 *           dec / test it; $104D8 steps $22(a5); 91 corpus runs; $48DCA `3FFE 0020 3FE4`
 */
void Op2_SetSprVar0(const u16 **script, Vm2Sprite *spr)
{
    spr->var[0] = (int16_t)*(*script)++;
    dk::jump(dk::code((int16_t)(uint16_t)*(*script)++));
}
