#include "game.h"

/* VInt_UploadScroll - the V-int's scroll upload: continues at the slot of VInt_ScrollModeTbl that
 * g_ScrollMode selects; every mode ends by turning the display back on (VDP reg 1 = $64).
 * In:  a0 = VDP control port, a1 = data port, a2 = the DMA command scratch (VInt_Handler's);
 *      g_ScrollMode (a multiple of 4)
 * Out: as the mode's routine
 * Evidence: move.w g_ScrollMode,d0 / jmp VInt_ScrollModeTbl(pc,d0.w); bsr from VInt_Handler every
 *           full V-int (130,055 TAS frames); runs in the V-int, so D2 checks it
 */
void VInt_UploadScroll(uint32_t ctrl, uint32_t data, uint32_t scratch)
{
    (void)ctrl; (void)data; (void)scratch;        /* handed on unchanged to the mode's routine */
    dk::jump(at_VInt_ScrollModeTbl + (int16_t)(uint16_t)g_ScrollMode);
}
