#include "game.h"

/* Pad_ReadAll - the V-int's full-path pad read: g_Pad1 / g_Pad2 from ports 1 and 2 (Pad_Update),
 * then both records copied to g_PadLive1 / g_PadLive2.
 * In:  the pad ports
 * Out: g_Pad1, g_Pad2, g_PadLive1, g_PadLive2
 * Evidence: lea g_Pad1,a5 / lea $A10003,a4 / bsr Pad_Update / lea $A10005,a4 / bsr Pad_Update /
 *           move.l g_Pad1,g_PadLive1 / move.l g_Pad2,g_PadLive2; bsr from VInt_Handler between the
 *           Z80 bus request and release
 */
void Pad_ReadAll(void)
{
    PadRec *rec = (PadRec *)g_Pad1;
    Pad_Update(hw::IO_DATA1, &rec);               /* rec now g_Pad2 */
    Pad_Update(hw::IO_DATA2, &rec);
    *(PadRec *)g_PadLive1 = *(const PadRec *)g_Pad1;
    *(PadRec *)g_PadLive2 = *(const PadRec *)g_Pad2;
}
