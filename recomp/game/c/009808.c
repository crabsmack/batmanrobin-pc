#include "game.h"

/* ScrollJt_CameraA - parallax command $C's jump-table slot: on to ScrollCmd_CameraA.
 * In:  list = past the command word (a5)
 * Out: as ScrollCmd_CameraA
 * Evidence: bra.w ScrollCmd_CameraA at offset $C from the table base $97FC; stage 4's lists
 */
void ScrollJt_CameraA(uint32_t *d0, uint32_t d1, uint32_t *d2, uint32_t *d3, uint16_t *d6, uint16_t *d7, uint32_t *bands,
                      const u16 **list, uint32_t *layer)
{
    dk::jump(at_ScrollCmd_CameraA);
}
