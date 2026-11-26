/* verify.cpp - shadow differential tests of the hand-written C (phase 4 D4: `dkit genesis recomp
 * verify`; from xmen2's game/runtime/verify.cpp). Written by the kit's recomp template.
 *
 * Built into the verify runner: the timed runner with ALL generated code, which plays a corpus
 * input exactly as the reference does, plus the hand-written C. At every call of a function F
 * that has C (the generated wrapper's XENTRY hook, GameSpec.on_entry):
 *   1. snapshot RAM and the registers; run F's generated code, recording every hardware access
 *      and the instructions of F it executes;
 *   2. if an interrupt was taken meanwhile, that run is not comparable (the C runs untimed: no
 *      interrupt can land in it at the same point): keep its result, and compare the call with
 *      the interrupt deferred instead (below), unless the run waited for the V-int;
 *   3. else roll RAM and registers back and run F's C through its slot entry, untimed, with the
 *      recorded hardware reads replayed in order and its hardware writes captured (not
 *      performed); whatever F calls runs as generated code ("nested calls run plain");
 *   4. compare: RAM outside the dead stack ([stack_low, the returned A7)) and [recomp] dont_care, A7, the return
 *      target, every register (an output as wide as its C type; clobbers() skipped; any other
 *      must come back as the generated code left it), SR's system byte and the condition codes
 *      F hands back (dk_verify_fns.h), and the hardware access sequence;
 *   5. put the generated result back and continue: the game always runs the generated code.
 * A call that passes adds the instructions it executed to F's covered set.
 *
 * A call an interrupt landed in (kit 0.9.6; Batman & Robin: the stage-3 road's H-int every line,
 * the V-int in 224-line fills, made 73 functions' every call incomparable): its timed run logs only
 * the main program's hardware accesses and instructions (an interrupt handler's, from its first
 * instruction until A7 is back above its exception frame, are left out). Then, from the same entry
 * state (RAM, registers, return slot), the generated code runs again untimed with interrupts held,
 * its hardware reads supplied in order from the timed run's main-program reads and its writes
 * captured, not performed; the C runs from that state too, replaying them (step 3); the two are
 * compared as in step 4, nothing masked. Both runs see the same inputs, so a difference is the
 * C's; the call counts as checked (and `irq_deferred`), and adds the instructions both its timed
 * run and the deferred run executed. Then the timed generated result is put back (step 5).
 * Not deferred: a run that reached a [recomp.sync] wait_vint site (it waits for the V-int, so
 * holding it would spin for ever: Frame_End, the scene loops), counted as `irq_waits` (and
 * irq_skipped); a deferred run of more than 16x the timed run's instructions + 1M is a fatal
 * error (a wait for an interrupt no wait_vint site names), not a silent hang.
 *
 * Nesting: a function G with C called inside F's generated run (step 1) is tested there the same
 * way, as part of F's run (up to MAX_DEPTH deep); never inside a C run or a synthetic one.
 * Otherwise a callee of a function that is tested at every call would never be tested.
 *
 * Synthetic cases (captured states mutated to reach other paths, run the same way):
 * DKIT_VERIFY_SYNTH=<dir> holds <ADDR>.syn files (format: load_synth below). At the first call
 * of F (or, `at any`, the first top-level call of any function: for functions the corpus never
 * runs; or `at $ADDR`, the first call of function ADDR) each case mutates that entry state (RAM, registers, SR; ROM too, for a path a constant of
 * this ROM rules out) and runs both versions from it, untimed (no interrupts), with hardware reads
 * supplied by the case (default 0) and hardware writes captured, not performed; then everything is
 * put back. Passing cases count as checked calls and add coverage; failing ones as mismatches.
 *
 * DKIT_VERIFY_OUT=<path> writes {"stopped_early": null or why, "frames", "functions_tested",
 * "calls_checked", "calls_refused", "functions": {"ADDR": {"calls_checked", "mismatches",
 * "irq_skipped", "irq_deferred", "irq_waits", "synthetic_checked", "too_deep", "probes", "first_mismatch",
 * "covered":
 * [pc, ...]}}} at exit;
 * Sampling: every call of a function until 2000 have been checked, then one in 64. A call the
 * sampling skips is still a probe (run recorded, compared only if it reached an instruction no
 * passing call or case of its function file covered yet) while its file has such instructions
 * that the reference executed in the run being played (DKIT_VERIFY_TARGETS, kit 0.9.10: per
 * member run of the process, in play order, the run's exec map from the corpus coverage); a run
 * without one falls back to the file's instruction count (DKIT_VERIFY_INSNS), which only the
 * file's head entry can ever complete.
 * DKIT_VERIFY_ONLY=ADDR,... restricts the tests to those functions. DKIT_VERIFY_DIFF=1 prints each
 * mismatch in full to stderr (the runner.log): every differing RAM byte with its value before the
 * call, both register files, the lowest A7 each run reached and the instructions each ran (the C
 * run's: the generated code it called).
 *
 * Testing that stops is loud (xmen2: two full verifies ran on for ~1.5 h after testing had
 * stopped, with 498 and 416 functions checked). `stopped_early` is set when the run ended (a) on
 * a fatal error (dk::fatal), (b) inside a C run or a synthetic case (they are untimed: one still
 * open at the end never returned), or (c) after STOP_FRAMES or more frames in which every call of
 * a function under test was refused because no test could open. `verify` fails on it.
 * DKIT_VERIFY_DEV_STOP=<frame> refuses every test from that frame on, to prove the check.
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <algorithm>
#include <set>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

extern "C" {
#include "genesis_runtime.h"
extern uint8_t g_rom[0x400000];
extern uint32_t g_xclk, g_xstart, g_xend, g_xrts;
extern int g_xtrace;
uint32_t dk_stack_low(void);   /* spec.c: the sync rows' dead-stack bound */
const uint8_t *dk_dont_care_map(void);   /* spec.c: [recomp] dont_care, 1 per masked RAM byte */
void dk_install_crash_report(void (*report)(unsigned long code, void *addr));   /* crash_win.cpp */
int input_script_mutates_state(void);   /* runner/input_script.c: the script writes RAM (`@poke`) */
}
#include "dk_verify_fns.h"
#include "../dkit_sync.h"     /* [recomp.sync] wait_vint: dk_wait_sites */

namespace dk {
extern char g_fatal_msg[];    /* handc_rt.cpp */
extern uint32_t g_fn_current;
[[noreturn]] void fatal(const char *fmt, ...);
}

