/* dk_flow.h - how hand-written C leaves a function other than by returning (phase 4).
 * Written by the kit's recomp template; implemented in game/runtime/handc_rt.cpp.
 *
 * Hand-written 68000 doesn't only call and return: script VMs chain their ops with `jmp (a0)`,
 * call an interpreter with the script inline after the `jsr`, yield by restoring a saved stack
 * pointer, read arguments from behind their return address, return past their caller, and
 * interrupt handlers end with RTE. The C does each the way the 68000 did, on the 68000's own
 * stack (every call from C goes through it: the bridge's stubs), with these calls instead of
 * registers. Each `leave` ends the C function: return right after it.
 *
 *     void Op2_SkipWord(u16 **script)          the op skips its operand, then NEXT:
 *     {
 *         (*script)++;
 *         dk::jump(dk::code(*(*script)++));    jmp (a0): the handler the next word names
 *     }
 *
 * Calls through a pointer (`jsr (a0)` to a handler stored in data) use a function type with its
 * convention in the tag (dkit genesis recomp bridge, "type prototypes"), never these.
 */
#ifndef DK_FLOW_H
#define DK_FLOW_H

#include <stdint.h>

#include "dk_mem.h"

namespace dk {

/* A 68000 code address from a word or long the game stored (a handler word is sign-extended:
   `movea.w (a2)+,a0`). */
inline uint32_t code(int16_t w) { return (uint32_t)(int32_t)w & 0xFFFFFFu; }
inline uint32_t code(uint32_t l) { return l & 0xFFFFFFu; }

#ifdef DK_NATIVE
/* The native build (phase 5 ruling R8): the same calls, done by the native flow runtime
   (game/runtime/flow.cpp: a register record, A7 and the stack as data at the original addresses,
   return tokens, the bridge's table from code address to C; nothing of the 68000 under it). The
   C names them as dk::jump etc. in both modes. */
namespace flow {
void jump(uint32_t code);
void jump_script(uint32_t vm, uint32_t script);
void rte(void);
uint32_t stack_pointer(void);
void set_stack_pointer(uint32_t sp);
uint32_t return_address(void);
void set_return_address(uint32_t a);
void drop_return(void);
uint32_t pop_long(void);
void push_long(uint32_t v);
void push_word(uint16_t v);
void set_interrupt_mask(int level);
uint16_t status_register(void);
void set_status_register(uint16_t sr);
uint32_t usp(void);
void set_usp(uint32_t v);
}  /* namespace flow */
using flow::jump;
using flow::jump_script;
using flow::rte;
using flow::stack_pointer;
using flow::set_stack_pointer;
using flow::return_address;
using flow::set_return_address;
using flow::drop_return;
using flow::pop_long;
using flow::push_long;
using flow::push_word;
using flow::set_interrupt_mask;
using flow::status_register;
using flow::set_status_register;
using flow::usp;
using flow::set_usp;
#else
/* Leave: continue at `code` with the outputs as the prototype hands them back (jmp: a script
   NEXT, a computed jump, a jump table to another function). */
void jump(uint32_t code);

/* Leave: `jsr vm` with the script inline after the call: the interpreter pops its return
   address as the script pointer (a [disasm] noreturn VM entry). `script` is the ROM address of
   the bytes after the original call. */
void jump_script(uint32_t vm, uint32_t script);

/* Leave: RTE (an interrupt handler's return: SR and PC from the stack). */
void rte(void);

/* The stack pointer, for the VMs that save it at entry and yield by restoring it
   (`move.l a7,(g_X)` ... `movea.l (g_X),a7` / `rts`): save it, and set it before returning. */
uint32_t stack_pointer(void);
void set_stack_pointer(uint32_t sp);

/* The return address this function was called with: arguments inline after the caller's `jsr`
   (read them, then move it past them), or the script a VM entry runs. */
uint32_t return_address(void);
void set_return_address(uint32_t a);

/* `addq.l #4,a7` / `rts`: drop the return address, so the return goes to the caller's caller. */
void drop_return(void);

/* `move.l (a7)+,x` / `move.l x,-(a7)`: a long the caller left on the stack for this function
   (movem save / restore split across a jump), or one it leaves for the next; a word the same. */
uint32_t pop_long(void);
void push_long(uint32_t v);
void push_word(uint16_t v);

/* `ori.w #$700,sr` / `andi.w #$F8FF,sr`: the interrupt mask (0-7). */
void set_interrupt_mask(int level);

/* `move.w sr,dN`: the status register (the system byte with the interrupt mask, and the condition
   codes), for code that saves it (a task context an RTE restores later); `move.w #$2700,sr`: set
   all of it (a reset handler; the interrupt mask alone is set_interrupt_mask). */
uint16_t status_register(void);
void set_status_register(uint16_t sr);

/* `move.l usp,aN` / `move.l aN,usp`: the user stack pointer, which a supervisor-mode game uses as
   a spare register (a value parked across a branch into another function). */
uint32_t usp(void);
void set_usp(uint32_t v);
#endif /* DK_NATIVE */

/* return_address() as a pointer: the data inline after the caller's `jsr` (or the script a VM
   entry runs), and moving the return address past it. */
template <class T> const T *return_data(void) { return (const T *)host(return_address()); }
inline void set_return_data(const void *p) { set_return_address(addr(p)); }

/* Condition codes a function hands back (a caller branches on them after the return): the `ccr`
   pseudo-register of the prototype's regs(...), a Flags parameter. Build them the way the asm
   left them: compare(a, b, bytes) is `cmp.s b,a` (a - b), test(v, bytes) is `tst.s v`. */
struct Flags {
    bool x, n, z, v, c;
    static Flags compare(uint32_t a, uint32_t b, int bytes, bool x = false)
    {
        uint32_t m = bytes >= 4 ? 0xFFFFFFFFu : bytes == 2 ? 0xFFFFu : 0xFFu;
        uint32_t s = bytes >= 4 ? 0x80000000u : bytes == 2 ? 0x8000u : 0x80u;
        uint32_t r = (a - b) & m;
        a &= m;
        b &= m;
        return Flags{ x, (r & s) != 0, r == 0, (((a ^ b) & (a ^ r)) & s) != 0, b > a };
    }
    /* the condition codes of an SR word (one an RTE restores) */
    static Flags from_sr(uint16_t sr)
    {
        return Flags{ (sr & 0x10) != 0, (sr & 0x8) != 0, (sr & 0x4) != 0, (sr & 0x2) != 0, (sr & 0x1) != 0 };
    }
    static Flags test(uint32_t v, int bytes, bool x = false)
    {
        uint32_t m = bytes >= 4 ? 0xFFFFFFFFu : bytes == 2 ? 0xFFFFu : 0xFFu;
        uint32_t s = bytes >= 4 ? 0x80000000u : bytes == 2 ? 0x8000u : 0x80u;
        return Flags{ x, (v & s) != 0, (v & m) == 0, false, false };
    }
};

}  /* namespace dk */

#endif /* DK_FLOW_H */
