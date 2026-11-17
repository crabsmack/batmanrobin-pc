#include "game.h"

/* The a2 VM's core ops ($0007FC-$000985): native code, entering a script from native code,
 * resume points, the yields, the off-screen branches and kills, and destroying an object. Each op
 * ends with the VM's NEXT (the next word names the next op's handler), a branch (into Op2_Goto /
 * Op2_SkipGoto), or a yield (the stack pointer VM2_RunObject saved, then its caller's return). */

/* NEXT: the handler the script's next word names, jumped to */
static void next(uint32_t *handler, const u16 **script)
{
    *handler = dk::code((int16_t)(uint16_t)*(*script)++);
    dk::jump(*handler);
}

/* branch taken: the script continues at the offset word plus its value (adda.w (a2),a2) */
static void branch(uint32_t *handler, const u16 **script)
{
    *script = (const u16 *)((const u8 *)*script + (int16_t)(uint16_t)**script);
    next(handler, script);
}

/* the object's screen position: world position minus the camera's (both integer parts) */
static int16_t screen_x(const Vm2Obj *self) { return (int16_t)(self->x - (int16_t)(g_CameraX >> 16)); }
static int16_t screen_y(const Vm2Obj *self) { return (int16_t)(self->y - (int16_t)(g_CameraY >> 16)); }

/* the low word of a data register, the upper word kept (a .w operation) */
static void set_low_word(uint32_t *r, uint16_t v) { *r = (*r & 0xFFFF0000u) | v; }

/* Op2_Native - a2 op: leaves the script and runs the 68000 code that follows the op word; that
 * code goes back to script with `jsr VM2_Enter`, which resumes right after the jsr.
 * In:  script = the native code (right after this op's word)
 * Out: jumped to it
 * Evidence: jmp (a2); native fragments $13F36, $1806C, $197FC end in jsr $7FE.w; 12k TAS frames
 */
void Op2_Native(const u16 *script)
{
    dk::jump(dk::addr(script));
}

/* VM2_Enter - enters a2 script from native code: the return address of the `jsr VM2_Enter` is the
 * script (the bytes after the jsr), popped as the script pointer, then NEXT.
 * In:  the return address = the script; the running object's registers (the fragment runs under
 *      VM2_RunObject)
 * Out: NEXT; never returns to the jsr
 * Evidence: movea.l (a7)+,a2 / NEXT; 73 jsr $7FE.w sites; dkit.toml [disasm] noreturn
 */
void VM2_Enter(uint32_t *handler, const u16 **script)
{
    *script = dk::return_data<u16>();
    dk::drop_return();
    next(handler, script);
}

/* Op2_SetOwnerId - a2 op: sets the object's owner / side id from a word operand.
 * In:  script = the id (1/2 the players, $A/$B enemies elsewhere)
 * Out: self->owner_id; NEXT
 * Evidence: move.w (a2)+,$58(a6); $385A/$3798 copy $58(a6) into each collision box +$10
 */
void Op2_SetOwnerId(const u16 **script, Vm2Obj *self)
{
    uint32_t handler;
    self->owner_id = (uint16_t)*(*script)++;
    next(&handler, script);
}

/* Op2_SetResume - a2 op: the point VM2_RunObject resumes at next frame becomes the operand's
 * address plus its value; the script continues here.
 * In:  script = a word offset (from the word itself)
 * Out: vm->resume; NEXT
 * Evidence: movea.l a2,a0 / adda.w (a2)+,a0 / move.l a0,$2A(a4)
 */
void Op2_SetResume(const u16 **script, Vm2Block *vm)
{
    uint32_t handler;
    vm->resume = (const u16 *)((const u8 *)*script + (int16_t)(uint16_t)**script);
    (*script)++;
    next(&handler, script);
}

/* Op2_SetResumeHere - a2 op: the next op becomes the resume point.
 * In:  script = the next op (no operand)
 * Out: vm->resume = it; NEXT
 * Evidence: move.l a2,$2A(a4) then NEXT
 */
void Op2_SetResumeHere(const u16 **script, Vm2Block *vm)
{
    uint32_t handler;
    vm->resume = *script;
    next(&handler, script);
}

/* Op2_Yield - a2 op: ends this frame's run: the next op is where the script resumes; the stack
 * pointer VM2_RunObject saved, then its caller's return with 0.
 * In:  script = the next op; vm
 * Out: vm->resume; returns 0 from VM2_RunObject's call
 * Evidence: move.l a2,$2A(a4) / movea.l (g_VM2SavedSP).w,a7 / moveq #0,d0 / rts; native callers
 *           branch here to wait a frame ($120E, $2486, $3B76, $437A, $43E2)
 */
uint32_t Op2_Yield(const u16 *script, Vm2Block *vm)
{
    vm->resume = script;
    return Op2_YieldAgain();
}

