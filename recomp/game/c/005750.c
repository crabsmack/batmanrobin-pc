#include "game.h"

/* Op4_OffsetSpawnXY / Op4_OffsetSpawnY ($005750-$005769): pixel offsets added to the integer
 * words of the spawn position registers. */

/* add a signed pixel word to the integer (high) word of a 16.16 register: swap / add.w / swap */
static uint32_t add_high_word(uint32_t r, uint16_t px)
{
    return (uint32_t)(uint16_t)((r >> 16) + px) << 16 | (r & 0xFFFFu);
}

/* Op4_OffsetSpawnXY - a4 op: adds signed pixel offsets to the integer words of the spawn x/y
 * registers d4/d5.
 * In:  script = word dx, word dy; d4/d5 = spawn x/y (16.16)
 * Out: d4/d5 moved; then as Op4_OffsetSpawnY (NEXT)
 * Evidence: swap d4 / add.w (a4)+,d4 / swap d4, falling into Op4_OffsetSpawnY; script uses $254E6
 *           ($A,$A), $254FE (-$F,-8) after $63A2 and before Op4_JitterSpawnXY + Op4_Spawn; 6 runs
 */
void Op4_OffsetSpawnXY(uint32_t *d4, uint32_t *d5, const u16 **script)
{
    *d4 = add_high_word(*d4, *(*script)++);
    Op4_OffsetSpawnY(d5, script);
}

/* Op4_OffsetSpawnY - a4 op: adds a signed pixel offset to the integer word of the spawn y
 * register d5.
 * In:  script = word dy; d5 = spawn y (16.16)
 * Out: d5 moved; NEXT
 * Evidence: swap d5 / add.w (a4)+,d5 / swap d5 / NEXT; a label inside Op4_OffsetSpawnXY, a
 *           computed-jump target in 5 corpus runs; script use $433E8 (`5756 0014`)
 */
void Op4_OffsetSpawnY(uint32_t *d5, const u16 **script)
{
    *d5 = add_high_word(*d5, *(*script)++);
    dk::jump(dk::code((int16_t)(uint16_t)*(*script)++));
}
