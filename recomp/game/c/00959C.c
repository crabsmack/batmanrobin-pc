#include "game.h"

/* Shake_StartIfIdle - native callback: starts the standard screen shake (Shake_Start, which it falls
 * into) unless one is running.
 * In:  none
 * Out: a shake running (g_ShakeY != 0): the tst.l flags (Z clear), nothing changed; else Shake_Start's
 * Evidence: tst.l g_ShakeY / bne.s Shake_Done, falling into Shake_Start (the next file); script use
 *           $1C5E8 (TAS f19339-f20434). Prototype: &ccr added (the tst.l's flags on the early exit)
 */
void Shake_StartIfIdle(dk::Flags *ccr)
{
    *ccr = dk::Flags::test(g_ShakeY, 4, ccr->x);
    if (g_ShakeY != 0) {
        dk::jump(at_Shake_Done);
        return;
    }
    dk::jump(at_Shake_Start);
}
