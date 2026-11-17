#include "game.h"

/* HudOp_Wait - panel script op 4: waits the operand's number of frames, then goes on with the
 * next op.
 * In:  a0 = the operand (word frames); rec = the player record (a6)
 * Out: +$1E = the frames, +$20 = the next op
 * Evidence: move.w (a0)+,$1E(a6) / move.l a0,$20(a6) / rts; entry 4 of Tbl_011764; 'wait 4' in
 *           Tbl_010F76
 */
void HudOp_Wait(uint32_t a0, PlayerRec *rec)
{
    PTR(const u16) operand;
    operand.v = a0;
    rec->panel_timer = *operand.get();
    rec->panel_script = a0 + 2;
}
