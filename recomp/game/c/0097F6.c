#include "game.h"

/* The parallax command dispatcher ($0097F6-$0097FF): a command word, then a jump through the
 * 4-byte slots that start at ScrollJt_End (each slot a bra.w to its command, which branches back
 * here; slot 0 is the rts that ends the list). */

/* Scroll_NextCmd - fetches the next parallax command word and jumps to its slot in the jump table
 * at ScrollJt_End (0 = end of the list: the slot is ScrollJt_End's rts).
 * In:  list = the command list (a5)
 * Out: cmd = the command word (d0's low word); list past it; jumped to the slot
 * Evidence: move.w (a5)+,d0 / jmp $97FC(pc,d0.w); commands 0, 4, $C, $14, $18 seen (NOTES
 *           "Parallax line scroll"); also called directly by $33398, $475C2
 */
void Scroll_NextCmd(uint32_t *cmd, uint32_t d1, uint32_t *d2, uint32_t *d3, uint16_t *d6, uint16_t *d7,
                    uint32_t *bands, const u16 **list, uint32_t *layer)
{
    uint16_t c = *(*list)++;
    *cmd = (*cmd & 0xFFFF0000u) | c;
    dk::jump(at_ScrollJt_End + (uint32_t)(int32_t)(int16_t)c);
}

/* ScrollJt_End - parallax command 0: the jump table's base, an rts that ends the command list (the
 * return goes to whoever called Scroll_RunParallax / Scroll_NextCmd).
 * In:  none
 * Out: none
 * Evidence: jmp $97FC(pc,d0.w) with d0 = 0 lands on this rts; every command list ends with 0
 */
void ScrollJt_End(void)
{
}
