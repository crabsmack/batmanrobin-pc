/* dk_mem.h - how hand-written C reaches game memory inside the recomp (phase 4).
 * Written by the kit's recomp template (from xmen2's xmen2_mem.h, PC mode); yours to extend.
 *
 * Every game/c/<ADDR>.c is compiled as C++17 inside the recomp runners. Game memory is still the
 * emulated 68000's (work RAM, ROM; big-endian), because the generated code of the functions not
 * yet written in C shares it. The types below make plain-looking C read and write it. Phase 5
 * (the port) changes these types, not the C.
 *
 * Rules for C written against these headers (lint: dkit genesis recomp lint):
 *   - s8 u8 s16 u16 s32 u32 for every value that lives in game memory: struct fields, globals,
 *     whatever a pointer points at. Plain short/int/long for locals and value parameters (they
 *     live on the host).
 *   - Pointers are plain C pointers (T *). A pointer stored IN game memory (a field, a global)
 *     is PTR(T).
 *   - Hardware through the platform layer (platform.h -> recomp/platform/), never by address.
 *   - No register access: the C never sees the 68000 (the bridge maps registers to parameters).
 *
 * How pointers keep their 68000 value: the 68000 has 24 address bits, so one work-RAM byte is
 * $00FFxxxx or $FFFFxxxx depending on how the original computed the pointer (absolute short
 * addressing gives $FFFFxxxx), and the original stores both forms in RAM. The runner maps work
 * RAM twice (g_ram = $00FFxxxx, g_ram_ffff = $FFFFxxxx, the same memory): a host pointer into
 * either view converts back to exactly the 68000 value the original had, and pointer
 * comparisons behave as on the 68000 (the two forms compare unequal there too).
 */
#ifndef DK_MEM_H
#define DK_MEM_H

/* Modes (phase 5 ruling R4: docs/phases/P5-port.md). Every game/c file and game/include header
 * compiles in both, switched here and in the headers this one includes:
 *   emulated PC  the stepped, stepped-gen and verify runners: game memory is the emulated 68000's
 *                (below), the C's flow and calls go through the 68000 (handc_rt.cpp, the bridge);
 *   native       DK_NATIVE, the native runner's handc library (CMakeLists.txt): the port moves
 *                state, flow and hardware to native storage, a native flow runtime and the
 *                platform layer HERE, in the headers, not in the C. Until it does, this branch is
 *                the PC one, and the exporter copies (and counts) the emulated RAM. */
#ifdef DK_NATIVE
#define DK_MODE_NATIVE 1
#else
#define DK_MODE_NATIVE 0
#endif

#ifndef __cplusplus
#error "hand-written C builds as C++ (dk_mem.h)"
#endif
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <type_traits>

extern "C" {
#if DK_MODE_NATIVE
/* native: the arena (game/runtime/arena.cpp), mapped twice back to back: $00FFxxxx, then the same
   memory as $FFFFxxxx; and each byte's store tag */
extern uint8_t *dk_arena;
extern uint8_t *dk_arena_ffff;
extern uint8_t  dk_tags[0x10000];
#define DK_RAM                 dk_arena
#define DK_RAM_FFFF            dk_arena_ffff
/* globals.h (generated from src/ram.asm) names the $FFFF view by the emulated RAM's name: in the
   native build that view is the arena's, so every global lives in the arena at its address */
#define g_ram_ffff             dk_arena_ffff
#else
extern uint8_t *g_ram;        /* work RAM as $00FFxxxx (the engine's view) */
extern uint8_t *g_ram_ffff;   /* the same RAM as $FFFFxxxx */
#define DK_RAM                 g_ram
#define DK_RAM_FFFF            g_ram_ffff
#endif
extern uint8_t  g_rom[0x400000];
uint8_t  m68k_read8(uint32_t addr);
uint16_t m68k_read16(uint32_t addr);
uint32_t m68k_read32(uint32_t addr);
void     m68k_write8(uint32_t addr, uint8_t v);
void     m68k_write16(uint32_t addr, uint16_t v);
void     m68k_write32(uint32_t addr, uint32_t v);
}

