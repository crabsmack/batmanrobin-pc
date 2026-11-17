#include "game.h"

/* a 68000 address held in a register, as a pointer (PTR(T): the 68000 value kept) */
template <class T> static T *ptr(uint32_t a)
{
    PTR(T) p;
    p.v = a;
    return p;
}

/* Fx_RunScriptArg - runs the a4 spawn script the script's next dc.l names (Fx_RunScript, which it
 * falls into).
 * In:  a2 = the script at the dc.l; d2-d5 = the new objects' radius, heading, x, y (Fx_RunScript)
 * Out: a0 = the a4 script, a2 past the dc.l; then Fx_RunScript
 * Evidence: movea.l (a2)+,a0, falling into Fx_RunScript (the next file); the tail of the
 *           Fx_SpawnAt* callbacks; $1A04C, $25E4C, $29534. Prototype: a0 an output (measured a
 *           clobber): the jump hands the script to Fx_RunScript
 */
void Fx_RunScriptArg(uint32_t d0, uint32_t d1, uint32_t d2, uint32_t d3, uint32_t d4, uint32_t d5, uint32_t d6,
                     uint32_t d7, uint32_t *a0, uint32_t a1, uint32_t *a2, uint32_t a3, uint32_t a4, uint32_t a5,
                     uint32_t a6)
{
    (void)d0, (void)d1, (void)d2, (void)d3, (void)d4, (void)d5, (void)d6, (void)d7, (void)a1, (void)a3, (void)a4,
        (void)a5, (void)a6;
    *a0 = *ptr<const u32>(*a2);
    *a2 += 4;
    dk::jump(at_Fx_RunScript);
}