/* Op2_YieldAgain - a2 op: ends this frame's run without moving the resume point, so the next frame
 * runs the script again from the point saved last: the saved stack pointer, then VM2_RunObject's
 * caller's return with 0.
 * In:  none (the VM block's resume point stays)
 * Out: returns 0 from VM2_RunObject's call
 * Evidence: Op2_Yield's code after its move.l a2,$2A(a4); 786,961 computed jumps in the corpus
 *           (call-site log: NEXT $7E2, $2CBA, $2D26, $2D2E, $3A52 ...)
 */
uint32_t Op2_YieldAgain(void)
{
    dk::set_stack_pointer(g_VM2SavedSP);
    return 0;
}

/* Native_NoOp - a native that does nothing: the rts closing Op2_Yield, stored as a handler
 * wherever one must be callable but has nothing to do.
 * In:  none
 * Out: none
 * Evidence: jsr/jmp (a1) to it from Hitbox_Strike $4E62, Hitbox_StrikeAll $4E74 and $37574 (the
 *           call-site log: 4448, 9601, 2480 calls); never-run ops set a coroutine slot to it
 */
void Native_NoOp(void)
{
}

/* Op2_YieldGoto - a2 op: ends this frame's run; the next frame resumes at the operand's address
 * plus its value.
 * In:  script = a word offset; vm
 * Out: script = the resume point; result = 0 (Op2_Yield)
 * Evidence: adda.w (a2),a2, then the Op2_Yield code
 */
void Op2_YieldGoto(uint32_t *result, const u16 **script, Vm2Block *vm)
{
    *script = (const u16 *)((const u8 *)*script + (int16_t)(uint16_t)**script);
    *result = Op2_Yield(*script, vm);
}

/* Op2_BranchOffScrY - a2 op: branches when the object's screen y is outside [lo, hi]; also the y
 * half of Op2_BranchOffScrBox.
 * In:  script = words lo, hi, offset; self
 * Out: screen = the screen y; the branch by the offset, else NEXT after it
 * Evidence: blt.s $89A (skip hi, branch) / bgt.w Op2_Goto / addq.l #2,a2 + NEXT
 */
void Op2_BranchOffScrY(uint16_t *screen, uint32_t *handler, const u16 **script, const Vm2Obj *self)
{
    int16_t y = screen_y(self);
    *screen = (uint16_t)y;
    if (y < (int16_t)*(*script)++) {
        (*script)++;
        branch(handler, script);
        return;
    }
    if (y > (int16_t)*(*script)++) {
        dk::jump(at_Op2_Goto);
        return;
    }
    (*script)++;
    next(handler, script);
}

/* Op2_BranchOffScrBox - a2 op: branches when the object's screen position is outside the box
 * [xlo, xhi] x [ylo, yhi].
 * In:  script = words xlo, xhi, ylo, yhi, offset; self
 * Out: screen = the screen x (or y, from Op2_BranchOffScrY); the branch, else NEXT after the five words
 * Evidence: the x part here (blt.s $846 / ble.s Op2_BranchOffScrY), the y part shared
 */
void Op2_BranchOffScrBox(uint16_t *screen, uint32_t *handler, const u16 **script, const Vm2Obj *self)
{
    int16_t x = screen_x(self);
    *screen = (uint16_t)x;
    if (x < (int16_t)*(*script)++) {
        *script += 3;
        branch(handler, script);
        return;
    }
    if (x <= (int16_t)*(*script)++) {
        Op2_BranchOffScrY(screen, handler, script, self);
        return;
    }
    *script += 2;
    branch(handler, script);
}

/* Op2_BranchOffScrX - a2 op: branches when the object's screen x is outside [lo, hi].
 * In:  script = words lo, hi, offset; self
 * Out: screen = the screen x; the branch (Op2_SkipGoto / Op2_Goto), else NEXT after the offset
 * Evidence: blt.w $2D20 / bgt.w Op2_Goto / addq.l #2,a2 + NEXT
 */
void Op2_BranchOffScrX(uint16_t *screen, uint32_t *handler, const u16 **script, const Vm2Obj *self)
{
    int16_t x = screen_x(self);
    *screen = (uint16_t)x;
    if (x < (int16_t)*(*script)++) {
        dk::jump(at_Op2_SkipGoto);
        return;
    }
    if (x > (int16_t)*(*script)++) {
        dk::jump(at_Op2_Goto);
        return;
    }
    (*script)++;
    next(handler, script);
}

/* Op2_SwitchScrX - a2 op: three-way branch on the object's screen x: left of lo, right of hi, or
 * inside.
 * In:  script = words lo, hi, offLeft, offRight; self
 * Out: screen = the screen x; left branches by offLeft, right by offRight (both via Op2_SkipGoto),
 *      inside NEXT after both offsets
 * Evidence: blt.w $2D20 / bgt.w $2D20 / addq.l #4,a2 + NEXT
 */
