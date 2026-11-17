#include "game.h"

/* The child-object ops ($002486-$00254F): spawn-or-wait, a native call on the child, kill, the
 * has-child branches, running the child's handler inline, and the field sync. A word operand names
 * the a6 field holding the child's handle (0 = none). */

/* a script pointer from a 68000 address held in a register */
static const u16 *script_at(uint32_t a)
{
    PTR(const u16) p;
    p.v = a;
    return p;
}

/* NEXT: the handler the script's next word names, jumped to */
static void next(uint32_t *handler, const u16 **script)
{
    *handler = dk::code((int16_t)(uint16_t)*(*script)++);
    dk::jump(*handler);
}

/* the low word of a data register, the upper word kept (a .w operation) */
static void set_low_word(uint32_t *r, uint16_t v) { *r = (*r & 0xFFFF0000u) | v; }

/* the child handle field at a6 + the operand's offset */
static u16 &child_field(Vm2Obj *self, uint16_t field)
{
    return *(u16 *)((u8 *)self + (int16_t)field);
}

/* Op2_SpawnChildOrWait - a2 op: as Op2_SpawnChild, but when the spawn fails (the pool full) the
 * script rewinds to this op and yields, to retry next frame.
 * In:  script = word a6 field offset, long spawner (a6 = the new object, Z set = spawned)
 * Out: d4/d5 = x/y, then as the spawner leaves them with d7; d0's low word = the field, d1 = 8
 *      (restored); a0 = the spawner, or the new object; the field = the child's handle; NEXT (Z as
 *      the spawner left it); failed: script back to the op word, Op2_Yield
 * Evidence: move.w $3E(a6),d4 / move.w $42(a6),d5 / moveq #8,d1 / ... / movem.l d0-d1/a2-a6,-(a7)
 *           / jsr (a0) / movea.l a6,a0 / movem.l (a7)+,d0-d1/a2-a6 / bne / move.w a0,(a6,d0.w) /
 *           NEXT; failed: suba.w d1,a2 / bra.w Op2_Yield; dispatched in 9 runs (6 TAS frames)
 */
void Op2_SpawnChildOrWait(uint32_t *d0, uint32_t *d1, uint16_t *d4, uint16_t *d5, uint32_t *d7, uint32_t *a0,
                          const u16 **script, uint32_t a3, uint32_t a4, uint32_t a5, Vm2Obj *self)
{
    *d4 = (uint16_t)self->x;
    *d5 = (uint16_t)self->y;
    *d1 = 8;
    set_low_word(d0, *(*script)++);
    const u32 *spawner = (const u32 *)*script;
    *a0 = *spawner;
    *script = (const u16 *)(spawner + 1);
    if (child_field(self, (uint16_t)*d0) != 0) {
        dk::jump(at_VM2_Next);
        return;
    }
    /* movem.l d0-d1/a2-a6,-(a7): d0 lowest; the spawner runs with them on the stack */
    dk::push_long(dk::addr(self));
    dk::push_long(a5);
    dk::push_long(a4);
    dk::push_long(a3);
    dk::push_long(dk::addr(*script));
    dk::push_long(*d1);
    dk::push_long(*d0);
    uint32_t d0_in = *d0, d1_in = *d1, a6 = dk::addr(self);
    const u16 *spawner_script = *script;
    bool spawned = ChildSpawnerWaitFn(dk::code(*a0), &d0_in, &d1_in, d4, d5, d7, a0, &spawner_script, a3, a4, a5, &a6);
    *a0 = a6;   /* movea.l a6,a0: the new object */
    *d0 = dk::pop_long();
    *d1 = dk::pop_long();
    *script = script_at(dk::pop_long());
    dk::pop_long();
    dk::pop_long();
    dk::pop_long();
    dk::pop_long();
    if (!spawned) {
        *script = (const u16 *)((const u8 *)*script - (int16_t)(uint16_t)*d1);   /* back to the op */
        dk::jump(at_Op2_Yield);
        return;
    }
    child_field(self, (uint16_t)*d0) = (uint16_t)*a0;
    next(a0, script);
}

/* Op2_CallNativeOnChild - a2 op: calls a native routine with a0 = the child object; no child: the
 * call is skipped.
 * In:  script = word a6 field offset, long routine
 * Out: d0's low word = the field, d1's = the child; a0 = the child, a1 = the routine; then every
 *      register as the routine leaves it; NEXT (no child: VM2_Next)
 * Evidence: move.w (a2)+,d0 / movea.l (a2)+,a1 / move.w (a6,d0.w),d1 / beq.w VM2_Next /
 *           movea.w d1,a0 / jsr (a1) / NEXT; dispatched in 22 runs (target $371C6)
 */
