#include "game.h"

/* VM4_RunObject - object handler of every a4-VM object: no child spawned yet (a5 = the object
 * itself), the script resumes at the object's saved pc; the ops chain until one returns to the
 * object runner.
 * In:  self = the a4 object (one $5A pool block on the object list)
 * Out: child = self; script = self->resume; then VM4_Next (the next file) fetches its first op
 *      into handler and jumps there
 * Evidence: movea.l a6,a5 / movea.l $8(a6),a4, falling into VM4_Next; lea ($53B4).w,a0 before the
 *           object alloc $8A08 in the spawn cores $60C4/$613A; 98,843 TAS frames
 */
void VM4_RunObject(uint32_t *handler, uint32_t *script, uint32_t *child, Vm4Obj *self)
{
    (void)handler;
    *child = dk::addr(self);
    *script = self->resume.raw();
    dk::jump(at_VM4_Next);
}
