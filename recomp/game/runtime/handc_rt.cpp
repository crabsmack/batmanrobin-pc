/* handc_rt.cpp - the runtime under the hand-written C (phase 4; from xmen2's): with the
 * generated bridge, the only code besides the generated code and the engine that sees the
 * 68000's registers. Written by the kit's recomp template; yours to extend.
 *
 * Hand-written 68000 has no uniform calling convention: each function's is its prototype's tag
 * (dkit genesis recomp bridge: regs(...), ->r, tst(...)), measured by `dkit disasm regs`.
 *   - C -> generated: the bridge's stub saves every register, sets the inputs, calls the code
 *     like a generated JSR (dk::call_gen: the callee's RTS pops into g_xrts), reads the outputs
 *     and restores every register.
 *   - generated -> C: a slot entry (hcslot_ADDR) reads the inputs, writes the outputs and
 *     returns with dk_slot_return (an RTS), unless the C left another way (dk_flow.h: g_left)
 *     or a call it made never came back to it (Unwind: the 68000 returned further out).
 *   - C -> C goes the same way (stub, then slot): every call from C is a 68000 call, so stack
 *     tricks (yields, return-address reads, returns past the caller) work as on the hardware.
 */
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

extern "C" {
#include "genesis_runtime.h"
extern uint32_t g_xrts;
}
#include "dk_flow.h"

namespace dk {

struct Unwind {};        /* dk_bridge.h's: thrown by call_gen, caught by the slot entries */

char g_fatal_msg[512];   /* the fatal error ending the run: verify.cpp's report names it */
uint32_t g_fn_current;   /* the C function running (dk_bridge.h FnScope) */

[[noreturn]] void fatal(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(g_fatal_msg, sizeof g_fatal_msg, fmt, ap);
    va_end(ap);
    fprintf(stderr, "[handc] %s (C function $%06X)\n", g_fatal_msg, (unsigned)g_fn_current);
    fflush(stderr);
    exit(5);
}

/* A return address no 68000 code has (odd, in the unused $F00000 area): the RTS of the callee
   hands it back in g_xrts, which proves the call returned to us. Each call from C nested in
   another gets its own (RET_FROM_C + 2 x the depth), as each generated JSR site has its own
   return address: an RTS that skips frames (`addq.l #8,a7 / rts`, a return past the caller and
   its caller) and lands on an OUTER C call's return slot must unwind every C frame in between,
   not be taken by the innermost call (kit <= 0.9.4: one address for all, so the innermost took it
   and stopped on "came back with A7 ..."). */
static const uint32_t RET_FROM_C = 0xF00001u;
static const int C_DEPTH_MAX = 0x7FFF;   /* RET_FROM_C .. $F0FFFF */
static int s_c_depth;                    /* calls from C in progress */

/* Exact mode's returns nobody waits for: an RTS to an address no call site waits for is run on
   by the dispatcher, as the 68000 continues there. A call from C waits for its own return
   address (RET_FROM_C), exactly as a generated JSR waits for its own: an RTS that lands where no
   site waits (threaded code: `pea VM6_Next(pc)` then a helper's rts; `move.l (a1),-(sp) / rts`,
   a jump through a pushed address) runs on inside the call, and only a return to an address an
   enclosing site waits for (a yield, a return past the caller) unwinds the C. Waiting for ANY
   return (XAWAIT_ANY, kit <= 0.9.1) took those inner RTSes as the callee's and unwound the C. */
#ifdef XAWAIT_ANY
#define CALL_FROM_C(ret, call) do { XCALL_BEGIN(ret); call; XCALL_END(); \
                                    if (g_xrts == (ret)) XRTS_TAKEN(); } while (0)
#else
#define CALL_FROM_C(ret, call) call
#endif

int g_left;              /* the running C function left by dk_flow.h (its slot doesn't RTS) */

