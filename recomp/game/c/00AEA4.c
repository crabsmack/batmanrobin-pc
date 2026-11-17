#include "game.h"

/* VInt_ScrollModeTbl28 ($00AEA4-$00AEBB): the scroll-mode jump table's modes $28-$3C (the slots
 * VInt_UploadScroll's jmp reads past $24), most of them ending in the H-int raster setup. Each
 * slot hands its registers on unchanged. Runs in the V-int: D2 checks it. */

/* VInt_ScrollModeTbl28 - slot $28: plane A's line DMA, then the raster re-armed.
 * In:  a0 = VDP control port, a2 = DMA scratch
 * Out: as VInt_ScrollLinesARaster
 * Evidence: bra.w $AF84; 8 corpus runs, TAS frames 113312-116145
 */
void VInt_ScrollModeTbl28(uint32_t ctrl, uint32_t scratch)
{
    (void)ctrl; (void)scratch;
    dk::jump(at_VInt_ScrollLinesARaster);
}

/* VInt_ScrollMode2C - slot $2C: line H-scroll plus column V-scroll.
 * In:  a0 = VDP control port, a2 = DMA scratch
 * Out: as VInt_ScrollLinesColumns
 * Evidence: bra.w $AF8C at $AEA8
 */
void VInt_ScrollMode2C(uint32_t ctrl, uint32_t scratch)
{
    (void)ctrl; (void)scratch;
    dk::jump(at_VInt_ScrollLinesColumns);
}

/* VInt_ScrollMode30 - slot $30: plane A's line DMA only.
 * In:  a0 = VDP control port, a2 = DMA scratch
 * Out: as VInt_ScrollLinesA
 * Evidence: bra.w $AF9A at $AEAC
 */
void VInt_ScrollMode30(uint32_t ctrl, uint32_t scratch)
{
    (void)ctrl; (void)scratch;
    dk::jump(at_VInt_ScrollLinesA);
}

/* VInt_ScrollMode34 - slot $34: plane B's line DMA, then the raster re-armed.
 * In:  a0 = VDP control port, a2 = DMA scratch
 * Out: as VInt_ScrollLinesBRaster
 * Evidence: bra.w $AFA4 at $AEB0
 */
void VInt_ScrollMode34(uint32_t ctrl, uint32_t scratch)
{
    (void)ctrl; (void)scratch;
    dk::jump(at_VInt_ScrollLinesBRaster);
}

/* VInt_ScrollMode38 - slot $38: the VSRAM long, both planes' line DMA, the raster re-armed.
 * In:  a0/a1 = VDP ports, a2 = DMA scratch
 * Out: as VInt_ScrollLinesRaster
 * Evidence: bra.w $AFAC at $AEB4
 */
void VInt_ScrollMode38(uint32_t ctrl, uint32_t data, uint32_t scratch)
{
    (void)ctrl; (void)data; (void)scratch;
    dk::jump(at_VInt_ScrollLinesRaster);
}

/* VInt_ScrollMode3C - slot $3C: the whole-plane scroll longs, the raster re-armed.
 * In:  a0/a1 = VDP ports
 * Out: as VInt_ScrollPlanesRaster
 * Evidence: bra.w $AFBE at $AEB8
 */
void VInt_ScrollMode3C(uint32_t ctrl, uint32_t data)
{
    (void)ctrl; (void)data;
    dk::jump(at_VInt_ScrollPlanesRaster);
}
