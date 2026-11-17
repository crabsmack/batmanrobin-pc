#include "game.h"

/* Pad_CheckStart - a2 native test: whether Start was just pressed on pad 1 or 2 (pad 1 first),
 * recording which pad in g_StartPad.
 * In:  g_Pad1, g_Pad2 (newly pressed, bit 7 = Start)
 * Out: 1 and g_StartPad = 1 or 2 when pressed, else 0
 * Evidence: btst #7,$F626 / beq / move.b #1,g_StartPad / moveq #1,d0; the same for $F62A and 2;
 *           moveq #0,d0; op $2D40 (`dc.l $A196`) in the continue screen script $491F2
 */
uint32_t Pad_CheckStart(void)
{
    if (((const PadRec *)g_Pad1)->pressed & 0x80) {
        g_StartPad = 1;
        return 1;
    }
    if (((const PadRec *)g_Pad2)->pressed & 0x80) {
        g_StartPad = 2;
        return 1;
    }
    return 0;
}
