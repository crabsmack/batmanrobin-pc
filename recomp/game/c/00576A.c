#include "game.h"

/* Op4_OffsetSpawnHeading - a4 op: adds a signed amount to the integer word of the spawn heading
 * register d3 ($400 units a turn), e.g. to fan out a row of spawns.
 * In:  script = word amount; d3 = spawn heading (16.16)
 * Out: d3 turned; the condition codes of the closing swap; NEXT (handler = the next op)
 * Evidence: swap d3 / add.w (a4)+,d3 / swap d3 / NEXT; 167 a4-script uses, e.g. $13144-$13174
 *           (`576A 0040 5606 ...` repeated: a fan of objects $40 apart); 52 corpus runs
 */
void Op4_OffsetSpawnHeading(uint32_t *d3, uint32_t *handler, const u16 **script, dk::Flags *ccr)
{
    uint16_t whole = (uint16_t)((*d3 >> 16) + *(*script)++);
    *d3 = (uint32_t)whole << 16 | (*d3 & 0xFFFFu);
    *ccr = dk::Flags::test(*d3, 4, ccr->x);
    *handler = dk::code((int16_t)(uint16_t)*(*script)++);
    dk::jump(*handler);
}