namespace dk {

/* A fatal error in the C: the runner stops with it (verify reports it as stopped_early). */
[[noreturn]] void fatal(const char *fmt, ...);

/* 68000 address -> host pointer (0 -> null). */
inline void *host(uint32_t a)
{
    if (!a) return nullptr;
    uint32_t a24 = a & 0xFFFFFFu;
    if (a24 >= 0xFF0000u) {
        uint32_t top = a >> 24;
        if (top == 0x00u) return DK_RAM + (a24 & 0xFFFFu);
        if (top == 0xFFu) return DK_RAM_FFFF + (a24 & 0xFFFFu);
    } else if (a == a24 && a24 < 0x400000u) {
        return g_rom + a24;
    }
    fatal("dk::host: $%08X is not a work-RAM or ROM address", (unsigned)a);
}

/* host pointer -> the 68000 address it stands for (null -> 0). */
inline uint32_t addr(const void *p)
{
    if (!p) return 0;
    uintptr_t u = (uintptr_t)p;
    if (u - (uintptr_t)DK_RAM < 0x10000u)      return 0x00FF0000u + (uint32_t)(u - (uintptr_t)DK_RAM);
    if (u - (uintptr_t)DK_RAM_FFFF < 0x10000u) return 0xFFFF0000u + (uint32_t)(u - (uintptr_t)DK_RAM_FFFF);
    if (u - (uintptr_t)g_rom < 0x400000u)      return (uint32_t)(u - (uintptr_t)g_rom);
    fatal("dk::addr: host pointer %p is not in game memory", p);
}

/* A block the game names by a word (`movea.w $8(a6),a3`: sign-extended, so $8000-$FFFF is work
   RAM at $FFFF8000-$FFFFFFFF), as a host pointer; and back. */
template <class T = void> inline T *handle(int16_t h) { return (T *)host((uint32_t)(int32_t)h); }
inline int16_t handle_of(const void *p) { return (int16_t)(uint16_t)addr(p); }

inline bool in_game_memory(const void *p)
{
    uintptr_t u = (uintptr_t)p;
    return u - (uintptr_t)DK_RAM < 0x10000u || u - (uintptr_t)DK_RAM_FFFF < 0x10000u
        || u - (uintptr_t)g_rom < 0x400000u;
}

#if DK_MODE_NATIVE
/* NATIVE (phase 5 P5, ruling R1): a scalar in game memory, held in host byte order. Where it is
   decides how it reads: in the arena through its store tag (game/runtime/arena.cpp: a value read
   at the width it was stored is a host load; any other read is assembled from the bytes as the
   68000 sees them, so layout-dependent code reads what the original read); in the ROM image as
   the big-endian bytes it is (R2); in host memory (a C local of a game type) as a host value.
   Copying one to another goes through the value, so copying a record (a ROM record into RAM, RAM
   into a local) converts field by field. Bytes (s8, u8) too: a byte store into a word makes that
   word plain bytes (its tags), so it stays exact. */
uint32_t arena_load_slow(uint32_t off, int w);
void     arena_store_slow(uint32_t off, int w, uint32_t v);

template <class T>
struct mem {
    static_assert(sizeof(T) == 1 || sizeof(T) == 2 || sizeof(T) == 4, "mem<T>: 8, 16 or 32 bits");
    static constexpr uint8_t TAG = sizeof(T) > 1 ? (uint8_t)(sizeof(T) << 4) : 0;
    uint8_t b[sizeof(T)];