namespace {

/* Tests open at once. Every test counts, even one that will never be compared (a scene loop that
   waits for the V-int stays open for the whole scene), so a game's frame nests deep: Batman &
   Robin's title menu natives run under Main_* > Title_Run > Scene_Loop* > Frame_RunWorld >
   Obj_RunList > VM2_RunObject > ... at level 16, which 16 refused at every call (kit <= 0.9.7:
   35 files with 0 checked calls whole-corpus, each checked when tested alone). 40 and 64 measured
   the same there; a call refused for depth counts as `too_deep`. */
const int MAX_DEPTH = 64;
/* refused calls this many frames in a row, up to the end of the run: testing stopped */
const uint64_t STOP_FRAMES = 60;

struct Stat {
    uint64_t checked = 0, mismatches = 0, irq = 0, seen = 0, synth = 0, deferred = 0, waits = 0, deep = 0;
    uint64_t probes = 0;   /* sampled-out calls run as probes (compared or not) */
    bool synth_done = false;
    std::string first;
    std::unordered_set<uint32_t> covered;
    /* instructions deferred runs were tried for and didn't cover (the timed run took them, the
       deferred one not: it read what the interrupt wrote): a sampled-out call is a probe for one
       of them at most DEFER_TRIES times, or every such call would be compared twice over */
    std::unordered_map<uint32_t, uint8_t> tries;
};
const uint8_t DEFER_TRIES = 4;
struct Hw {
    uint32_t addr; int size; int write; uint16_t v;
    bool operator==(const Hw &o) const { return addr == o.addr && size == o.size && write == o.write && v == o.v; }
};

struct Assign { int what; int size; std::string addr; std::string value; };   /* what: 0 memory, 1 register, 2 SR */
/* `poke N` in a case: a RAM write once the run has made N hardware reads (resolved at the case's
   start); both runs make the same reads in the same order, so both see it at the same point */
struct Poke { size_t after; uint32_t addr; int size; uint32_t value; };
struct SynCase {
    std::string name;
    std::vector<Assign> sets;
    std::vector<uint16_t> reads;
    std::vector<std::pair<size_t, Assign>> pokes;
};
struct SynSpec { bool at_any = false; uint32_t at = 0; std::vector<SynCase> cases; };   /* at: `at ADDR` */

/* One test in progress (a nesting level). */
struct Ctx {
    const DkVerifyFn *fn;
    int mode;                      /* 1 record (generated run), 2 replay (C run), 3 synthetic generated run */
    uint8_t pre[0x10000], post[0x10000];
    std::vector<Hw> logA, logB;
    size_t readpos;
    std::vector<uint32_t> cov;
    const std::vector<uint16_t> *supply;
    size_t supply_pos;
    const std::vector<Poke> *pokes;
    size_t nreads;
    uint64_t irq0;
    uint64_t frame0;
    bool doomed;                   /* its generated run took an interrupt: compared deferred */
    bool waited;                   /* ... and reached a wait_vint site: not compared at all */
    int irq_level;                 /* interrupt handlers open (s_irq_n) when it opened */
    uint64_t ninsn;                /* main-program instructions its run executed (mode 1) */
    uint64_t budget;               /* mode 3: instructions allowed, 0 = unbounded */
    uint32_t a7lo, a7lo_gen;       /* DKIT_VERIFY_DIFF: the lowest A7 of the run so far, of its generated run */
    std::vector<uint32_t> path, path_gen;   /* DKIT_VERIFY_DIFF: the instructions it ran (the top run) */
};

std::unordered_map<uint32_t, const DkVerifyFn *> s_fns;
/* Static storage, not the heap: a test open across a frame's end refers to its Ctx and to its
   function's Stat from the game fiber's stack, which a branch's restore puts back. */
const int MAX_FNS = 4096;
Stat s_stat_store[MAX_FNS];
Ctx s_ctx_store[MAX_DEPTH + 1];   /* + 1: the deferred run of a test at MAX_DEPTH */
Stat &stat_of(const DkVerifyFn *fn) { return s_stat_store[fn - dk_verify_fns]; }
std::unordered_map<uint32_t, SynSpec> s_synth;
std::unordered_set<uint32_t> s_at;   /* the functions `at ADDR` cases wait for */
Ctx *s_ctx[MAX_DEPTH + 1];
int s_depth;
uint32_t s_entering;              /* the function whose generated code is being started */
bool s_any_done;
uint64_t s_refused, s_refused_since;
uint64_t s_dev_stop;
/* DKIT_VERIFY_DIFF=1: each mismatch also prints, to stderr, every differing RAM byte (with its
   value before the call), both register files and the lowest A7 each run reached */
bool s_diff;

/* The interrupt handlers running inside the open tests: each one's exception frame (A7 at its
   first instruction). A handler runs until A7 is back above its frame (its RTE); the main program
   always runs above it. Updated at instructions while a timed run is on top. */
const int MAX_IRQ = 64;
uint32_t s_irq_base[MAX_IRQ];
int s_irq_n;
uint64_t s_irq_seen;

void irq_track()
{
    if (g_xirq_taken != s_irq_seen) {    /* the first instruction of a handler taken since */
        s_irq_seen = g_xirq_taken;
        if (s_irq_n < MAX_IRQ) s_irq_base[s_irq_n++] = g_cpu.A[7];
    }
    /* (A7 - 1: an A7 of 0 is the top of the stack, as the engine's g_xa7min takes it) */
    while (s_irq_n && g_cpu.A[7] - 1u > s_irq_base[s_irq_n - 1] - 1u) s_irq_n--;
}

/* An access or instruction is c's own (its main program) unless a handler opened since c did. */
bool main_of(const Ctx &c) { return s_irq_n <= c.irq_level; }

bool is_wait_site(uint32_t pc)
{
    for (int i = 0; i < DK_WAIT_SITES; i++)
        if (dk_wait_sites[i].pc == pc) return true;
    return false;
}

/* A timed run that took an interrupt is doomed (compared deferred instead); one that also reached
   a wait site is not compared at all, so it stops logging (its log and coverage are no use). */
bool live(Ctx &c)
{
    if (!c.doomed && c.mode == 1 && g_xirq_taken != c.irq0) c.doomed = true;
    if (!(c.doomed && c.waited)) return true;
    if (c.logA.capacity()) std::vector<Hw>().swap(c.logA);
    if (c.cov.capacity()) std::vector<uint32_t>().swap(c.cov);
    return false;
}

void apply_pokes(Ctx &t)
{
    if (!t.pokes) return;
    for (const Poke &p : *t.pokes) {
        if (p.after != t.nreads) continue;
        for (int i = 0; i < p.size; i++)
            g_ram[(p.addr + (uint32_t)i) & 0xFFFFu] = (uint8_t)(p.value >> (8 * (p.size - 1 - i)));
    }
}

int hw_hook(uint32_t a, int size, int kind, uint16_t v, uint16_t *out)
{
    if (!s_depth) return 0;
    Ctx &t = *s_ctx[s_depth - 1];
    if (t.mode == 1) {
        for (int d = 0; d < s_depth; d++) {
            if (s_ctx[d]->mode != 1 || !live(*s_ctx[d]) || !main_of(*s_ctx[d])) continue;
            if (kind == 1) s_ctx[d]->logA.push_back({ a, size, 0, v });
            else if (kind == 2) s_ctx[d]->logA.push_back({ a, size, 1, v });
        }
        return 0;
    }
    if (t.mode == 2) {
        if (kind == 0) {
            uint16_t r = 0;
            while (t.readpos < t.logA.size() && t.logA[t.readpos].write) t.readpos++;
            if (t.readpos < t.logA.size()) r = t.logA[t.readpos++].v;
            *out = r;
            t.logB.push_back({ a, size, 0, r });
            t.nreads++;
            apply_pokes(t);
            return 1;
        }
        if (kind == 2) { t.logB.push_back({ a, size, 1, v }); return 1; }
        return 0;
    }
    if (t.mode == 3) {
        if (kind == 0) {
            uint16_t r = t.supply && t.supply_pos < t.supply->size() ? (*t.supply)[t.supply_pos++] : 0;
            *out = r;
            t.logA.push_back({ a, size, 0, r });
            t.nreads++;
            apply_pokes(t);
            return 1;
        }
        if (kind == 2) { t.logA.push_back({ a, size, 1, v }); return 1; }
    }
    return 0;
}

/* Coverage goes to the innermost run and, while it is a timed one, to the timed runs it is nested
   in (their main program); never from a C run or a shadow run (mode 2/3) to the tests below it. */
void insn_hook(uint32_t pc)
{
    if (!s_depth) return;
    Ctx &top = *s_ctx[s_depth - 1];
    if (s_diff) {
        for (int d = 0; d < s_depth; d++)
            if (g_cpu.A[7] - 1u < s_ctx[d]->a7lo - 1u) s_ctx[d]->a7lo = g_cpu.A[7];
        if (top.path.size() < 100000) top.path.push_back(pc);
    }
    if (top.mode == 3) {
        if (pc >= top.fn->addr && pc < top.fn->end) top.cov.push_back(pc);
        if (top.budget && ++top.ninsn > top.budget)
            dk::fatal("verify: the deferred run of $%06X ran %llu instructions (its timed run's x16 + 1M) "
                      "without returning, at $%06X: a wait for an interrupt no [recomp.sync] wait_vint site names?",
                      (unsigned)top.fn->addr, (unsigned long long)top.ninsn, (unsigned)pc);
        return;
    }
    if (top.mode != 1) return;
    irq_track();
    bool wait = is_wait_site(pc);
    for (int d = 0; d < s_depth; d++) {
        Ctx &c = *s_ctx[d];
        if (!live(c) || !main_of(c)) continue;
        if (wait) c.waited = true;
        c.ninsn++;
        if (pc >= c.fn->addr && pc < c.fn->end) c.cov.push_back(pc);
    }
}

std::unordered_set<uint32_t> s_only;
std::unordered_map<uint32_t, size_t> s_insns;   /* each function's instruction count */

/* DKIT_VERIFY_TARGETS (kit 0.9.10): one line per member run of this process, in the order it plays
   them (its own run or the carrier first, then the branch plan's lines): `TAG AT PC...` (hex
   instruction starts the reference executed in that run; `-` = no map: the run falls back to
   s_insns). Batman & Robin, kit 0.9.9: after an entry's 2000th call no sampled-out call was a probe
   (the instruction counts are per function file, so an interior entry never had one), and the
   stage-2 arena's tilt paths in Stage2_UpdateScroll, which the reference ran, were never compared;
   probing every call instead cost the movie +18% (an entry whose range never completes probes for
   ever). The targets are what the reference ran: a file stops being probed once every one of them
   is covered, and only a path no passing call reaches keeps it probed. */
struct RunTarget { std::string tag; uint32_t at; bool have; std::vector<uint32_t> pcs; };
std::vector<RunTarget> s_targets;
size_t s_run, s_next = 1;   /* the member run playing; the next branch of the plan */
bool s_targeted;            /* the run playing has a target */
/* per function file (keyed by its end, which its entries share): the targets of the run playing
   that no passing call or case of the file covered yet */
std::unordered_map<uint32_t, std::set<uint32_t>> s_rem;
std::vector<const DkVerifyFn *> s_by_addr;   /* the functions under test, by address */

const DkVerifyFn *file_of(uint32_t pc)
{
    auto it = std::upper_bound(s_by_addr.begin(), s_by_addr.end(), pc,
                               [](uint32_t a, const DkVerifyFn *f) { return a < f->addr; });
    if (it == s_by_addr.begin()) return nullptr;
    const DkVerifyFn *f = *(it - 1);
    return pc < f->end ? f : nullptr;
}

void load_targets()
{
    const char *tp = getenv("DKIT_VERIFY_TARGETS");
    FILE *f = tp ? fopen(tp, "r") : nullptr;
    if (!f) return;
    std::string line;
    for (int ch;;) {
        ch = fgetc(f);
        if (ch != EOF && ch != '\n') { line += (char)ch; continue; }
        if (!line.empty() && line.back() == '\r') line.pop_back();
        if (!line.empty()) {
            RunTarget t;
            char tag[256];
            unsigned at;
            int used = 0;
            if (sscanf(line.c_str(), "%255s %u %n", tag, &at, &used) < 2) {
                fprintf(stderr, "[verify] %s: bad line\n", tp);
                exit(2);
            }
            t.tag = tag;
            t.at = at;
            const char *q = line.c_str() + used;
            t.have = *q && *q != '-';
            while (t.have && *q) {
                char *end;
                unsigned long v = strtoul(q, &end, 16);
                if (end == q) break;
                t.pcs.push_back((uint32_t)v);
                q = end;
                while (*q == ' ') q++;
            }
            s_targets.push_back(std::move(t));
        }
        line.clear();
        if (ch == EOF) break;
    }
    fclose(f);
}

/* the targets of the run playing, less what the testing state (restored at a branch) covered */
void rebuild_rem()
{
    s_rem.clear();
    s_targeted = s_run < s_targets.size() && s_targets[s_run].have;
    if (!s_targeted) return;
    for (uint32_t pc : s_targets[s_run].pcs)
        if (const DkVerifyFn *f = file_of(pc)) s_rem[f->end].insert(pc);
    for (int i = 0; i < dk_verify_fn_count; i++) {
        auto r = s_rem.find(dk_verify_fns[i].end);
        if (r == s_rem.end()) continue;
        for (uint32_t pc : s_stat_store[i].covered) r->second.erase(pc);
    }
}

/* a passing call or case of fn executed these */
template <class It> void cover(Stat &st, const DkVerifyFn *fn, It b, It e)
{
    auto r = s_targeted ? s_rem.find(fn->end) : s_rem.end();
    for (; b != e; ++b) {
        st.covered.insert(*b);
        if (r != s_rem.end()) r->second.erase(*b);
    }
}

/* fn's file still has a target at or after fn (an entry's runs record [fn, file end)) */
bool rem_from(const DkVerifyFn *fn)
{
    auto r = s_rem.find(fn->end);
    return r != s_rem.end() && r->second.lower_bound(fn->addr) != r->second.end();
}

bool rem_has(const DkVerifyFn *fn, uint32_t pc)
{
    auto r = s_rem.find(fn->end);
    return r != s_rem.end() && r->second.count(pc);
}

void load_only()
{
    if (const char *e = getenv("DKIT_VERIFY_ONLY")) {
        for (const char *s = e; *s;) {
            char *end;
            unsigned long v = strtoul(s, &end, 16);
            if (end == s) break;
            s_only.insert((uint32_t)v);
            s = *end ? end + 1 : end;
        }
    }
    if (const char *d = getenv("DKIT_VERIFY_DEV_STOP")) s_dev_stop = 1u + strtoull(d, nullptr, 0);
    if (const char *d = getenv("DKIT_VERIFY_DIFF")) s_diff = *d && *d != '0';
    load_targets();
    /* DKIT_VERIFY_INSNS=<path>: lines "ADDR COUNT" (hex address, decimal count of the function's
       instructions): sampled-out calls stop being probes once its checked calls covered that many */
    const char *ip = getenv("DKIT_VERIFY_INSNS");
    FILE *f = ip ? fopen(ip, "r") : nullptr;
    if (!f) return;
    unsigned a, n;
    while (fscanf(f, "%x %u", &a, &n) == 2) s_insns[a] = n;
    fclose(f);
}

/* <ADDR>.syn: '#' comments; `at any` (run at the first top-level call of any function, for
   functions the corpus never runs; default: at F's own first top-level call); `at $ADDR` (at
   the first call of function ADDR, which must have C: a state where F's callers run, for a
   function the corpus never calls whose inputs need the game's structures set up; kit 0.9.6);
   `case NAME` starts
   a case; in a case, `b|w|l ADDR = VALUE` (memory), `reg dN|aN = VALUE`, `sr = VALUE`, `hw V V ...`
   (hardware read values, in order), `poke N b|w|l ADDR = VALUE` (a RAM write once the run has
   made N hardware reads). ADDR and VALUE are sums/differences of terms: $hex, decimal, dN, aN
   (register values at entry), [EXPR] (the long at EXPR), {EXPR} (the word at EXPR, sign-extended
   like movea.w: a handle; kit 0.9.6). */
void load_synth()
{
    const char *dir = getenv("DKIT_VERIFY_SYNTH");
    if (!dir) return;
    for (int i = 0; i < dk_verify_fn_count; i++) {
        char path[1024];
        snprintf(path, sizeof path, "%s/%06X.syn", dir, (unsigned)dk_verify_fns[i].addr);
        FILE *f = fopen(path, "r");
        if (!f) continue;
        SynSpec spec;
        char line[512];
        while (fgets(line, sizeof line, f)) {
            std::string l = line;
            size_t h = l.find('#');
            if (h != std::string::npos) l.resize(h);
            while (!l.empty() && (l.back() == '\n' || l.back() == '\r' || l.back() == ' ' || l.back() == '\t')) l.pop_back();
            size_t b = l.find_first_not_of(" \t");
            if (b == std::string::npos) continue;
            l = l.substr(b);
            if (l == "at any") { spec.at_any = true; continue; }
            if (l.rfind("at $", 0) == 0) { spec.at = (uint32_t)strtoul(l.c_str() + 4, nullptr, 16); continue; }
            if (l.rfind("case", 0) == 0) { spec.cases.push_back({ l.size() > 5 ? l.substr(5) : "", {}, {}, {} }); continue; }
            if (spec.cases.empty()) spec.cases.push_back({ "", {}, {}, {} });
            SynCase &c = spec.cases.back();
            if (l.rfind("hw ", 0) == 0) {
                const char *s = l.c_str() + 3;
                while (*s) {
                    while (*s == ' ' || *s == '\t') s++;
                    if (!*s) break;
                    char *e;
                    unsigned long v = *s == '$' ? strtoul(s + 1, &e, 16) : strtoul(s, &e, 0);
                    c.reads.push_back((uint16_t)v);
                    s = e;
                }
                continue;
            }
            size_t after = 0;
            bool poke = l.rfind("poke ", 0) == 0;
            if (poke) {
                char *e;
                after = strtoul(l.c_str() + 5, &e, 0);
                l = e;
                size_t nb = l.find_first_not_of(" \t");
                if (nb == std::string::npos) continue;
                l = l.substr(nb);
            }
            size_t eq = l.find('=');
            if (eq == std::string::npos) continue;
            std::string lhs = l.substr(0, eq), rhs = l.substr(eq + 1);
            while (!lhs.empty() && lhs.back() == ' ') lhs.pop_back();
            Assign a{};
            a.value = rhs;
            if (lhs == "sr") { a.what = 2; }
            else if (lhs.rfind("reg ", 0) == 0) { a.what = 1; a.addr = lhs.substr(4); }
            else { a.what = 0; a.size = lhs[0] == 'b' ? 1 : lhs[0] == 'w' ? 2 : 4; a.addr = lhs.substr(2); }
            if (poke) {
                if (a.what == 0) c.pokes.push_back({ after, a });
                continue;
            }
            c.sets.push_back(a);
        }
        fclose(f);
        if (spec.at) s_at.insert(spec.at);
        s_synth[dk_verify_fns[i].addr] = spec;
    }
}

uint32_t eval(const char *&s);
uint32_t term(const char *&s)
{
    while (*s == ' ') s++;
    if (*s == '[') { s++; uint32_t a = eval(s); while (*s == ' ') s++; if (*s == ']') s++; return m68k_read32(a); }
    if (*s == '{') {
        s++;
        uint32_t a = eval(s);
        while (*s == ' ') s++;
        if (*s == '}') s++;
        return (uint32_t)(int32_t)(int16_t)m68k_read16(a);
    }
    if (*s == '$') { char *e; uint32_t v = (uint32_t)strtoul(s + 1, &e, 16); s = e; return v; }
    if ((s[0] == 'd' || s[0] == 'a') && s[1] >= '0' && s[1] <= '7') { uint32_t v = s[0] == 'd' ? g_cpu.D[s[1] - '0'] : g_cpu.A[s[1] - '0']; s += 2; return v; }
    char *e; long v = strtol(s, &e, 0); s = e; return (uint32_t)v;
}
uint32_t eval(const char *&s)
{
    while (*s == ' ') s++;
    bool neg = false;
    if (*s == '-') { neg = true; s++; }
    uint32_t v = term(s);
    if (neg) v = 0u - v;
    for (;;) {
        while (*s == ' ') s++;
        if (*s == '+') { s++; v += term(s); }
        else if (*s == '-') { s++; v -= term(s); }
        else break;
    }
    return v;
}
uint32_t eval_str(const std::string &x) { const char *s = x.c_str(); return eval(s); }

std::string json_str(const std::string &s)
{
    std::string o = "\"";
    for (char c : s) {
        if (c == '"' || c == '\\') o += '\\';
        o += (unsigned char)c < 0x20 ? ' ' : c;
    }
    return o + "\"";
}

std::string stopped_early()
{
    char b[400];
    if (dk::g_fatal_msg[0]) return std::string("fatal error: ") + dk::g_fatal_msg;
    for (int d = 0; d < s_depth; d++) {
        const Ctx &c = *s_ctx[d];
        if (c.mode == 1) continue;
        snprintf(b, sizeof b, "the run ended inside the %s of $%06X opened at frame %llu (untimed: it never "
                              "returned), so nothing was tested after it",
                 c.mode == 2 ? "C run" : "synthetic case", (unsigned)c.fn->addr, (unsigned long long)c.frame0);
        return b;
    }
    if (s_refused_since && g_frame_count + 1 >= s_refused_since + STOP_FRAMES) {
        snprintf(b, sizeof b, "no test could open from frame %llu to the end (%llu frames%s; %d tests open, the "
                              "innermost $%06X mode %d)", (unsigned long long)(s_refused_since - 1),
                 (unsigned long long)(g_frame_count + 1 - s_refused_since),
                 s_dev_stop ? ", DKIT_VERIFY_DEV_STOP" : "", s_depth,
                 s_depth ? (unsigned)s_ctx[s_depth - 1]->fn->addr : 0u, s_depth ? s_ctx[s_depth - 1]->mode : 0);
        return b;
    }
    return "";
}

void write_report(const char *path)
{
    FILE *f = fopen(path, "w");
    if (!f) return;
    uint64_t tested = 0, checked = 0;
    for (int i = 0; i < dk_verify_fn_count; i++) {
        tested += s_stat_store[i].checked + s_stat_store[i].mismatches > 0;
        checked += s_stat_store[i].checked;
    }
    std::string stop = stopped_early();
    if (!stop.empty()) fprintf(stderr, "[verify] testing stopped early: %s\n", stop.c_str());
    fprintf(f, "{\"stopped_early\": %s, \"frames\": %llu, \"functions_tested\": %llu, \"calls_checked\": %llu, "
               "\"calls_refused\": %llu,\n\"functions\": {", stop.empty() ? "null" : json_str(stop).c_str(),
            (unsigned long long)g_frame_count, (unsigned long long)tested, (unsigned long long)checked,
            (unsigned long long)s_refused);
    for (int i = 0; i < dk_verify_fn_count; i++) {
        const Stat &st = s_stat_store[i];
        fprintf(f, "%s\n\"%06X\": {\"calls_checked\": %llu, \"mismatches\": %llu, \"irq_skipped\": %llu, "
                   "\"irq_deferred\": %llu, \"irq_waits\": %llu, \"synthetic_checked\": %llu, \"too_deep\": %llu, "
                   "\"probes\": %llu, \"first_mismatch\": %s, \"covered\": [",
                i ? "," : "", (unsigned)dk_verify_fns[i].addr, (unsigned long long)st.checked,
                (unsigned long long)st.mismatches, (unsigned long long)st.irq, (unsigned long long)st.deferred,
                (unsigned long long)st.waits, (unsigned long long)st.synth, (unsigned long long)st.deep,
                (unsigned long long)st.probes,
                st.first.empty() ? "null" : json_str(st.first).c_str());
        std::vector<uint32_t> cov(st.covered.begin(), st.covered.end());
        std::sort(cov.begin(), cov.end());
        for (size_t k = 0; k < cov.size(); k++) fprintf(f, "%s%u", k ? "," : "", (unsigned)cov[k]);
        fprintf(f, "]}");
    }
    fprintf(f, "\n}}\n");
    fclose(f);
}

void report()
{
    if (const char *path = getenv("DKIT_VERIFY_OUT")) write_report(path);
}

struct Kept {
    std::vector<Stat> stats;
    std::vector<Ctx> ctx;
    int depth;
    uint32_t entering;
    bool any_done;
    uint64_t refused, refused_since;
    uint32_t irq_base[MAX_IRQ];
    int irq_n;
    uint64_t irq_seen;
};
Kept *s_kept;

std::string describe(const char *what, uint32_t a, uint32_t g, uint32_t c, const std::string &call)
{
    char b[200];
    snprintf(b, sizeof b, "%s: %s %06X: generated %08X, C %08X", call.c_str(), what, (unsigned)a, (unsigned)g,
             (unsigned)c);
    return b;
}

/* The first index in [from, to) where a and b differ, else `to`. */
uint32_t first_diff(const uint8_t *a, const uint8_t *b, uint32_t from, uint32_t to)
{
    uint32_t i = from;
    for (; i < to && (i & 7u); i++)
        if (a[i] != b[i]) return i;
    for (; i + 8u <= to; i += 8u) {
        uint64_t x, y;
        memcpy(&x, a + i, 8);
        memcpy(&y, b + i, 8);
        if (x != y) break;
    }
    for (; i < to; i++)
        if (a[i] != b[i]) return i;
    return to;
}

/* Step 4: the C run's result (current state) against the generated run's (r1, c.post). */
std::string compare(const Ctx &c, const DkVerifyFn *fn, const M68KState &r1, uint32_t xrts1, const std::string &call)
{
    std::string bad;
    M68KState rc = g_cpu;
    /* dead stack: below the returned A7. A return that empties the stack leaves A7 at the top of
       RAM + 1 ($00000000 once it wraps: the reset SSP is 0, so a call at depth 1 returns to it):
       the dead stack is then everything from stack_low to the top, $10000 (kit <= 0.9.4 took 0 and
       masked nothing, comparing the return slots the function's own calls left) */
    uint32_t lo = dk_stack_low() & 0xFFFFu, hi = r1.A[7] & 0xFFFFu;
    if ((r1.A[7] & 0xFFFFFFu) == 0) hi = 0x10000u;
    if ((dk_stack_low() & 0xFF0000u) != 0xFF0000u || hi < lo) hi = lo;
    const uint8_t *dc = dk_dont_care_map();   /* [recomp] dont_care: bytes nothing reads */
    /* the first differing byte outside the dead stack and dont_care, in address order: 8 bytes at
       a time to a difference, then that byte against the masks (kit <= 0.9.9 tested every byte
       against both masks: ~100 us a call, half of a verify process's time) */
    for (uint32_t i = 0; i < 0x10000u && bad.empty();) {
        if (i >= lo && i < hi) { i = hi; continue; }
        uint32_t to = i < lo ? lo : 0x10000u;   /* (i < lo: below the dead stack; else above it) */
        uint32_t j = first_diff(g_ram, c.post, i, to);
        if (j == to) { i = to; continue; }
        if (!dc[j]) bad = describe("RAM", 0xFF0000u + j, c.post[j], g_ram[j], call);
        i = j + 1;
    }
    if (bad.empty() && rc.A[7] != r1.A[7]) bad = describe("A7", 7, r1.A[7], rc.A[7], call);
    if (bad.empty() && g_xrts != xrts1) bad = describe("return to", 0, xrts1, g_xrts, call);
    /* registers: an output as wide as its C type, a clobber not at all, any other in full (hand
       asm has no callee-saved set: a register F leaves alone may hold its caller's value) */
    for (int r = 0; r < 15 && bad.empty(); r++) {
        if (fn->skip_mask >> r & 1) continue;
        uint32_t m = 0xFFFFFFFFu;
        if (fn->out_mask >> r & 1) {
            int b = fn->out_bytes[r];
            m = r >= 8 || b >= 4 ? 0xFFFFFFFFu : b == 2 ? 0xFFFFu : 0xFFu;
        }
        uint32_t g = r < 8 ? r1.D[r] : r1.A[r - 8], cc = r < 8 ? rc.D[r] : rc.A[r - 8];
        if ((g & m) != (cc & m)) bad = describe(r < 8 ? "d" : "a", (uint32_t)(r & 7), g & m, cc & m, call);
    }
    uint16_t srm = (uint16_t)(0xFF00u | fn->flag_mask);
    if (bad.empty() && (r1.SR & srm) != (rc.SR & srm)) bad = describe("SR", 0, r1.SR & srm, rc.SR & srm, call);
    if (bad.empty() && !(c.logA == c.logB)) {
        size_t k = 0;
        while (k < c.logA.size() && k < c.logB.size() && c.logA[k] == c.logB[k]) k++;
        char b[240];
        if (k < c.logA.size() && k < c.logB.size())
            snprintf(b, sizeof b, "%s: hardware access %zu: generated %s $%06X=%04X, C %s $%06X=%04X", call.c_str(), k,
                     c.logA[k].write ? "write" : "read", (unsigned)c.logA[k].addr, c.logA[k].v,
                     c.logB[k].write ? "write" : "read", (unsigned)c.logB[k].addr, c.logB[k].v);
        else
            snprintf(b, sizeof b, "%s: %zu hardware accesses generated, %zu in C", call.c_str(), c.logA.size(),
                     c.logB.size());
        bad = b;
    }
    if (!bad.empty() && s_diff) {
        fprintf(stderr, "[verify] diff %s\n  A7 generated $%08X C $%08X; lowest generated $%08X C $%08X; "
                        "dead stack [$%04X,$%04X)\n", bad.c_str(), (unsigned)r1.A[7], (unsigned)rc.A[7],
                (unsigned)c.a7lo_gen, (unsigned)c.a7lo, (unsigned)lo, (unsigned)hi);
        for (int k = 0; k < 2; k++) {
            const M68KState &s = k ? rc : r1;
            fprintf(stderr, "  %s:", k ? "C        " : "generated");
            for (int r = 0; r < 8; r++) fprintf(stderr, " d%d=%08X", r, (unsigned)s.D[r]);
            for (int r = 0; r < 8; r++) fprintf(stderr, " a%d=%08X", r, (unsigned)s.A[r]);
            fprintf(stderr, " sr=%04X\n", (unsigned)s.SR);
        }
        int shown = 0;
        for (uint32_t i = 0; i < 0x10000u && shown < 64; i++)
            if (g_ram[i] != c.post[i]) {
                fprintf(stderr, "  RAM $FF%04X: before %02X, generated %02X, C %02X%s\n", (unsigned)i, c.pre[i],
                        c.post[i], g_ram[i], (i >= lo && i < hi) || dc[i] ? " (masked)" : "");
                shown++;
            }
        /* the instructions each run executed (the C run's: the generated code it called) */
        for (int k = 0; k < 2; k++) {
            const std::vector<uint32_t> &p = k ? c.path : c.path_gen;
            fprintf(stderr, "  path %s (%zu):", k ? "C        " : "generated", p.size());
            for (size_t i = 0; i < p.size() && i < 400; i++) fprintf(stderr, " %06X", (unsigned)p[i]);
            fprintf(stderr, "\n");
        }
        fflush(stderr);
    }
    return bad;
}

Ctx *push(const DkVerifyFn *fn, int mode)
{
    Ctx *c = s_ctx[s_depth];
    c->fn = fn;
    c->mode = mode;
    c->logA.clear();
    c->logB.clear();
    c->cov.clear();
    c->readpos = 0;
    c->supply = nullptr;
    c->supply_pos = 0;
    c->pokes = nullptr;
    c->nreads = 0;
    c->irq0 = g_xirq_taken;
    c->frame0 = g_frame_count;
    c->doomed = false;
    c->waited = false;
    c->ninsn = 0;
    c->budget = 0;
    c->a7lo = c->a7lo_gen = 0xFFFFFFFFu;
    c->path.clear();
    c->path_gen.clear();
    if (!s_depth) { s_irq_n = 0; s_irq_seen = g_xirq_taken; }
    c->irq_level = s_irq_n;
    s_depth++;
    g_xhw_hook = hw_hook;
    g_xinsn_hook = insn_hook;
    return c;
}

void pop()
{
    s_depth--;
    if (!s_depth) {
        g_xhw_hook = nullptr;
        g_xinsn_hook = nullptr;
    }
    if (!(s_depth && (s_ctx[s_depth - 1]->mode != 1 || s_depth >= MAX_DEPTH))
        && !(s_dev_stop && g_frame_count + 1 >= s_dev_stop))
        s_refused_since = 0;
}

/* The run's await: the function's own return address (on top of the stack at its entry), as its
   caller's call site waits for it. Waiting for ANY return (kit <= 0.9.1) ended the run at an RTS
   inside it that no site waits for (threaded code's `pea next(pc)` + a helper's rts; `move.l
   (a1),-(sp) / rts`, a jump through a pushed address), where the 68000 runs on: the comparison
   then saw the function's live return slot (RET_FROM_C in the C run). A return further out (to an
   address an enclosing site waits for) still ends the run. */
static uint32_t own_return(void) { return m68k_read32(g_cpu.A[7]) & 0xFFFFFFu; }

void run_gen(const DkVerifyFn *fn)
{
    int trace0 = g_xtrace;
    g_xtrace = 1;
    s_entering = fn->addr;
    /* its XENTRY sees s_entering and lets the body run; in a dispatch frame of its own, so that a
       jmp to another function (a pending tail call) runs before the comparison. Its RTS ends the
       run wherever it returns (the comparison checks where) */
#ifdef XAWAIT_ANY
    XCALL_BEGIN(own_return());
    recomp_call_func(fn->gen);
    XCALL_END();
#else
    recomp_call_func(fn->gen);
#endif
    s_entering = 0;
    g_xtrace = trace0;
}

/* The C run, the same way: in a dispatch frame of its own, so that a C function that leaves by a
   jump (dk_flow.h: a script op's NEXT) continues into the generated code it jumps to, as the
   generated run did, until the first return out of the frame; the comparison is at that return. */
void run_c(const DkVerifyFn *fn)
{
    int trace0 = g_xtrace;
    if (s_diff) g_xtrace = 1;   /* the generated code the C calls: its instructions for the diff */
#ifdef XAWAIT_ANY
    XCALL_BEGIN(own_return());
    recomp_call_func(fn->slot);
    XCALL_END();
#else
    recomp_call_func(fn->slot);
#endif
    g_xtrace = trace0;
}

void run_case(const DkVerifyFn *fn, Stat &st, const SynCase &k)
{
    static uint8_t base[0x10000];
    memcpy(base, g_ram, 0x10000);
    M68KState r00 = g_cpu;
    uint32_t xrts00 = g_xrts, clk = g_xclk, start = g_xstart, end = g_xend;
    uint64_t irq0 = g_xirq_taken;
    std::vector<std::pair<uint32_t, uint8_t>> rom_saved;
    for (const Assign &a : k.sets) {
        uint32_t v = eval_str(a.value);
        if (a.what == 2) { g_cpu.SR = (uint16_t)v; continue; }
        if (a.what == 1) {
            int r = a.addr[1] - '0';
            if (a.addr[0] == 'd') g_cpu.D[r] = v; else g_cpu.A[r] = v;
            continue;
        }
        uint32_t at = eval_str(a.addr) & 0xFFFFFFu;
        if (at + (uint32_t)a.size <= 0x400000u) {
            for (int i = 0; i < a.size; i++) {
                rom_saved.push_back({ at + (uint32_t)i, g_rom[at + i] });
                g_rom[at + i] = (uint8_t)(v >> (8 * (a.size - 1 - i)));
            }
            continue;
        }
        if (a.size == 1) m68k_write8(at, (uint8_t)v);
        else if (a.size == 2) m68k_write16(at, (uint16_t)v);
        else m68k_write32(at, v);
    }
    std::vector<Poke> pokes;
    for (const auto &pk : k.pokes) {
        uint32_t at = eval_str(pk.second.addr) & 0xFFFFFFu;
        if (at >= 0xFF0000u) pokes.push_back({ pk.first, at, pk.second.size, eval_str(pk.second.value) });
    }
    g_xend = 0xFFFFFFFFu;   /* untimed: no slice, no interrupt */
    Ctx &c = *push(fn, 3);
    memcpy(c.pre, g_ram, 0x10000);
    M68KState r0 = g_cpu;
    uint32_t xrts0 = g_xrts;
    c.supply = &k.reads;
    c.pokes = &pokes;
    g_xirq_hold = 1;
    run_gen(fn);
    c.a7lo_gen = c.a7lo;
    c.a7lo = 0xFFFFFFFFu;
    c.path_gen.swap(c.path);
    g_xirq_hold = 0;
    M68KState r1 = g_cpu;
    uint32_t xrts1 = g_xrts;
    memcpy(c.post, g_ram, 0x10000);
    if (const char *tc = getenv("DKIT_VERIFY_TRACE_CASE"); tc && k.name == tc) {
        fprintf(stderr, "[verify] case %s path:", k.name.c_str());
        for (uint32_t pc : c.cov) fprintf(stderr, " %06X", (unsigned)pc);
        fprintf(stderr, "\n");
    }
    memcpy(g_ram, c.pre, 0x10000);
    g_cpu = r0;
    g_xrts = xrts0;
    c.mode = 2;
    c.readpos = 0;
    c.nreads = 0;
    g_xirq_hold = 1;
    run_c(fn);
    g_xirq_hold = 0;
    std::string bad = compare(c, fn, r1, xrts1, "synthetic case " + (k.name.empty() ? std::string("?") : k.name));
    if (g_xirq_taken != irq0) bad = "synthetic case " + k.name + ": an interrupt was taken";
    if (bad.empty()) {
        st.checked++;
        st.synth++;
        cover(st, fn, c.cov.begin(), c.cov.end());
    } else {
        st.mismatches++;
        if (st.first.empty()) st.first = bad;
    }
    pop();
    for (size_t i = rom_saved.size(); i-- > 0;) g_rom[rom_saved[i].first] = rom_saved[i].second;
    memcpy(g_ram, base, 0x10000);
    g_cpu = r00;
    g_xrts = xrts00;
    g_xclk = clk;
    g_xstart = start;
    g_xend = end;
}

/* A call an interrupt landed in (c: its timed run, finished; the state is its result): the
   generated code and the C again from the call's entry state, both untimed with interrupts held,
   the generated run's hardware reads supplied from the timed run's main-program reads (in order;
   0 past them), the C's replayed from the generated run's. Returns the comparison ("" = equal)
   and, in `cov`, the instructions of F the deferred run executed. The timed result is put back. */
std::string run_deferred(Ctx &c, const DkVerifyFn *fn, const M68KState &r0, uint32_t xrts0, const std::string &call,
                         std::vector<uint32_t> &cov)
{
    static uint8_t timed[0x10000];
    memcpy(timed, g_ram, 0x10000);
    M68KState r1 = g_cpu;
    uint32_t xrts1 = g_xrts, clk = g_xclk, start = g_xstart, end = g_xend;
    uint64_t irq0 = g_xirq_taken;
    std::vector<uint16_t> reads;
    for (const Hw &h : c.logA)
        if (!h.write) reads.push_back(h.v);
    Ctx &d = *push(fn, 3);
    memcpy(d.pre, c.pre, 0x10000);
    memcpy(g_ram, c.pre, 0x10000);
    g_cpu = r0;
    g_xrts = xrts0;
    d.supply = &reads;
    d.budget = 16 * c.ninsn + 1000000;
    g_xend = 0xFFFFFFFFu;   /* untimed: no slice, no interrupt */
    g_xirq_hold = 1;
    run_gen(fn);
    d.a7lo_gen = d.a7lo;
    d.a7lo = 0xFFFFFFFFu;
    d.path_gen.swap(d.path);
    g_xirq_hold = 0;
    M68KState d1 = g_cpu;
    uint32_t xrts_d1 = g_xrts;
    memcpy(d.post, g_ram, 0x10000);
    memcpy(g_ram, c.pre, 0x10000);
    g_cpu = r0;
    g_xrts = xrts0;
    d.mode = 2;
    d.readpos = 0;
    d.nreads = 0;
    g_xirq_hold = 1;
    run_c(fn);
    g_xirq_hold = 0;
    std::string bad = compare(d, fn, d1, xrts_d1, call + " (interrupt deferred)");
    if (bad.empty() && g_xirq_taken != irq0) bad = call + " (interrupt deferred): an interrupt was taken";
    cov.swap(d.cov);
    pop();
    memcpy(g_ram, timed, 0x10000);
    g_cpu = r1;
    g_xrts = xrts1;
    g_xclk = clk;
    g_xstart = start;
    g_xend = end;
    return bad;
}

void run_synth(uint32_t addr, bool self)
{
    for (auto &kv : s_synth) {
        const SynSpec &sp = kv.second;
        if (self ? (sp.at ? sp.at != addr : kv.first != addr) : !sp.at_any) continue;
        const DkVerifyFn *fn = s_fns[kv.first];
        Stat &st = stat_of(fn);
        if (st.synth_done) continue;
        st.synth_done = true;
        if (!s_only.empty() && !s_only.count(kv.first)) continue;
        for (const SynCase &k : kv.second.cases) run_case(fn, st, k);
    }
}

}  /* namespace */

