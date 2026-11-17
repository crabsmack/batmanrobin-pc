/* dk_bridge.h - what the generated bridge (dkit genesis recomp bridge) and the hand-C runtime
 * share. Written by the kit's recomp template; the bridge.cpp it includes is build output.
 * The only C that sees the 68000's registers is the bridge and game/runtime/handc_rt.cpp: the
 * hand-written C itself never does (lint rule `registers`).
 */
#ifndef DK_BRIDGE_H
#define DK_BRIDGE_H

#include <stdint.h>
#include <type_traits>

#ifdef DK_NATIVE
/* The native build (phase 5 ruling R8): the register record and the calls of the native flow
   runtime (game/runtime/dk_flow_rt.h), nothing of the engine's. The helpers below are the
   bridge's, outside namespace dk::flow (the runtime's own names stay in its own object). */
#include "runtime/dk_flow_rt.h"
#include "game.h"

namespace dk {

extern uint32_t g_fn_current;
struct FnScope {
    uint32_t saved;
    explicit FnScope(uint32_t a) : saved(g_fn_current) { g_fn_current = a; }
    ~FnScope() { g_fn_current = saved; }
};

namespace bridge {
using flow::g_regs;
inline uint32_t reg(int r) { return r < 8 ? g_regs.d[r] : g_regs.a[r - 8]; }
inline void set_reg(int r, uint32_t v, int bytes)
{
    if (r >= 8) {   /* an address register is always written whole (word writes sign-extend) */
        g_regs.a[r - 8] = bytes == 2 ? (uint32_t)(int32_t)(int16_t)v : v;
        return;
    }
    uint32_t &d = g_regs.d[r];
    if (bytes >= 4)      d = v;
    else if (bytes == 2) d = (d & 0xFFFF0000u) | (v & 0xFFFFu);
    else                 d = (d & 0xFFFFFF00u) | (v & 0xFFu);
}
inline void save(uint32_t *s)          /* d0-d7, a0-a6 (A7 is the call's own business) */
{
    for (int i = 0; i < 8; i++) s[i] = g_regs.d[i];
    for (int i = 0; i < 7; i++) s[8 + i] = g_regs.a[i];
}
inline void restore(const uint32_t *s)
{
    for (int i = 0; i < 8; i++) g_regs.d[i] = s[i];
    for (int i = 0; i < 7; i++) g_regs.a[i] = s[8 + i];
}
inline int flag(int bit) { return (g_regs.sr >> bit) & 1; }
inline void set_flag(int bit, int on) { g_regs.sr = (uint16_t)((g_regs.sr & ~(1u << bit)) | ((on ? 1u : 0u) << bit)); }
inline void flags_get(Flags *f) { *f = Flags::from_sr(g_regs.sr); }
/* A Flags member as stored: its byte read as unsigned char, nonzero = set. A bool holding any
   other byte than 0/1 (an uninitialized Flags passed as a call's ccr: Batman & Robin's GCC hang,
   kit 0.10.1) then still sets one condition code and nothing else, under either compiler. */
inline unsigned cc_bit(const bool &b, unsigned bit)
{
    unsigned char u;
    memcpy(&u, &b, 1);
    return u != 0 ? bit : 0u;
}
inline void flags_set(const Flags *f)   /* the condition codes only */
{
    unsigned cc = cc_bit(f->x, 0x10u) | cc_bit(f->n, 0x8u) | cc_bit(f->z, 0x4u) | cc_bit(f->v, 0x2u) | cc_bit(f->c, 0x1u);
    g_regs.sr = (uint16_t)((g_regs.sr & ~0x1Fu) | cc);
}
inline void tst(int r, int bytes)      /* tst.s r: N and Z from r at that width, V and C clear, X kept */
{
    uint32_t v = reg(r);
    uint32_t m = bytes >= 4 ? 0xFFFFFFFFu : bytes == 2 ? 0xFFFFu : 0xFFu;
    uint32_t sign = bytes >= 4 ? 0x80000000u : bytes == 2 ? 0x8000u : 0x80u;
    uint16_t sr = (uint16_t)(g_regs.sr & ~0x000Fu);
    if (!(v & m)) sr |= 0x4;
    if (v & sign) sr |= 0x8;
    g_regs.sr = sr;
}
}  /* namespace bridge */

#else  /* the emulated runners: the 68000's registers (handc_rt.cpp) */
extern "C" {
#include "genesis_runtime.h"
int  x68k_entry(uint32_t addr);
uint32_t dk_reg(int r);                          /* d0-d7 = 0-7, a0-a7 = 8-15 */
void dk_set_reg(int r, uint32_t v, int bytes);   /* a .w/.b write keeps a data register's upper bytes */
void dk_set_flag(int bit, int on);               /* a condition code: C 0, V 1, Z 2, N 3, X 4 */
void dk_tst(int r, int bytes);                   /* the condition codes as tst.s leaves them */
void dk_regs_save(uint32_t *saved);              /* d0-d7, a0-a6 */
void dk_regs_restore(const uint32_t *saved);
void dk_slot_return(void);                       /* the slot entry's RTS */
}
#include "dk_flow.h"
extern "C" void dk_flags_get(dk::Flags *f);      /* the ccr pseudo-register from SR */
extern "C" void dk_flags_set(const dk::Flags *f); /* and back */

#include "game.h"   /* the game's headers: dk_mem.h, the types, the globals, every prototype */

namespace dk {

/* Call the generated code (or C behind its slot entry) at `a` like a JSR. A call the 68000 never
   came back from (a yield, a return past its caller) throws Unwind: the C caller stops there too,
   and its slot entry returns without an RTS, as the generated code's call sites unwind. */
void call_gen(uint32_t a);
struct Unwind {};

/* Set by dk_flow.h's exits (jump, jump_script, rte): the slot entry doesn't RTS. */
extern int g_left;

/* The C function running (a crash report and verify name it): set while a wrapper runs. */
extern uint32_t g_fn_current;
struct FnScope {
    uint32_t saved;
    explicit FnScope(uint32_t a) : saved(g_fn_current) { g_fn_current = a; }
    ~FnScope() { g_fn_current = saved; }
};
#endif /* DK_NATIVE */

template <class T> inline uint32_t to_u32(T v)
{
    if constexpr (std::is_pointer<T>::value) return addr((const void *)v);
    else if constexpr (std::is_class<T>::value) return (uint32_t)v.get();   /* be<>, bptr<> by value */
    else return (uint32_t)(int32_t)v;
}
template <class T> inline T from_u32(uint32_t s)
{
    if constexpr (std::is_pointer<T>::value) return (T)host(s);
    else if constexpr (std::is_same<T, bool>::value) return s != 0;
    else if constexpr (std::is_class<T>::value) { T r{}; r.set((decltype(r.get()))s); return r; }
    else if constexpr (sizeof(T) == 1) return (T)(uint8_t)s;
    else if constexpr (sizeof(T) == 2) return (T)(uint16_t)s;
    else return (T)s;
}
template <class T> constexpr int val_bytes()
{
    if constexpr (std::is_pointer<T>::value) return 4;
    else if constexpr (std::is_same<T, bool>::value) return 1;
    else return (int)sizeof(T) > 4 ? 4 : (int)sizeof(T);
}

}  /* namespace dk */

#endif /* DK_BRIDGE_H */
