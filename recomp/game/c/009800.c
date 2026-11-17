#include "game.h"

/* ScrollJt_LayerA - parallax command 4's jump-table slot: on to ScrollCmd_LayerA.
 * In:  list = past the command word (a5)
 * Out: as ScrollCmd_LayerA
 * Evidence: bra.w ScrollCmd_LayerA, 4 bytes after the table base $97FC; Tbl_00970A's first command
 */
void ScrollJt_LayerA(uint32_t *d0, uint32_t d1, uint32_t *d2, uint32_t *d3, uint16_t *d6, uint16_t *d7, uint32_t *bands,
                     const u16 **list, uint32_t *layer)
{
    dk::jump(at_ScrollCmd_LayerA);
}