/* A host exception (a C function dereferencing what the 68000 reads harmlessly): name the tests
   open and the C function running before the runner dies (crash_win.cpp). */
static void crash_report(unsigned long code, void *where)
{
    fprintf(stderr, "[verify] host exception %08lX at %p; tests open (outermost first):", code, where);
    for (int d = 0; d < s_depth; d++)
        fprintf(stderr, " $%06X (mode %d)", (unsigned)s_ctx[d]->fn->addr, s_ctx[d]->mode);
    fprintf(stderr, "; C function running $%06X\n", (unsigned)dk::g_fn_current);
    fflush(stderr);
}

extern "C" void dk_verify_branch_save(void)
{
    if (!s_kept) s_kept = new Kept();
    Kept &k = *s_kept;
    k.stats.assign(s_stat_store, s_stat_store + dk_verify_fn_count);
    k.ctx.clear();
    for (int d = 0; d < s_depth; d++) k.ctx.push_back(*s_ctx[d]);
    k.depth = s_depth;
    k.entering = s_entering;
    k.any_done = s_any_done;
    k.refused = s_refused;
    k.refused_since = s_refused_since;
    memcpy(k.irq_base, s_irq_base, sizeof s_irq_base);
    k.irq_n = s_irq_n;
    k.irq_seen = s_irq_seen;
    if (!s_targets.empty()) {   /* a branch starts: the next line of the plan */
        s_run = s_next++;
        rebuild_rem();
    }
}

