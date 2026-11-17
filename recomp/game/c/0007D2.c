#include "game.h"

/* VM2_RunObject - object handler that runs one frame of an object's a2 script: saves the stack
 * pointer in g_VM2SavedSP, loads the object's motion / VM / sprite blocks from its handles and
 * resumes the script where its VM block says; a yield op restores the stack pointer and returns
 * to this function's caller.
 * In:  self = the object (main or V-int list entry)
 * Out: motion / vm / sprite = its blocks; script = the resume point, past the first op's word;
 *      handler = that op, jumped to (the VM's NEXT: VM2_Next); d0-d7/a1 pass through to the
 *      script, so a C caller gets back what its ops leave (the measured contract ends at the jmp)
 * Evidence: move.l a7,(g_VM2SavedSP).w / movem.w $8(a6),a3-a5 / movea.l $2A(a4),a2, then falls into
 *           VM2_Next; installed as an object handler by $1E70A, called directly by $102D6, $290B6,
 *           $29572 and $2604C; 129k TAS frames
 */
void VM2_RunObject(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6,
                   uint32_t *d7, uint32_t *handler, uint32_t *a1, const u16 **script, void **motion, Vm2Block **vm,
                   void **sprite, Vm2Obj *self)
{
    (void)d0, (void)d1, (void)d2, (void)d3, (void)d4, (void)d5, (void)d6, (void)d7, (void)a1;
    g_VM2SavedSP = dk::stack_pointer();
    *motion = dk::handle(self->motion);
    *vm = dk::handle<Vm2Block>(self->vm);
    *sprite = dk::handle(self->sprite);
    *script = (*vm)->resume;
    VM2_Next(handler, script);
}

/* VM2_Next - the a2 VM's NEXT: the script's next word names the handler of the next op; jump to it.
 * In:  script = at the handler word
 * Out: script past it; handler = it (the ops read it back), jumped to
 * Evidence: movea.w (a2)+,a0 / jmp (a0); the target of 46 op branches ("continue")
 */
void VM2_Next(uint32_t *handler, const u16 **script)
{
    *handler = dk::code((int16_t)(uint16_t)*(*script)++);
    dk::jump(*handler);
}
