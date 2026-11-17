#include "game.h"

/* Two slots of the parallax jump table ($009810-$009817), commands $14 and $18. */

/* ScrollJt_CameraB - parallax command $14's jump-table slot: on to ScrollCmd_CameraB.
 * In:  list = past the command word (a5)
 * Out: as ScrollCmd_CameraB
 * Evidence: bra.w ScrollCmd_CameraB at offset $14 from the table base $97FC; Tbl_00970A's second
 */
void ScrollJt_CameraB(uint32_t *d0, uint32_t d1, uint32_t *d2, uint32_t *d3, uint16_t *d6, uint16_t *d7, uint32_t *bands,
                      const u16 **list, uint32_t *layer)
{
    dk::jump(at_ScrollCmd_CameraB);
}

/* ScrollJt_CameraOfsB - parallax command $18's jump-table slot: on to ScrollCmd_CameraOfsB.
 * In:  list = past the command word (a5)
 * Out: as ScrollCmd_CameraOfsB
 * Evidence: bra.w ScrollCmd_CameraOfsB at $9814, offset $18 from the table base $97FC
 */
void ScrollJt_CameraOfsB(uint32_t *d0, uint32_t d1, uint32_t *d2, uint32_t *d3, uint16_t *d6, uint16_t *d7,
                         uint32_t *bands, const u16 **list, uint32_t *layer)
{
    dk::jump(at_ScrollCmd_CameraOfsB);
}