extern "C" void dk_verify_branch_restore(void)
{
    const Kept &k = *s_kept;
    for (int i = 0; i < dk_verify_fn_count; i++) s_stat_store[i] = k.stats[(size_t)i];
    for (int d = 0; d < k.depth; d++) *s_ctx[d] = k.ctx[(size_t)d];
    s_depth = k.depth;
    s_entering = k.entering;
    s_any_done = k.any_done;
    s_refused = k.refused;
    s_refused_since = k.refused_since;
    memcpy(s_irq_base, k.irq_base, sizeof s_irq_base);
    s_irq_n = k.irq_n;
    s_irq_seen = k.irq_seen;
    g_xhw_hook = s_depth ? hw_hook : nullptr;
    g_xinsn_hook = s_depth ? insn_hook : nullptr;
    if (!s_targets.empty()) {   /* the next branch of the plan from the same snapshot, or the carrier */
        size_t ended = s_run;
        bool same = ended && ended < s_targets.size() && s_next < s_targets.size()
                    && s_targets[s_next].at == s_targets[ended].at;
        s_run = same ? s_next++ : 0;
        rebuild_rem();
    }
}

/* A branch's results: <dir of DKIT_VERIFY_OUT>/<tag>.json */
extern "C" void dk_verify_branch_end(const char *tag)
{
    const char *out = getenv("DKIT_VERIFY_OUT");
    if (!out) return;
    if (!s_targets.empty() && (s_run >= s_targets.size() || s_targets[s_run].tag != tag)) {
        fprintf(stderr, "[verify] branch %s ended, but DKIT_VERIFY_TARGETS's line %zu is %s\n", tag, s_run + 1,
                s_run < s_targets.size() ? s_targets[s_run].tag.c_str() : "past its end");
        exit(2);
    }
    std::string dir(out);
    size_t slash = dir.find_last_of("/\\");
    dir = slash == std::string::npos ? std::string() : dir.substr(0, slash + 1);
    write_report((dir + tag + ".json").c_str());
}

