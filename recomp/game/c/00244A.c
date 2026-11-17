#include "game.h"

/* Op2_SpawnChild ($00244A-$002475; the twin at $2476 is raw bytes, never run). */

/* the child handle field at a6 + the operand's offset */
static u16 &child_field(Vm2Obj *self, uint16_t field)
{
    return *(u16 *)((u8 *)self + (int16_t)field);
}

/* Op2_SpawnChild - a2 op: spawns a child object once: when the a6 field the operand names is 0,
 * calls the spawner with d4/d5 = the object's x/y and stores the new object's handle there. A
 * field already set, or a failed spawn (the pool full), just goes on (VM2_Next).
 * In:  script = word a6 field offset, long spawner (a6 = the new object, Z set = spawned)
 * Out: d4/d5 low words = x/y, then as the spawner leaves them with d1, d7, a1; d0's low word = the
 *      field; a0 = the spawner, or the new object; the field = the child's handle; NEXT
 * Evidence: move.w $3E(a6),d4 / move.w $42(a6),d5 / move.w (a2)+,d0 / movea.l (a2)+,a0 /
 *           tst.w (a6,d0.w) / bne.w VM2_Next / movem.l d0/a2-a6,-(a7) / jsr (a0) / movea.l a6,a0 /
 *           movem.l (a7)+,d0/a2-a6 / bne.w VM2_Next / move.w a0,(a6,d0.w) / NEXT; 26 runs;
 *           $39686 `244A 002A 000399C6`
 */
void Op2_SpawnChild(uint32_t *d0, uint16_t *d1, uint32_t *d4, uint32_t *d5, uint32_t *d7, uint32_t *a0, uint32_t *a1,
                    const u16 **script, uint32_t a3, uint32_t a4, uint32_t a5, Vm2Obj *self)
{
    *d4 = (*d4 & 0xFFFF0000u) | (uint16_t)self->x;
    *d5 = (*d5 & 0xFFFF0000u) | (uint16_t)self->y;
    *d0 = (*d0 & 0xFFFF0000u) | *(*script)++;
    const u32 *spawner = (const u32 *)*script;
    *a0 = *spawner;
    *script = (const u16 *)(spawner + 1);
    if (child_field(self, (uint16_t)*d0) != 0) {
        dk::jump(at_VM2_Next);
        return;
    }
    /* movem.l d0/a2-a6,-(a7): d0 lowest; the spawner runs with them on the stack */
    dk::push_long(dk::addr(self));
    dk::push_long(a5);
    dk::push_long(a4);
    dk::push_long(a3);
    dk::push_long(dk::addr(*script));
    dk::push_long(*d0);
    uint32_t d0_in = *d0, a6 = dk::addr(self);
    const u16 *spawner_script = *script;
    bool spawned = ChildSpawnerFn(dk::code(*a0), &d0_in, d1, d4, d5, d7, a0, a1, &spawner_script, a3, a4, a5, &a6);
    *a0 = a6;   /* movea.l a6,a0: the new object */
    /* movem.l (a7)+,d0/a2-a6: d0 and the script come back from the stack (a3-a6 are the op's
       inputs only) */
    PTR(const u16) saved_script;
    *d0 = dk::pop_long();
    saved_script.v = dk::pop_long();
    *script = saved_script;
    dk::pop_long();
    dk::pop_long();
    dk::pop_long();
    dk::pop_long();
    if (!spawned) {
        dk::jump(at_VM2_Next);
        return;
    }
    child_field(self, (uint16_t)*d0) = (uint16_t)*a0;
    *a0 = dk::code((int16_t)(uint16_t)*(*script)++);
    dk::jump(*a0);
}
