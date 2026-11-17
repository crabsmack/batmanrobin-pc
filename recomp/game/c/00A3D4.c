#include "game.h"

/* Pad_ReadLive - the V-int's busy-path pad read: g_PadLive1 / g_PadLive2 straight from ports 1 and
 * 2, so the pause can poll the pads while the main loop is held; pad 2 by falling into Pad_Update.
 * In:  the pad ports
 * Out: g_PadLive1, g_PadLive2 (g_Pad1/g_Pad2 untouched); port (a4) = port 2's data register and
 *      rec (a5) = g_PadLive2 for the fall into Pad_Update (which leaves rec past it)
 * Evidence: lea g_PadLive1,a5 / lea $A10003,a4 / bsr Pad_Update / lea $A10005,a4, then the fall
 *           into Pad_Update; only from VInt_BusyPath. The prototype hands a4/a5 back (measured as
 *           clobbers: no caller reads them) because Pad_Update, jumped to, takes them.
 */
void Pad_ReadLive(uint32_t *port, uint32_t *rec)
{
    PadRec *r = (PadRec *)g_PadLive1;
    Pad_Update(hw::IO_DATA1, &r);                 /* r now g_PadLive2 */
    *rec = dk::addr(r);
    *port = hw::IO_DATA2;
    dk::jump(at_Pad_Update);
}
