#include "game.h"

/* VInt_ScrollModeTbl ($00AE7C-$00AE8F): the V-int's scroll-mode jump table, modes 0-$10, one
 * bra.w per 4 bytes, entered by VInt_UploadScroll's jmp. Each slot hands its registers (the VDP
 * ports a0/a1, the DMA scratch a2) on unchanged. Runs in the V-int: D2 checks it. */

/* VInt_ScrollModeTbl - slot 0: the display back on, no scroll upload.
 * In:  a0 = VDP control port
 * Out: as VInt_ScrollDisplayOn
 * Evidence: bra.w $AF04; mode 0 entered in TAS frames 21158-35416
 */
void VInt_ScrollModeTbl(uint32_t ctrl)
{
    (void)ctrl;
    dk::jump(at_VInt_ScrollDisplayOn);
}

/* VInt_ScrollMode04 - slot 4: the scene's own upload routine.
 * In:  as VInt_UploadScroll
 * Out: as VInt_ScrollCustom
 * Evidence: bra.w $AF0A at $AE80
 */
void VInt_ScrollMode04(void)
{
    dk::jump(at_VInt_ScrollCustom);
}

/* VInt_ScrollMode08 - slot 8: the whole-plane scroll longs.
 * In:  a0/a1 = VDP control/data ports
 * Out: as VInt_ScrollPlanes
 * Evidence: bra.w $AF10 at $AE84
 */
void VInt_ScrollMode08(uint32_t ctrl, uint32_t data)
{
    (void)ctrl; (void)data;
    dk::jump(at_VInt_ScrollPlanes);
}

/* VInt_ScrollMode0C - slot $C: the VSRAM long and the per-line H-scroll DMA (gameplay's mode).
 * In:  a0/a1 = VDP ports, a2 = DMA scratch
 * Out: as VInt_ScrollLines
 * Evidence: bra.w $AF2E at $AE88
 */
void VInt_ScrollMode0C(uint32_t ctrl, uint32_t data, uint32_t scratch)
{
    (void)ctrl; (void)data; (void)scratch;
    dk::jump(at_VInt_ScrollLines);
}

/* VInt_ScrollMode10 - slot $10: the VSRAM long and the line-pair H-scroll DMA.
 * In:  a0/a1 = VDP ports, a2 = DMA scratch
 * Out: as VInt_ScrollLinePairs
 * Evidence: bra.w $AF42 at $AE8C
 */
void VInt_ScrollMode10(uint32_t ctrl, uint32_t data, uint32_t scratch)
{
    (void)ctrl; (void)data; (void)scratch;
    dk::jump(at_VInt_ScrollLinePairs);
}
