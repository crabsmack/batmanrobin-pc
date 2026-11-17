#include "game.h"

/* HUD_StepIdle - the HUD step jump table's slot 0: no DMA this frame (the nop after its rts pads
 * the slot to 4 bytes; nothing enters it).
 * In:  none
 * Out: none
 * Evidence: rts / nop at $11310; jmp HUD_StepIdle(pc,d0.w) at $1130C with d0 = g_HUDStep = 0 when no
 *           step set one
 */
void HUD_StepIdle(void)
{
}
