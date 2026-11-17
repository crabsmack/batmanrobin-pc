#include "game.h"

/* VM4_ResumeA5 / Op4_SpawnRunChild ($0055C2-$0055D7): continuing the a4 script of a re-made
 * object, and spawning a child that runs at once. */

/* the handler word at a script address (a register holding it), and the address past it */
static uint32_t fetch_at(uint32_t *a4)
{
    PTR(const u16) script;
    script.v = *a4;
    uint32_t handler = dk::code((int16_t)(uint16_t)*script++);
    *a4 = script.raw();
    return handler;
}

/* VM4_ResumeA5 - a4 VM: continues the a4 script of the object in a5 from its saved pc; the return
 * point Op4_ReplaceSelf pushes before re-initialising the object.
 * In:  obj = a5 (the object itself here)
 * Out: a4 = obj->resume past its first handler word; that op, jumped to
 * Evidence: movea.l $8(a5),a4 / NEXT; pea (VM4_ResumeA5).l in Op4_ReplaceSelf, entered by the rts of
 *           $6156/$6182; 147 corpus runs
 */
uint32_t VM4_ResumeA5(const Vm4Obj *obj)
{
    uint32_t script = obj->resume.raw();
    uint32_t handler = fetch_at(&script);
    dk::jump(handler);
    return script;
}

/* Op4_SpawnRunChild - a4 op: spawns a child object (VM4_SpawnChild) and switches to it: the child
 * becomes a6 and its script runs right away this frame (the parent stops here and resumes next
 * frame from its own pc).
 * In:  a4 = script: word failure offset (0 = end the run), word child-script offset, word
 *      parameter-table offset (0 = start at d2-d5); d2-d5 = radius/heading/x/y; a6 = parent
 * Out: a5 = a6 = the child; a4 = its script past the first handler word; that op, jumped to (a full
 *      pool: VM4_SpawnChild never returns here)
 * Evidence: bsr VM4_SpawnChild / movea.l a5,a6 / movea.l $8(a6),a4 / NEXT; script uses $1AB56,
 *           $298D0, $298F0 (the child script = the words right after the op); 34 corpus runs
 */
void Op4_SpawnRunChild(uint32_t *d0, uint32_t *d1, uint32_t d2, uint32_t d3, uint32_t d4, uint32_t d5, uint32_t d6,
                       uint32_t d7, uint32_t *a0, uint32_t *a1, uint32_t a2, uint32_t *a3, uint32_t *a4,
                       uint32_t *a5, uint32_t *a6)
{
    (void)d6, (void)d7, (void)a2;
    dk::Flags spawn_ccr{};   /* VM4_SpawnChild's condition codes (not read here) */
    VM4_SpawnChild(d0, d1, d2, d3, d4, d5, a0, a1, a3, a4, a5, *a6, &spawn_ccr);
    *a6 = *a5;
    PTR(const Vm4Obj) child;
    child.v = *a6;
    *a4 = child->resume.raw();
    *a0 = fetch_at(a4);
    dk::jump(*a0);
}
