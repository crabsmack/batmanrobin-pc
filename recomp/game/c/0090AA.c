#include "game.h"

/* a 68000 address held in a register, as a pointer (PTR(T): the 68000 value kept) */
template <class T> static T *ptr(uint32_t a)
{
    PTR(T) p;
    p.v = a;
    return p;
}

/* Fx_SpawnAimed - native callback: runs an a4 spawn script at a mirrored offset, the new objects
 * heading the object's way.
 * In:  a3 = the motion block (+$C heading, 16.16); a2 = the script: dc.w dx, dy, dc.l the a4 script
 * Out: d3 = the heading; then Fx_SpawnAtOfsWithAngle
 * Evidence: move.l $C(a3),d3 / bra.s Fx_SpawnAtOfsWithAngle; script uses $3282A-$328F4
 */
void Fx_SpawnAimed(uint32_t *d0, uint32_t d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t d6,
                   uint32_t d7, uint32_t a1, uint32_t *a2, uint32_t a3, uint32_t a4, uint32_t a5, uint32_t a6)
{
    (void)d0, (void)d1, (void)d2, (void)d4, (void)d5, (void)d6, (void)d7, (void)a1, (void)a2, (void)a4, (void)a5,
        (void)a6;
    *d3 = (uint32_t)ptr<const Vm2Motion>(a3)->heading;
    dk::jump(at_Fx_SpawnAtOfsWithAngle);
}