void call_gen(uint32_t a)
{
    uint32_t sp = g_cpu.A[7];
    if (s_c_depth >= C_DEPTH_MAX)
        fatal("calls from C nested %d deep (handc_rt.cpp C_DEPTH_MAX)", s_c_depth);
    const uint32_t ret = RET_FROM_C + 2u * (uint32_t)s_c_depth++;
    recomp_push_return(ret);
    CALL_FROM_C(ret, recomp_call_addr(a & 0xFFFFFFu));
    s_c_depth--;
    if (g_rte_pending || g_xrts != ret) {
        static int log = -1;
        if (log < 0) { const char *e = getenv("DK_UNWIND_LOG"); log = e && *e; }
        if (log)   /* diagnostics: where the 68000 went instead of back to this C */
            fprintf(stderr, "[handc] unwind: call of $%06X from C $%06X: rte_pending %d, rts to $%06X, A7 $%08X (call's $%08X, returns to $%06X)\n",
                    (unsigned)(a & 0xFFFFFFu), (unsigned)g_fn_current, g_rte_pending, (unsigned)g_xrts,
                    (unsigned)g_cpu.A[7], (unsigned)sp, (unsigned)ret);
        throw Unwind{};   /* the 68000 went further out (a yield, a return past its caller): so does this C */
    }
    if (g_cpu.A[7] != sp)
        fatal("call of $%06X from C came back with A7 $%08X (expected $%08X)", (unsigned)(a & 0xFFFFFFu),
              (unsigned)g_cpu.A[7], (unsigned)sp);
}

}  /* namespace dk */

/* A branch plan's snapshot (the runner's --branch-plan: rb_state.h, in this process) holds the
   machine and the game fiber's stack, so the C frames on that stack come back with it; the state
   this runtime keeps about them must come back too. Above all the depth: each call from C returns
   to RET_FROM_C + 2 x its depth, and a depth left by the branch before (Batman & Robin: a branch
   that ended with the C at another depth) gave a new call the return address of a call the
   restored stack still waits for, so an RTS past it was taken as the new call's own ("came back
   with A7 $FFFFFFFC (expected $FFFFFFF8)" 40 sync points before the branch's end; the run alone
   passed). spec.c's branch_save/restore call these (DK_HANDC). */
static int s_branch_depth, s_branch_left;
static uint32_t s_branch_fn;

extern "C" void dk_handc_branch_save(void)
{
    s_branch_depth = dk::s_c_depth;
    s_branch_left = dk::g_left;
    s_branch_fn = dk::g_fn_current;
}

extern "C" void dk_handc_branch_restore(void)
{
    dk::s_c_depth = s_branch_depth;
    dk::g_left = s_branch_left;
    dk::g_fn_current = s_branch_fn;
}

namespace dk {

/* dk_flow.h */
void jump(uint32_t code)
{
    recomp_tail_call(code & 0xFFFFFFu);
    g_left = 1;
}

void jump_script(uint32_t vm, uint32_t script)
{
    recomp_push_return(script & 0xFFFFFFu);
    recomp_tail_call(vm & 0xFFFFFFu);
    g_left = 1;
}

void rte(void)
{
    g_cpu.SR = (uint16_t)m68k_read16(g_cpu.A[7]);
    g_cpu.A[7] += 2;
    g_cpu.PC = m68k_read32(g_cpu.A[7]);
    g_cpu.A[7] += 4;
    XSR();
    x68k_rte();
    g_left = 1;
}

uint32_t stack_pointer(void) { return g_cpu.A[7]; }
void set_stack_pointer(uint32_t sp) { g_cpu.A[7] = sp; }
uint32_t return_address(void) { return m68k_read32(g_cpu.A[7]); }
void set_return_address(uint32_t a) { m68k_write32(g_cpu.A[7], a); }
void drop_return(void) { g_cpu.A[7] += 4; }
uint32_t pop_long(void) { uint32_t v = m68k_read32(g_cpu.A[7]); g_cpu.A[7] += 4; return v; }
void push_long(uint32_t v) { g_cpu.A[7] -= 4; m68k_write32(g_cpu.A[7], v); }
void push_word(uint16_t v) { g_cpu.A[7] -= 2; m68k_write16(g_cpu.A[7], v); }
void set_interrupt_mask(int level) { g_cpu.SR = (uint16_t)((g_cpu.SR & ~0x0700u) | ((unsigned)(level & 7) << 8)); }
uint16_t status_register(void) { return g_cpu.SR; }
void set_status_register(uint16_t sr) { g_cpu.SR = sr; XSR(); }
uint32_t usp(void) { return g_cpu.USP; }
void set_usp(uint32_t v) { g_cpu.USP = v; }

}  /* namespace dk */