    T get() const
    {
        uintptr_t u = (uintptr_t)b;
        uintptr_t a = u - (uintptr_t)dk_arena;
        if (a < 0x20000u) {                       /* the arena (both views) */
            uint32_t off = (uint32_t)a & 0xFFFFu;
            if (dk_tags[off] == TAG) { T v; memcpy(&v, b, sizeof v); return v; }
            return (T)arena_load_slow(off, (int)sizeof(T));
        }
        if (u - (uintptr_t)g_rom < 0x400000u) {   /* the ROM image: big-endian */
            uint32_t v = 0;
            for (size_t i = 0; i < sizeof(T); i++) v = v << 8 | b[i];
            return (T)v;
        }
        if (u < 0x10000u) fatal("a read through a null reference (+$%04X)", (unsigned)u);
        T v;                                      /* host memory */
        memcpy(&v, b, sizeof v);
        return v;
    }
    void set(T v)
    {
        uintptr_t u = (uintptr_t)b;
        uintptr_t a = u - (uintptr_t)dk_arena;
        if (a < 0x20000u) {
            uint32_t off = (uint32_t)a & 0xFFFFu;
            if (dk_tags[off] == TAG) { memcpy(b, &v, sizeof v); return; }
            arena_store_slow(off, (int)sizeof(T), (uint32_t)v);
            return;
        }
        if (u - (uintptr_t)g_rom < 0x400000u) fatal("a store into the ROM image at $%06X", (unsigned)(u - (uintptr_t)g_rom));
        if (u < 0x10000u) fatal("a store through a null reference (+$%04X)", (unsigned)u);
        memcpy(b, &v, sizeof v);
    }
    mem() = default;
    mem(const mem &o) { set(o.get()); }
    mem &operator=(const mem &o) { set(o.get()); return *this; }
    /* explicit: `c ? 5 : field` then converts the field to its T (as C promotes it), not 5 to a
       mem<T> (both ways make ?: ambiguous, which MSVC lets through and GCC rejects) */
    explicit mem(T v) { set(v); }
    operator T() const { return get(); }
    mem &operator=(T v) { set(v); return *this; }
    template <class U> mem &operator+=(U v)  { set((T)(get() + v)); return *this; }
    template <class U> mem &operator-=(U v)  { set((T)(get() - v)); return *this; }
    template <class U> mem &operator*=(U v)  { set((T)(get() * v)); return *this; }
    template <class U> mem &operator/=(U v)  { set((T)(get() / v)); return *this; }
    template <class U> mem &operator%=(U v)  { set((T)(get() % v)); return *this; }
    template <class U> mem &operator&=(U v)  { set((T)(get() & v)); return *this; }
    template <class U> mem &operator|=(U v)  { set((T)(get() | v)); return *this; }
    template <class U> mem &operator^=(U v)  { set((T)(get() ^ v)); return *this; }
    template <class U> mem &operator<<=(U v) { set((T)(get() << v)); return *this; }
    template <class U> mem &operator>>=(U v) { set((T)(get() >> v)); return *this; }
    mem &operator++() { set((T)(get() + 1)); return *this; }
    mem &operator--() { set((T)(get() - 1)); return *this; }
    T operator++(int) { T o = get(); set((T)(o + 1)); return o; }
    T operator--(int) { T o = get(); set((T)(o - 1)); return o; }
};
template <class T> using be = mem<T>;

/* memset / memcpy on game memory (the C's block fills and copies, dk::mem_set / dk::mem_copy):
   the bytes as the 68000 sees them, stored as plain bytes (game/runtime/arena.cpp). */
void *mem_set(void *d, int v, size_t n);
void *mem_copy(void *d, const void *s, size_t n);
#else
/* A big-endian scalar in game memory: reads and writes like the T it holds. */
template <class T>
struct be {
    static_assert(sizeof(T) == 2 || sizeof(T) == 4, "be<T>: 16 or 32 bits");
    uint8_t b[sizeof(T)];

    T get() const
    {
        if constexpr (sizeof(T) == 2)
            return (T)(uint16_t)((b[0] << 8) | b[1]);
        else
            return (T)(((uint32_t)b[0] << 24) | ((uint32_t)b[1] << 16) | ((uint32_t)b[2] << 8) | b[3]);
    }
    void set(T v)
    {
        uint32_t u = (uint32_t)v;
        if constexpr (sizeof(T) == 2) {
            b[0] = (uint8_t)(u >> 8); b[1] = (uint8_t)u;
        } else {
            b[0] = (uint8_t)(u >> 24); b[1] = (uint8_t)(u >> 16); b[2] = (uint8_t)(u >> 8); b[3] = (uint8_t)u;
        }
    }
    be() = default;
    /* explicit: `c ? 5 : field` then converts the field to its T (as C promotes it), not 5 to a
       be<T> (both ways make ?: ambiguous, which MSVC lets through and GCC rejects) */
    explicit be(T v) { set(v); }
    operator T() const { return get(); }
    be &operator=(T v) { set(v); return *this; }
    template <class U> be &operator+=(U v)  { set((T)(get() + v)); return *this; }
    template <class U> be &operator-=(U v)  { set((T)(get() - v)); return *this; }
    template <class U> be &operator*=(U v)  { set((T)(get() * v)); return *this; }
    template <class U> be &operator/=(U v)  { set((T)(get() / v)); return *this; }
    template <class U> be &operator%=(U v)  { set((T)(get() % v)); return *this; }
    template <class U> be &operator&=(U v)  { set((T)(get() & v)); return *this; }
    template <class U> be &operator|=(U v)  { set((T)(get() | v)); return *this; }
    template <class U> be &operator^=(U v)  { set((T)(get() ^ v)); return *this; }
    template <class U> be &operator<<=(U v) { set((T)(get() << v)); return *this; }
    template <class U> be &operator>>=(U v) { set((T)(get() >> v)); return *this; }
    be &operator++() { set((T)(get() + 1)); return *this; }
    be &operator--() { set((T)(get() - 1)); return *this; }
    T operator++(int) { T o = get(); set((T)(o + 1)); return o; }
    T operator--(int) { T o = get(); set((T)(o - 1)); return o; }
};

/* memset / memcpy on game memory: the emulated RAM is the 68000's bytes already. */
inline void *mem_set(void *d, int v, size_t n) { return memset(d, v, n); }
inline void *mem_copy(void *d, const void *s, size_t n) { return memcpy(d, s, n); }
#endif /* DK_MODE_NATIVE */

/* A pointer stored in game memory: the 68000 address (big-endian; native: a 32-bit mem<>). */
template <class T>
struct bptr {
    be<uint32_t> v;