void Op2_CallNativeOnChild(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5,
                           uint32_t *d6, uint32_t *d7, uint32_t *a0, uint32_t *a1, const u16 **script, uint32_t *a3,
                           uint32_t *a4, uint32_t *a5, uint32_t *a6)
{
    set_low_word(d0, *(*script)++);
    const u32 *routine = (const u32 *)*script;
    *a1 = *routine;
    *script = (const u16 *)(routine + 1);
    PTR(Vm2Obj) self;
    self.v = *a6;
    set_low_word(d1, child_field(self, (uint16_t)*d0));
    if ((uint16_t)*d1 == 0) {
        dk::jump(at_VM2_Next);
        return;
    }
    *a0 = (uint32_t)(int32_t)(int16_t)(uint16_t)*d1;
    NativeFn(dk::code(*a1), d0, d1, d2, d3, d4, d5, d6, d7, a0, a1, script, a3, a4, a5, a6);
    next(a0, script);
}

/* Op2_KillChild - a2 op: destroys the child object (Obj_DestroyOther) and clears the field.
 * In:  script = word a6 field offset
 * Out: d0's low word = the field, d1's = the child, then d0-d7 as Obj_DestroyOther leaves them; the
 *      field = 0; NEXT (no child: VM2_Next)
 * Evidence: move.w (a2)+,d0 / move.w (a6,d0.w),d1 / beq.w VM2_Next / clr.w (a6,d0.w) /
 *           movea.w d1,a0 / bsr.w Obj_DestroyOther / NEXT; dispatched in 1 run (TAS)
 */
void Op2_KillChild(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5,
                   uint32_t *d6, uint32_t *d7, uint32_t *a0, const u16 **script, Vm2Obj *self)
{
    set_low_word(d0, *(*script)++);
    set_low_word(d1, child_field(self, (uint16_t)*d0));
    if ((uint16_t)*d1 == 0) {
        dk::jump(at_VM2_Next);
        return;
    }
    child_field(self, (uint16_t)*d0) = 0;
    Obj_DestroyOther(d0, d1, d2, d3, d4, d5, d6, d7, (uint32_t)(int32_t)(int16_t)(uint16_t)*d1, dk::addr(*script));
    next(a0, script);
}

/* Op2_BranchHasChild - a2 op: branches while the a6 field operand holds a child object.
 * In:  script = word a6 field offset, word branch offset
 * Out: field (d0's low word); a child: Op2_Goto, else NEXT after the offset
 * Evidence: move.w (a2)+,d0 / tst.w (a6,d0.w) / bne.w Op2_Goto / addq.l #2,a2 / NEXT; 27 runs
 */
void Op2_BranchHasChild(uint16_t *field, uint32_t *a0, const u16 **script, const Vm2Obj *self)
{
    *field = *(*script)++;
    if (child_field((Vm2Obj *)self, *field) != 0) {
        dk::jump(at_Op2_Goto);
        return;
    }
    (*script)++;
    next(a0, script);
}

/* Op2_BranchNoChild - a2 op: branches when the a6 field operand holds no child object.
 * In:  script = word a6 field offset, word branch offset
 * Out: field (d0's low word); no child: Op2_Goto, else NEXT after the offset
 * Evidence: move.w (a2)+,d0 / tst.w (a6,d0.w) / beq.w Op2_Goto / addq.l #2,a2 / NEXT; 37 runs
 */
void Op2_BranchNoChild(uint16_t *field, uint32_t *a0, const u16 **script, const Vm2Obj *self)
{
    *field = *(*script)++;
    if (child_field((Vm2Obj *)self, *field) == 0) {
        dk::jump(at_Op2_Goto);
        return;
    }
    (*script)++;
    next(a0, script);
}

/* the run part of Op2_RunChild ($251C): the child's handler runs now (a6 = the child), with
   g_VM2SavedSP and d0/a1-a6 saved on the stack around it (the handler's own VM2_RunObject saves
   the stack pointer there); Z clear back (the child ended): the field is cleared */