/* the ccr pseudo-register (dk_flow.h Flags) to and from SR's condition codes: the bridge's */
extern "C" void dk_flags_get(dk::Flags *f)
{
    uint16_t sr = g_cpu.SR;
    *f = dk::Flags{ (sr & 0x10) != 0, (sr & 0x8) != 0, (sr & 0x4) != 0, (sr & 0x2) != 0, (sr & 0x1) != 0 };
}

extern "C" void dk_flags_set(const dk::Flags *f)
{
    g_cpu.SR = (uint16_t)((g_cpu.SR & ~0x1Fu) | (f->x ? 0x10 : 0) | (f->n ? 0x8 : 0) | (f->z ? 0x4 : 0)
                          | (f->v ? 0x2 : 0) | (f->c ? 0x1 : 0));
}

namespace dk {

}  /* namespace dk */

extern "C" uint32_t dk_reg(int r)
{
    return r < 8 ? g_cpu.D[r] : g_cpu.A[r - 8];
}

extern "C" void dk_set_reg(int r, uint32_t v, int bytes)
{
    if (r >= 8) {   /* an address register is always written whole (word writes sign-extend) */
        g_cpu.A[r - 8] = bytes == 2 ? (uint32_t)(int32_t)(int16_t)v : v;
        return;
    }
    uint32_t &d = g_cpu.D[r];
    if (bytes >= 4)      d = v;
    else if (bytes == 2) d = (d & 0xFFFF0000u) | (v & 0xFFFFu);
    else                 d = (d & 0xFFFFFF00u) | (v & 0xFFu);
}

extern "C" void dk_set_flag(int bit, int on)
{
    g_cpu.SR = (uint16_t)((g_cpu.SR & ~(1u << bit)) | ((on ? 1u : 0u) << bit));
}

/* tst.s r: N and Z from r at that width, V and C clear, X kept */
extern "C" void dk_tst(int r, int bytes)
{
    uint32_t v = dk_reg(r);
    uint32_t m = bytes >= 4 ? 0xFFFFFFFFu : bytes == 2 ? 0xFFFFu : 0xFFu;
    uint32_t sign = bytes >= 4 ? 0x80000000u : bytes == 2 ? 0x8000u : 0x80u;
    uint16_t sr = (uint16_t)(g_cpu.SR & ~0x000Fu);
    if (!(v & m)) sr |= 0x4;
    if (v & sign) sr |= 0x8;
    g_cpu.SR = sr;
}

/* d0-d7, a0-a6 (A7 is the call's own business). */
extern "C" void dk_regs_save(uint32_t *saved)
{
    for (int i = 0; i < 8; i++) saved[i] = g_cpu.D[i];
    for (int i = 0; i < 7; i++) saved[8 + i] = g_cpu.A[i];
}

extern "C" void dk_regs_restore(const uint32_t *saved)
{
    for (int i = 0; i < 8; i++) g_cpu.D[i] = saved[i];
    for (int i = 0; i < 7; i++) g_cpu.A[i] = saved[8 + i];
}

/* The slot entry's RTS, exactly as generated code's: the return address into g_xrts (its top
   byte into g_pchi), A7 popped, and the return marked for the call site to take. */
extern "C" void dk_slot_return(void)
{
    uint32_t ret = m68k_read32(g_cpu.A[7]);
    g_pchi = ret & 0xFF000000u;
    g_xrts = ret & 0xFFFFFFu;
    g_cpu.A[7] += 4;
    g_xrts_new = 1;
}