static void verify_init(void)
{
    static bool init;
    if (init) return;
    init = true;
    if (dk_verify_fn_count > MAX_FNS) {
        fprintf(stderr, "[verify] %d functions with C, room for %d (verify.cpp MAX_FNS)\n", dk_verify_fn_count, MAX_FNS);
        exit(2);
    }
    for (int i = 0; i < dk_verify_fn_count; i++) s_fns[dk_verify_fns[i].addr] = &dk_verify_fns[i];
    for (int i = 0; i < dk_verify_fn_count; i++) s_by_addr.push_back(&dk_verify_fns[i]);
    std::sort(s_by_addr.begin(), s_by_addr.end(),
              [](const DkVerifyFn *a, const DkVerifyFn *b) { return a->addr < b->addr; });
    for (int d = 0; d <= MAX_DEPTH; d++) s_ctx[d] = &s_ctx_store[d];
    load_only();
    rebuild_rem();
    load_synth();
    atexit(report);
    dk_install_crash_report(crash_report);
}

extern "C" int dk_verify_entry(uint32_t addr)
{
    verify_init();
    if (s_entering == addr) { s_entering = 0; return 0; }   /* the generated run under test */
    auto it = s_fns.find(addr);
    bool outside = !s_only.empty() && !s_only.count(addr);   /* not under test: maybe an `at` trigger */
    if (it == s_fns.end() || (outside && !s_at.count(addr))) return 0;
    if ((s_depth && (s_ctx[s_depth - 1]->mode != 1 || s_depth >= MAX_DEPTH))
        || (s_dev_stop && g_frame_count + 1 >= s_dev_stop)) {
        s_refused++;
        if (!s_refused_since) s_refused_since = g_frame_count + 1;
        if (!outside && s_depth >= MAX_DEPTH && s_ctx[s_depth - 1]->mode == 1) stat_of(it->second).deep++;
        return 0;
    }
    s_refused_since = 0;
    const DkVerifyFn *fn = it->second;
    for (int d = 0; d < s_depth; d++)
        if (s_ctx[d]->fn == fn) return 0;   /* recursion: the outer test covers it */
    if (!s_depth && !s_any_done) { s_any_done = true; run_synth(addr, false); }
    run_synth(addr, true);
    if (outside) return 0;
    Stat &st = stat_of(fn);
    /* every call until 2000 have been checked, then one in 64; a call the sampling skips is still
       a probe while its file has targets left (DKIT_VERIFY_TARGETS; without one, while the checked
       calls haven't covered the file's instruction count, DKIT_VERIFY_INSNS): it is compared only if
       it reached one (the rare paths are what sampling misses) */
    bool probe = false;
    if (st.checked >= 2000 && (st.seen++ & 63)) {
        if (s_targeted) {
            if (!rem_from(fn)) return 0;
        } else {
            auto n = s_insns.find(addr);
            if (n == s_insns.end() || st.covered.size() >= n->second) return 0;
        }
        probe = true;
    }

    /* 1. the generated run */
    Ctx &c = *push(fn, 1);
    memcpy(c.pre, g_ram, 0x10000);
    M68KState r0 = g_cpu;
    uint32_t xrts0 = g_xrts;
    uint64_t irq0 = g_xirq_taken;
    run_gen(fn);
    c.a7lo_gen = c.a7lo;
    c.a7lo = 0xFFFFFFFFu;
    c.path_gen.swap(c.path);
    M68KState r1 = g_cpu;
    uint32_t xrts1 = g_xrts;
    bool irq = g_xirq_taken != irq0;
    if (probe) st.probes++;
    if (irq && c.waited) {     /* 2. it waited for the V-int: not comparable */
        if (!probe) { st.irq++; st.waits++; }
        pop();
        return 1;
    }
    /* 2b. the run went on past a runner frame's end in a run whose input script writes RAM (a
       scenario's `@poke`, kit 0.11.9): the write landed between the timed run's entry and its
       return, and the C run (from the entry state) never sees it, so the call is not comparable
       (R27's boss runs, 2026-11-18: 6 calls at their pokes' frames differed by the poked byte
       $F64A alone). Counted with the interrupt-skipped calls. */
    if (!irq && g_frame_count != c.frame0 && input_script_mutates_state()) {
        if (!probe) st.irq++;
        pop();
        return 1;
    }
    if (probe) {               /* (before the RAM copy: most probes stop here) */
        bool fresh = false;
        for (uint32_t pc : c.cov) {
            if (s_targeted ? !rem_has(fn, pc) : st.covered.count(pc) > 0) continue;
            auto t = irq ? st.tries.find(pc) : st.tries.end();
            if (t == st.tries.end() || t->second < DEFER_TRIES) { fresh = true; break; }
        }
        if (!fresh) { pop(); return 1; }
    }
    char call[48];
    snprintf(call, sizeof call, "call %llu", (unsigned long long)(st.checked + st.mismatches + 1));
    if (irq) {                 /* 2. an interrupt landed in it: compare it deferred (a slot past MAX_DEPTH) */
        std::vector<uint32_t> cov;
        std::string bad = run_deferred(c, fn, r0, xrts0, call, cov);
        if (bad.empty()) {
            st.checked++;
            st.deferred++;
            std::unordered_set<uint32_t> timed(c.cov.begin(), c.cov.end());
            for (uint32_t pc : cov)
                if (timed.count(pc)) cover(st, fn, &pc, &pc + 1);
        } else {
            st.mismatches++;
            if (st.first.empty()) st.first = bad;
        }
        for (uint32_t pc : std::unordered_set<uint32_t>(c.cov.begin(), c.cov.end()))
            if (!st.covered.count(pc)) {
                uint8_t &n = st.tries[pc];
                if (n < DEFER_TRIES) n++;
            }
        pop();                 /* (run_deferred put the timed result back) */
        return 1;
    }

    /* 3. the C run: untimed (no slice end can park it), hardware replayed. The generated result is
       copied only now: a probe that found nothing new (kit 0.9.10) and a deferred call never read it */
    memcpy(c.post, g_ram, 0x10000);
    uint32_t clk = g_xclk, start = g_xstart, end = g_xend;
    g_xend = 0xFFFFFFFFu;
    memcpy(g_ram, c.pre, 0x10000);
    g_cpu = r0;
    g_xrts = xrts0;
    c.mode = 2;
    c.readpos = 0;
    g_xirq_hold = 1;
    run_c(fn);
    g_xirq_hold = 0;

    /* 4. compare */
    std::string bad = compare(c, fn, r1, xrts1, call);
    if (bad.empty()) {
        st.checked++;
        cover(st, fn, c.cov.begin(), c.cov.end());
    } else {
        st.mismatches++;
        if (st.first.empty()) st.first = bad;
    }

    /* 5. the generated result stands */
    memcpy(g_ram, c.post, 0x10000);
    g_cpu = r1;
    g_xrts = xrts1;
    g_xclk = clk;
    g_xstart = start;
    g_xend = end;
    pop();
    return 1;
}