    T *get() const { return (T *)host(v.get()); }
    uint32_t raw() const { return v.get(); }
    bptr() = default;
    bptr(T *p) { v = addr(p); }
    operator T *() const { return get(); }
    T *operator->() const { return get(); }
    bptr &operator=(T *p) { v = addr(p); return *this; }
    bptr &operator=(decltype(nullptr)) { v = 0u; return *this; }
    bptr &operator+=(long n) { v = v.get() + (uint32_t)(n * (long)sizeof(T)); return *this; }
    bptr &operator-=(long n) { v = v.get() - (uint32_t)(n * (long)sizeof(T)); return *this; }
    bptr &operator++() { return *this += 1; }
    bptr &operator--() { return *this -= 1; }
    T *operator++(int) { T *o = get(); *this += 1; return o; }
    T *operator--(int) { T *o = get(); *this -= 1; return o; }
};
template <>
struct bptr<void> {
    be<uint32_t> v;
    void *get() const { return host(v.get()); }
    uint32_t raw() const { return v.get(); }
    bptr() = default;
    bptr(void *p) { v = addr(p); }
    operator void *() const { return get(); }
    template <class U> operator U *() const { return (U *)get(); }
    bptr &operator=(void *p) { v = addr(p); return *this; }
    bptr &operator=(decltype(nullptr)) { v = 0u; return *this; }
};

/* ROM bytes at a 68000 address (data the C names: the generated data header). */
inline void *rom(uint32_t a) { return g_rom + a; }

}  /* namespace dk */

#if DK_MODE_NATIVE
typedef dk::mem<int8_t>       s8;
typedef dk::mem<uint8_t>      u8;
#else
typedef int8_t                s8;
typedef uint8_t               u8;
#endif
typedef dk::be<int16_t>       s16;
typedef dk::be<uint16_t>      u16;
typedef dk::be<int32_t>       s32;
typedef dk::be<uint32_t>      u32;

#define PTR(T)                 dk::bptr<T>
#define ADDR68(p)              ((uint32_t)dk::addr(p))
#define AT68(T, a)             ((T *)dk::host((uint32_t)(a)))

/* A loop waiting for a flag only an interrupt handler sets (`while (!g_VIntDone) IRQ_WAIT();`):
   on the 68000 the interrupt simply arrives; the frame-synchronous runner takes the V-int there
   (spec.c's dk_irq_wait: the engine's x68k_wait_vint with the pads a wait site gets, so a wait
   for a press sees it), every other runner does nothing. The native build: the flow runtime's
   wait, which the platform answers with the V-int (game/runtime/flow.cpp). */
#ifdef DK_NATIVE
namespace dk { namespace flow { void irq_wait(void); } }
#define IRQ_WAIT()             dk::flow::irq_wait()
#else
extern "C" void x68k_wait_vint(uint32_t pc);
extern "C" void dk_irq_wait(void);
#define IRQ_WAIT()             dk_irq_wait()
#endif

#endif /* DK_MEM_H */
