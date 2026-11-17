#include "game.h"

/* Mot_JumpGravity - reads a height word and returns the gravity of a jump that rises that many
 * pixels in the time word that follows (Mot_JumpGravityD1, the next file, falls through).
 * In:  script = word height, word time; vm (speed factor, tick squared)
 * Out: height = the word, zero-extended; then as Mot_JumpGravityD1 (jumped to)
 * Evidence: moveq #0,d1 / move.w (a2)+,d1, falling into $1D82; callers Op2_ArcX,
 *           Op2_SetJumpGravity, Op2_JumpAnim, Op2_Jump, Op2_FallAnim
 */
void Mot_JumpGravity(uint32_t *d0, uint32_t *height, uint32_t *d2, uint16_t *d3, const u16 **script,
                     const Vm2Block *vm)
{
    *height = *(*script)++;
    dk::jump(at_Mot_JumpGravityD1);
}
