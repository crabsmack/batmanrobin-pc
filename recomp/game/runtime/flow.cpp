/* flow.cpp - the native flow runtime (phase 5 ruling R8; dk_flow_rt.h): how the hand-written C's
 * calls, jumps, yields, returns past the caller and RTEs work with no 68000 under them. Written
 * by the kit's recomp template (from the phase-4 runtime handc_rt.cpp, whose rules it keeps
 * exactly, so the native build's game state equals the emulated runners' at every sync point).
 *
 * It references nothing of the 68000 machine (the object scan checks: `dkit genesis recomp
 * scan`, flow): the registers are its own record, the stack is work RAM through the platform's
 * memory hooks, the code addresses go through the bridge's table, and interrupts, sync points
 * and the V-int wait are the platform's (Host). What it keeps of the 68000 is what the game's
 * own data shows: A7's values and the stack's bytes at their original addresses.
 *
 * The rules (the emulated runners' engine and handc_rt.cpp, mirrored):
 *   - a run loop dispatches its pending address, then: a jump (dk::jump) is the next address; an
 *     RTS no call waits for is the next address (threaded code: `pea next(pc)` + a handler's
 *     rts); an RTS some call waits for, or an RTE, ends the loop;
 *   - a call from C pushes RET_FROM_C + 2 x its depth (an odd address in the unused $F00000
 *     area: no code has it) and waits for it; when its loop ends on another address or an RTE,
 *     the C caller unwinds (dk::Unwind, caught by the slot of the C function the 68000 returned
 *     into: it returns without an RTS, as the 68000 never came back to it);
 *   - an interrupt runs its handler in a loop of its own and must end with an RTE to where it
 *     interrupted (phase-5 ruling R9: no task switches, no RTE outside an interrupt).
 */
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dk_flow_rt.h"