void Op2_SwitchScrX(uint16_t *screen, uint32_t *handler, const u16 **script, const Vm2Obj *self)
{
    int16_t x = screen_x(self);
    *screen = (uint16_t)x;
    if (x < (int16_t)*(*script)++ || x > (int16_t)*(*script)++) {
        dk::jump(at_Op2_SkipGoto);
        return;
    }
    *script += 2;
    next(handler, script);
}

/* Op2_Kill - a2 op: destroys the running object (Obj_Destroy) and leaves VM2_RunObject with -1.
 * In:  the running object (a6, its blocks a4/a5) and whatever Obj_Destroy passes through
 * Out: the stack pointer VM2_RunObject saved, its caller's return with d0 = -1
 * Evidence: bsr.s Obj_Destroy / movea.l (g_VM2SavedSP).w,a7 / moveq #-1,d0 / rts; the target of
 *           the off-screen kill ops $8A2-$93A
 */
void Op2_Kill(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6,
              uint32_t *d7, uint32_t *a0, uint32_t *a1, uint32_t *a2, uint32_t a4, uint32_t a5, uint32_t a6)
{
    Obj_Destroy(d0, d1, d2, d3, d4, d5, d6, d7, a0, a1, a2, a4, a5, a6);
    dk::set_stack_pointer(g_VM2SavedSP);
    *d0 = 0xFFFFFFFFu;
}

/* The off-screen kill ops: a coordinate (plus a margin) against a span, or the script's bounds;
   out of it, branch to Op2_Kill (registers as they are: it destroys the object and yields), else
   NEXT. d0's low word is the last coordinate tested, as the asm leaves it. */
#define KILL_PARAMS uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, \
                    uint32_t *d6, uint32_t *d7, uint32_t *handler, uint32_t *a1, const u16 **script, \
                    uint32_t a4, uint32_t a5, const Vm2Obj *self
#define KILL_ARGS d0, d1, d2, d3, d4, d5, d6, d7, handler, a1, script, a4, a5, self

static bool outside(uint32_t *d0, int16_t coord, uint16_t margin, uint16_t span)
{
    uint16_t v = (uint16_t)(coord + margin);
    set_low_word(d0, v);
    return v >= span;   /* unsigned: below the left/top margin wraps above the span too */
}

static void kill_unless_inside(uint16_t margin, uint16_t width, uint16_t height, uint32_t *d0, uint32_t *handler,
                               const u16 **script, const Vm2Obj *self)
{
    /* native: the x window widened to the view on each side (the gameplay mode, R4, R20) */
    if (outside(d0, screen_x(self), (uint16_t)(margin + WS_WIN_L()), (uint16_t)(width + (WS_WIN_L() + WS_WIN_R()))) ||
        outside(d0, screen_y(self), margin, height)) {
        dk::jump(at_Op2_Kill);
        return;
    }
    next(handler, script);
}

/* Op2_KillOffscreen50 - a2 op: destroys the object when it is more than 50 px outside the 320x224
 * screen, else NEXT.
 * In:  self; the registers Op2_Kill passes through
 * Out: Op2_Kill, or NEXT; d0's low word the last coordinate + 50
 * Evidence: addi.w #$32 / cmpi.w #$1A4 (320 + 2*50) / bcc Op2_Kill, then y against $144 (224 + 2*50)
 */
void Op2_KillOffscreen50(KILL_PARAMS)
{
    kill_unless_inside(50, 320 + 2 * 50, 224 + 2 * 50, d0, handler, script, self);
}

/* Op2_KillOffscreen100 - a2 op: destroys the object when it is more than 100 px outside the
 * screen, else NEXT.
 * In:  self; the registers Op2_Kill passes through
 * Out: Op2_Kill, or NEXT; d0's low word the last coordinate + 100
 * Evidence: addi.w #$64 / cmpi.w #$208 (320 + 200), then y against $1A8 (224 + 200)
 */
void Op2_KillOffscreen100(KILL_PARAMS)
{
    kill_unless_inside(100, 320 + 2 * 100, 224 + 2 * 100, d0, handler, script, self);
}

/* Op2_KillOffscreen200 - a2 op: destroys the object when it is more than 200 px outside the
 * screen, else NEXT.
 * In:  self; the registers Op2_Kill passes through
 * Out: Op2_Kill, or NEXT; d0's low word the last coordinate + 200
 * Evidence: addi.w #$C8 / cmpi.w #$2D0 (320 + 400), then y against $270 (224 + 400)
 */
void Op2_KillOffscreen200(KILL_PARAMS)
{
    kill_unless_inside(200, 320 + 2 * 200, 224 + 2 * 200, d0, handler, script, self);
}