static void run_child(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5,
                      uint32_t *d6, uint32_t *d7, uint32_t *a0, uint32_t a1, const u16 **script, uint32_t a3, uint32_t a4,
                      uint32_t a5, Vm2Obj *self)
{
    dk::push_long(g_VM2SavedSP);
    /* movem.l d0/a1-a6,-(a7): d0 lowest */
    dk::push_long(dk::addr(self));
    dk::push_long(a5);
    dk::push_long(a4);
    dk::push_long(a3);
    dk::push_long(dk::addr(*script));
    dk::push_long(a1);
    dk::push_long(*d0);
    uint32_t child = (uint32_t)(int32_t)(int16_t)(uint16_t)*d1;
    *a0 = dk::handle<Vm2Obj>((int16_t)(uint16_t)*d1)->handler;
    uint32_t r0 = *d0;
    uint32_t r_a1 = a1, r_a2 = dk::addr(*script), r_a3 = a3, r_a4 = a4, r_a5 = a5;
    bool still_running = ChildHandlerFn(dk::code(*a0), &r0, d1, d2, d3, d4, d5, d6, d7, a0, &r_a1, &r_a2,
                                        &r_a3, &r_a4, &r_a5, child);
    /* movem.l (a7)+,d0/a1-a6, then g_VM2SavedSP: d0 and the script come back */
    *d0 = dk::pop_long();
    dk::pop_long();
    *script = script_at(dk::pop_long());
    dk::pop_long();
    dk::pop_long();
    dk::pop_long();
    dk::pop_long();
    g_VM2SavedSP = dk::pop_long();
    if (!still_running)
        child_field(self, (uint16_t)*d0) = 0;
    next(a0, script);
}

/* Op2_SyncAndRunChild - a2 op: Op2_SyncChild (copies the chosen fields to the child), then with a
 * child runs its handler at once as Op2_RunChild does.
 * In:  script = word a6 field offset, word flags (+ word pairs, see Obj_SyncChild)
 * Out: d0-d3 low words as Obj_SyncChild leaves them; then as Op2_RunChild; NEXT
 * Evidence: bsr.w Obj_SyncChild / tst.w d1 / bne.s $251C / NEXT; dispatched in 24 runs
 */
void Op2_SyncAndRunChild(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5,
                         uint32_t *d6, uint32_t *d7, uint32_t *a0, uint32_t a1, const u16 **script, uint32_t a3,
                         uint32_t a4, uint32_t a5, Vm2Obj *self)
{
    uint16_t field = (uint16_t)*d0, child = (uint16_t)*d1, flags = (uint16_t)*d2, pair = (uint16_t)*d3;
    Obj_SyncChild(&field, &child, &flags, &pair, script, dk::handle<Vm2Sprite>((int16_t)(uint16_t)a5), self);
    set_low_word(d0, field);
    set_low_word(d1, child);
    set_low_word(d2, flags);
    set_low_word(d3, pair);
    if (child != 0) {
        run_child(d0, d1, d2, d3, d4, d5, d6, d7, a0, a1, script, a3, a4, a5, self);
        return;
    }
    next(a0, script);
}

/* Op2_RunChild - a2 op: runs the child object's handler now (a6 = the child), keeping
 * g_VM2SavedSP across it; clears the field when the handler comes back with Z clear (the child
 * ended).
 * In:  script = word a6 field offset
 * Out: d0's low word = the field; d1 = the child's handle, then d1-d7 as the handler leaves them
 *      (not saved around it); handler = the next op's; NEXT (no child: VM2_Next)
 * Evidence: move.w (a2)+,d0 / move.w (a6,d0.w),d1 / beq.w VM2_Next / move.l (g_VM2SavedSP).w,-(a7)
 *           / movem.l d0/a1-a6,-(a7) / movea.w d1,a6 / movea.l (a6),a0 / jsr (a0) / bne.s $2538
 *           (clr.w (a6,d0.w)); dispatched in 22 runs; the handler is VM2_RunObject (413k calls)
 */
void Op2_RunChild(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5,
                  uint32_t *d6, uint32_t *d7, uint32_t *a0, uint32_t a1, const u16 **script, uint32_t a3, uint32_t a4, uint32_t a5,
                  Vm2Obj *self)
{
    set_low_word(d0, *(*script)++);
    set_low_word(d1, child_field(self, (uint16_t)*d0));
    if ((uint16_t)*d1 == 0) {
        dk::jump(at_VM2_Next);
        return;
    }
    run_child(d0, d1, d2, d3, d4, d5, d6, d7, a0, a1, script, a3, a4, a5, self);
}

/* Op2_SyncChild - a2 op: copies the parent's position / display fields to its child as a flags
 * word selects (Obj_SyncChild).
 * In:  script = word a6 field offset, word flags, word pairs
 * Out: as Obj_SyncChild; NEXT
 * Evidence: bsr.w Obj_SyncChild / NEXT; dispatched in 13 runs
 */
void Op2_SyncChild(uint16_t *field, uint16_t *child, uint16_t *flags, uint16_t *pair, const u16 **script,
                   const Vm2Sprite *spr, const Vm2Obj *self)
{
    Obj_SyncChild(field, child, flags, pair, script, spr, self);
    dk::jump(dk::code((int16_t)(uint16_t)*(*script)++));
}