namespace dk {

char g_fatal_msg[512];   /* the fatal error ending the run */
uint32_t g_fn_current;   /* the C function running (the bridge's FnScope) */

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

namespace flow {

Regs g_regs;
int g_left;

static const uint32_t RET_FROM_C = 0xF00001u;
static const int C_DEPTH_MAX = 0x7FFF;   /* RET_FROM_C .. $F0FFFF */

/* A run loop: one per call from C (awaiting its token), per interrupt, and the reset's. Each lives
   on the game's own stack, innermost first from s_top (a branch snapshot carries them). */
struct Frame {
    Frame *prev;
    uint32_t token;   /* the return address its call waits for (awaits) */
    int awaits;
    int pending;      /* addr is the next code to run */
    uint32_t addr;
};

static Host s_host;
static Frame *s_top;        /* the innermost run loop: dk::jump sets its next address */
static int s_depth;         /* calls from C in progress */
static int s_irq;           /* interrupt handlers running */
static uint32_t s_ret;      /* where the last RTS returned */
static int s_ret_new;       /* that RTS is not taken yet */
static int s_rte;           /* an RTE ended the running handler */
static uint32_t s_rte_pc;   /* the PC it popped */

void install(const Host &h) { s_host = h; }

/* ---- the stack: work RAM at A7, through the platform ---- */

static uint32_t rd32(uint32_t a)
{
    if (!s_host.read32) fatal("flow: no memory installed (dk::flow::install)");
    return s_host.read32(a);
}
static uint16_t rd16(uint32_t a) { return s_host.read16(a); }
static void wr32(uint32_t a, uint32_t v) { s_host.write32(a, v); }
static void wr16(uint32_t a, uint16_t v) { s_host.write16(a, v); }

static void push32(uint32_t v) { g_regs.a[7] -= 4; wr32(g_regs.a[7], v); }
static uint32_t pop32(void) { uint32_t v = rd32(g_regs.a[7]); g_regs.a[7] += 4; return v; }

/* ---- the code-address table ---- */

static void (*find(uint32_t a))(void)
{
    int lo = 0, hi = dk_code_count;
    while (lo < hi) {
        int mid = lo + (hi - lo) / 2;
        if (dk_code_table[mid].addr < a) lo = mid + 1;
        else hi = mid;
    }
    return lo < dk_code_count && dk_code_table[lo].addr == a ? dk_code_table[lo].slot : nullptr;
}

static void dispatch(uint32_t a)
{
    a &= 0xFFFFFFu;
    if (void (*slot)(void) = find(a)) {
        if (s_host.entry) s_host.entry(a);
        slot();
        return;
    }
    if (a >= 0xFF0000u && s_host.ram_code && s_host.ram_code(a))
        return;
    fatal("flow: no C for code address $%06X (the bridge's table: %d entries; R8: any other address is fatal)",
          (unsigned)a, dk_code_count);
}

/* A fresh RTS no enclosing call waits for: where the 68000 continues (1, *a). One a call waits
   for is left to that call (0): its loop ends, and every loop in between. */
static int resume(uint32_t *a)
{
    if (!s_ret_new) return 0;
    for (const Frame *f = s_top; f; f = f->prev)
        if (f->awaits && f->token == s_ret) return 0;
    s_ret_new = 0;
    *a = s_ret;
    return 1;
}

static void run(Frame &f)
{
    for (;;) {
        if (!f.pending && !s_rte && resume(&f.addr)) f.pending = 1;
        if (!f.pending) break;
        uint32_t a = f.addr;
        f.pending = 0;
        dispatch(a);
        if (s_rte) break;
    }
}

/* A loop pushed on s_top for its lifetime (also when a fatal or an exception leaves it). */
struct Push {
    Frame &f;
    explicit Push(Frame &fr) : f(fr) { f.prev = s_top; s_top = &f; }
    ~Push() { s_top = f.prev; }
};

void call(uint32_t code)
{
    uint32_t sp = g_regs.a[7];
    if (s_depth >= C_DEPTH_MAX)
        fatal("flow: calls from C nested %d deep (C_DEPTH_MAX)", s_depth);
    const uint32_t token = RET_FROM_C + 2u * (uint32_t)s_depth++;
    push32(token);
    Frame f{ nullptr, token, 1, 1, code & 0xFFFFFFu };
    {
        Push p(f);
        run(f);
    }
    if (s_ret == token) s_ret_new = 0;   /* taken */
    s_depth--;
    if (s_rte || s_ret != token) {
        static int log = -1;
        if (log < 0) { const char *e = getenv("DK_UNWIND_LOG"); log = e && *e; }
        if (log)
            fprintf(stderr, "[flow] unwind: call of $%06X from C $%06X: rte %d, rts to $%06X, A7 $%08X (call's $%08X, returns to $%06X)\n",
                    (unsigned)(code & 0xFFFFFFu), (unsigned)g_fn_current, s_rte, (unsigned)s_ret,
                    (unsigned)g_regs.a[7], (unsigned)sp, (unsigned)token);
        throw Unwind{};   /* the code went further out (a yield, a return past its caller): so does this C */
    }
    if (g_regs.a[7] != sp)
        fatal("flow: call of $%06X from C came back with A7 $%08X (expected $%08X)", (unsigned)(code & 0xFFFFFFu),
              (unsigned)g_regs.a[7], (unsigned)sp);
}

void rts(void)
{
    s_ret = pop32() & 0xFFFFFFu;
    s_ret_new = 1;
}

[[noreturn]] void reset(uint32_t entry)
{
    Frame f{ nullptr, 0, 0, 1, entry & 0xFFFFFFu };
    {
        Push p(f);
        run(f);
    }
    fatal("flow: the reset code at $%06X returned (to $%06X)", (unsigned)(entry & 0xFFFFFFu), (unsigned)s_ret);
}

void interrupt(unsigned level, uint32_t pc)
{
    const uint32_t vector = rd32((24u + level) * 4u) & 0xFFFFFFu;
    const uint16_t sr = g_regs.sr;
    g_regs.sr = (uint16_t)((sr & ~0x8000u) | 0x2000u);                      /* S set, T clear */
    g_regs.sr = (uint16_t)((g_regs.sr & ~0x0700u) | ((level & 7u) << 8));   /* the mask at the level */
    push32(pc);
    g_regs.a[7] -= 2;
    wr16(g_regs.a[7], sr);
    const int saved_rte = s_rte, saved_left = g_left;
    s_rte = 0;
    s_irq++;
    Frame f{ nullptr, 0, 0, 1, vector };
    {
        Push p(f);
        run(f);
    }
    s_irq--;
    if (!s_rte)
        fatal("flow: the interrupt handler $%06X returned without RTE (level %u)", (unsigned)vector, level);
    if ((s_rte_pc & 0xFFFFFFu) != (pc & 0xFFFFFFu))
        fatal("flow: the interrupt handler $%06X returned to $%06X, not where it interrupted ($%06X): a task "
              "switch (ruling R9: the V-int's background-task parking is not in the native build)",
              (unsigned)vector, (unsigned)(s_rte_pc & 0xFFFFFFu), (unsigned)(pc & 0xFFFFFFu));
    s_rte = saved_rte;
    g_left = saved_left;
}

void irq_wait(void)
{
    if (!s_host.irq_wait) fatal("flow: no V-int wait installed (dk::flow::install)");
    s_host.irq_wait();
}

/* ---- dk_flow.h ---- */

void jump(uint32_t code)
{
    if (!s_top) fatal("flow: dk::jump($%06X) outside any run loop", (unsigned)(code & 0xFFFFFFu));
    s_top->addr = code & 0xFFFFFFu;
    s_top->pending = 1;
    g_left = 1;
}

void jump_script(uint32_t vm, uint32_t script)
{
    push32(script & 0xFFFFFFu);
    jump(vm);
}

void rte(void)
{
    g_regs.sr = rd16(g_regs.a[7]);
    g_regs.a[7] += 2;
    s_rte_pc = pop32();
    if (!s_irq)
        fatal("flow: an RTE outside an interrupt (to $%06X with SR $%04X): a task switch (ruling R9: "
              "BgTask_Resume's resume of a parked task is not in the native build)",
              (unsigned)(s_rte_pc & 0xFFFFFFu), (unsigned)g_regs.sr);
    s_rte = 1;
    g_left = 1;
}

uint32_t stack_pointer(void) { return g_regs.a[7]; }
void set_stack_pointer(uint32_t sp) { g_regs.a[7] = sp; }
uint32_t return_address(void) { return rd32(g_regs.a[7]); }
void set_return_address(uint32_t a) { wr32(g_regs.a[7], a); }
void drop_return(void) { g_regs.a[7] += 4; }
uint32_t pop_long(void) { return pop32(); }
void push_long(uint32_t v) { push32(v); }
void push_word(uint16_t v) { g_regs.a[7] -= 2; wr16(g_regs.a[7], v); }
void set_interrupt_mask(int level) { g_regs.sr = (uint16_t)((g_regs.sr & ~0x0700u) | ((unsigned)(level & 7) << 8)); }
uint16_t status_register(void) { return g_regs.sr; }
void set_status_register(uint16_t sr) { g_regs.sr = sr; }
uint32_t usp(void) { return g_regs.usp; }
void set_usp(uint32_t v) { g_regs.usp = v; }

/* ---- branch plans ---- */

struct Saved {
    Regs regs;
    Frame *top;
    int depth, irq, left, ret_new, rte;
    uint32_t ret, rte_pc, fn;
};
static Saved s_saved;

void branch_save(void)
{
    s_saved = Saved{ g_regs, s_top, s_depth, s_irq, g_left, s_ret_new, s_rte, s_ret, s_rte_pc, g_fn_current };
}

void branch_restore(void)
{
    g_regs = s_saved.regs;
    s_top = s_saved.top;
    s_depth = s_saved.depth;
    s_irq = s_saved.irq;
    g_left = s_saved.left;
    s_ret_new = s_saved.ret_new;
    s_rte = s_saved.rte;
    s_ret = s_saved.ret;
    s_rte_pc = s_saved.rte_pc;
    g_fn_current = s_saved.fn;
}

}  /* namespace flow */
}  /* namespace dk */