/* Op2_KillOffScrX - a2 op: destroys the object when its screen x is outside [lo, hi], else NEXT.
 * In:  script = words lo, hi; self
 * Out: Op2_Kill (script past the words tested), or NEXT; d0's low word the screen x
 * Evidence: cmp.w (a2)+,d0 / blt.s Op2_Kill / cmp.w (a2)+,d0 / bgt.s Op2_Kill / NEXT
 */
void Op2_KillOffScrX(KILL_PARAMS)
{
    int16_t x = screen_x(self);
    set_low_word(d0, (uint16_t)x);
    if (x < (int16_t)*(*script)++ || x > (int16_t)*(*script)++) {
        dk::jump(at_Op2_Kill);
        return;
    }
    next(handler, script);
}

/* Op2_KillOffScrBox - a2 op: destroys the object unless xlo <= screen x <= xhi and
 * ylo <= screen y < yhi, else NEXT.
 * In:  script = words xlo, xhi, ylo, yhi; self
 * Out: NEXT (VM2_Next) when inside, else Op2_Kill (script past the words tested); d0's low word
 *      the last coordinate tested
 * Evidence: four cmp.w (a2)+,d0; the last is blt.w VM2_Next (strictly below yhi), then falls into Op2_Kill
 */
void Op2_KillOffScrBox(KILL_PARAMS)
{
    int16_t x = screen_x(self);
    set_low_word(d0, (uint16_t)x);
    if (x < (int16_t)*(*script)++ || x > (int16_t)*(*script)++) {
        dk::jump(at_Op2_Kill);
        return;
    }
    Op2_KillOffScrY(KILL_ARGS);
}

/* Op2_KillOffScrY - a2 op: destroys the object unless ylo <= its screen y < yhi; the y half of
 * Op2_KillOffScrBox.
 * In:  script = words ylo, yhi; self
 * Out: NEXT (VM2_Next) when inside, else Op2_Kill (script past the words tested); d0's low word
 *      the screen y
 * Evidence: move.w $42(a6),d0 / sub.w g_CameraY / cmp.w (a2)+ blt Op2_Kill / cmp.w (a2)+ blt.w
 *           VM2_Next / falls into Op2_Kill; 49,218 computed jumps in the corpus
 */
void Op2_KillOffScrY(KILL_PARAMS)
{
    int16_t y = screen_y(self);
    set_low_word(d0, (uint16_t)y);
    if (y >= (int16_t)*(*script)++ && y < (int16_t)*(*script)++) {
        dk::jump(at_VM2_Next);
        return;
    }
    dk::jump(at_Op2_Kill);
}

/* Obj_DestroyLoaded - Obj_DestroyOther's tail: the caller has pushed a1-a6 and loaded a6 / a3-a5
 * with the object to destroy; Obj_Destroy, then a1-a6 popped back.
 * In:  the object (a6, a4, a5); a1-a6 of the caller on the stack
 * Out: a1-a6 restored from the stack; d0-d7 / a0 as Obj_Destroy leaves them, its condition codes
 * Evidence: bsr.s Obj_Destroy / movem.l (a7)+,a1-a6 / rts; ToyChain_KillChild $371C6 pushes a1-a6,
 *           loads the registers itself and jumps here
 */
void Obj_DestroyLoaded(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5,
                       uint32_t *d6, uint32_t *d7, uint32_t *a0, uint32_t *a1, uint32_t *a2, uint32_t *a3,
                       uint32_t *a4, uint32_t *a5, uint32_t *a6)
{
    Obj_Destroy(d0, d1, d2, d3, d4, d5, d6, d7, a0, a1, a2, *a4, *a5, *a6);
    uint32_t *saved[6] = { a1, a2, a3, a4, a5, a6 };
    for (uint32_t *r : saved)
        *r = dk::pop_long();
}

/* Obj_DestroyOther - native: destroys another object (not the running one), its registers kept.
 * In:  victim = the object (a word handle in the low word); script (a2) for Obj_Destroy
 * Out: d0-d7 as Obj_Destroy leaves them; a1-a6 kept
 * Evidence: movem.l a1-a6,-(a7) / movea.w a0,a6 / its handles into a3-a5 / Obj_DestroyLoaded's
 *           tail; callers op $24CE (a child handle at a6+word)
 */
void Obj_DestroyOther(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5,
                      uint32_t *d6, uint32_t *d7, uint32_t victim, uint32_t a2)
{
    const Vm2Obj *obj = dk::handle<const Vm2Obj>((int16_t)(uint16_t)victim);
    uint32_t a0 = victim, a1 = 0, script = a2;
    Obj_Destroy(d0, d1, d2, d3, d4, d5, d6, d7, &a0, &a1, &script, dk::addr(dk::handle(obj->vm)),
                dk::addr(dk::handle(obj->sprite)), dk::addr(obj));
}
