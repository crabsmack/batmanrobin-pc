/* arena.cpp - the native arena: the game's work RAM in native storage (phase 5 criterion P5,
 * ruling R1: docs/phases/P5-port.md). The memory layer under the hand-written C in the native
 * build (dk_mem.h, DK_NATIVE); from xmen2's game/runtime/native_rt.cpp.
 *
 *   Layout: the 64 KB of the 68000's work RAM, same sizes and offsets, mapped twice back to back:
 *   dk_arena is the RAM as $00FFxxxx, dk_arena_ffff (= dk_arena + $10000) the same memory as
 *   $FFFFxxxx. The original stores both forms of RAM pointers, so a host pointer into either view
 *   converts back to exactly the value the original had (dk::addr), and the two compare unequal
 *   as on the 68000. Stored pointers keep their 68000 values (dk::bptr: a 32-bit value).
 *
 *   Host byte order with a store tag per byte (R1): a value stored at its own width (a field, a
 *   global, a stack long: dk::mem<T>) is kept in host byte order, and every byte has a tag:
 *   0 = a plain byte (its value as the 68000 sees it), or (W << 4 | i) = byte i (from the unit's
 *   first, lowest 68000 address) of a W-byte unit stored whole in host order. A read at the width
 *   the value was stored (tags[off] == W << 4) is a host load; any other read (a word read of a
 *   long's half, a byte of a word, two bytes read as a word) is assembled from the bytes as the
 *   68000 sees them, and a store that cuts a unit first turns that unit's other bytes into plain
 *   bytes. So the arena always holds exactly the 68000's RAM image, in host order where the code
 *   is layout-independent, and the export (dk_arena_image) is a per-unit byte swap.
 *
 *   Everything the game keeps in work RAM is here: the globals, every record and buffer, the
 *   native flow runtime's stack (A7's bytes at their original addresses, through dk_mem.h's
 *   types like any other game memory: platform/native/flow_native.cpp) and the H-int RAM code
 *   (runtime/hint_raster.cpp reads it the same way). The VDP's DMA from work RAM reads it through
 *   dk_arena_read_be (big-endian, as the 68000 bus delivers it; until P6 the platform backend
 *   stages those words for the engine's VDP model).
 *
 *   Census (DKIT_ARENA_CENSUS=<path>): at exit, every C function that read or wrote game memory
 *   at another width than it was stored (the layout-dependent sites: a byte buffer read as words,
 *   a word written then read as bytes), with counts and the first offset; and the arena's shape
 *   (bytes in host-order units vs plain bytes). docs/NOTES.md "Phase 5: the native arena".
 */
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#else
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#endif

#include "dk_mem.h"

extern "C" {
uint8_t *dk_arena;            /* work RAM as $00FFxxxx */
uint8_t *dk_arena_ffff;       /* the same memory as $FFFFxxxx: dk_arena + $10000 */
uint8_t dk_tags[0x10000];     /* the store tag of each byte (above) */
}

namespace dk {

extern uint32_t g_fn_current;   /* the C function running (the runtime's: the census names it) */

/* ---- the mapping -------------------------------------------------------------------------- */

static void *map_twice(void)
{
#ifdef _WIN32
    HANDLE h = CreateFileMappingW(INVALID_HANDLE_VALUE, nullptr, PAGE_READWRITE, 0, 0x10000, nullptr);
    if (!h) return nullptr;
    for (int attempt = 0; attempt < 64; attempt++) {
        /* 128 KB of free address space, then the section mapped into both halves (another thread
           could take the range in between: try again) */
        void *base = VirtualAlloc(nullptr, 0x20000, MEM_RESERVE, PAGE_NOACCESS);
        if (!base) break;
        VirtualFree(base, 0, MEM_RELEASE);
        void *a = MapViewOfFileEx(h, FILE_MAP_ALL_ACCESS, 0, 0, 0x10000, base);
        if (!a) continue;
        void *b = MapViewOfFileEx(h, FILE_MAP_ALL_ACCESS, 0, 0, 0x10000, (uint8_t *)base + 0x10000);
        if (b) return a;
        UnmapViewOfFile(a);
    }
    return nullptr;
#else
    char name[64];
    snprintf(name, sizeof name, "/dk-arena-%ld", (long)getpid());
    int fd = shm_open(name, O_RDWR | O_CREAT | O_EXCL, 0600);
    if (fd < 0) return nullptr;
    shm_unlink(name);
    if (ftruncate(fd, 0x10000) != 0) { close(fd); return nullptr; }
    void *base = mmap(nullptr, 0x20000, PROT_NONE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (base == MAP_FAILED) { close(fd); return nullptr; }
    void *a = mmap(base, 0x10000, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_FIXED, fd, 0);
    void *b = mmap((uint8_t *)base + 0x10000, 0x10000, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_FIXED, fd, 0);
    close(fd);
    return a != MAP_FAILED && b != MAP_FAILED ? a : nullptr;
#endif
}

/* ---- host byte order (R1) ---------------------------------------------------------------- */

static inline uint8_t be_byte(uint32_t off)
{
    uint8_t t = dk_tags[off];
    if (!t) return dk_arena[off];
    uint32_t w = t >> 4, i = t & 15u, start = off - i;
    return dk_arena[(start + (w - 1u - i)) & 0xFFFFu];
}

/* a plain byte stored (after the units at the store's ends became bytes) */
static inline void put_byte(uint32_t off, uint8_t v)
{
    dk_arena[off] = v;
    dk_tags[off] = 0;
}

/* the unit holding off becomes plain bytes */
static inline void unit_to_bytes(uint32_t off)
{
    uint8_t t = dk_tags[off];
    if (!t) return;
    uint32_t w = t >> 4, start = off - (t & 15u);
    uint8_t be[4];
    for (uint32_t k = 0; k < w; k++) be[k] = be_byte(start + k);
    for (uint32_t k = 0; k < w; k++) { dk_arena[start + k] = be[k]; dk_tags[start + k] = 0; }
}

/* ---- the census of layout-dependent accesses --------------------------------------------- */

static int s_census = -1;   /* -1 not read yet, 0 off, 1 on */
static uint64_t s_slow_loads, s_slow_stores, s_tagged_copies;

struct Site { uint32_t fn; uint8_t kind, width, shape; uint64_t n; uint32_t first_off; };
static Site s_sites[4096];
static int s_nsites;

static void census_dump(void)
{
    const char *path = getenv("DKIT_ARENA_CENSUS");
    FILE *f = path && *path ? fopen(path, "w") : nullptr;
    if (!f) return;
    uint32_t units = 0;
    for (uint32_t off = 0; off < 0x10000u; off++) units += dk_tags[off] != 0;
    fprintf(f, "# arena census: slow_loads %llu slow_stores %llu tagged_copies %llu; at exit %u bytes in "
               "host-order units, %u plain\n",
            (unsigned long long)s_slow_loads, (unsigned long long)s_slow_stores,
            (unsigned long long)s_tagged_copies, (unsigned)units, (unsigned)(0x10000u - units));
    fprintf(f, "# fn kind width shape count first_off  (kind: L load, S store, C copy/fill; shape: the first "
               "byte's tag, W<<4|i, 00 = plain bytes)\n");
    for (int i = 0; i < s_nsites; i++)
        fprintf(f, "%06X %c %u %02X %llu FF%04X\n", (unsigned)s_sites[i].fn, s_sites[i].kind, s_sites[i].width,
                s_sites[i].shape, (unsigned long long)s_sites[i].n, (unsigned)s_sites[i].first_off);
    fclose(f);
}

static inline bool census_on(void)
{
    if (s_census < 0) {
        const char *e = getenv("DKIT_ARENA_CENSUS");
        s_census = e && *e ? 1 : 0;
        if (s_census) atexit(census_dump);
    }
    return s_census == 1;
}

static void census_note(char kind, uint32_t off, int w)
{
    uint32_t fn = g_fn_current & 0xFFFFFFu;
    uint8_t shape = dk_tags[off];
    for (int i = 0; i < s_nsites; i++)
        if (s_sites[i].fn == fn && s_sites[i].kind == (uint8_t)kind && s_sites[i].width == (uint8_t)w
            && s_sites[i].shape == shape) {
            s_sites[i].n++;
            return;
        }
    if (s_nsites < (int)(sizeof s_sites / sizeof s_sites[0]))
        s_sites[s_nsites++] = Site{ fn, (uint8_t)kind, (uint8_t)w, shape, 1u, off };
}

uint32_t arena_load_slow(uint32_t off, int w)
{
    s_slow_loads++;
    if (off + (uint32_t)w > 0x10000u) fatal("a %d-byte read at $FF%04X runs off work RAM", w, (unsigned)off);
    if (census_on()) census_note('L', off, w);
    uint32_t v = 0;
    for (int k = 0; k < w; k++) v = v << 8 | be_byte(off + (uint32_t)k);
    return v;
}

void arena_store_slow(uint32_t off, int w, uint32_t v)
{
    s_slow_stores++;
    if (off + (uint32_t)w > 0x10000u) fatal("a %d-byte write at $FF%04X runs off work RAM", w, (unsigned)off);
    if (census_on()) census_note('S', off, w);
    unit_to_bytes(off);
    if (w > 1) unit_to_bytes(off + (uint32_t)w - 1u);
    for (int k = 0; k < w; k++) {   /* host order (little-endian), tagged */
        dk_arena[off + (uint32_t)k] = (uint8_t)(v >> (8 * k));
        dk_tags[off + (uint32_t)k] = w > 1 ? (uint8_t)(w << 4 | k) : 0;
    }
}

/* ---- block copies and fills (dk_mem.h dk::mem_copy / dk::mem_set) ------------------------ */

static inline bool in_arena(const void *p, size_t n, uint32_t *off)
{
    uintptr_t a = (uintptr_t)p - (uintptr_t)dk_arena;
    if (a >= 0x20000u) return false;
    *off = (uint32_t)a & 0xFFFFu;
    if (*off + n > 0x10000u) fatal("a %u-byte block at $FF%04X runs off work RAM", (unsigned)n, (unsigned)*off);
    return true;
}

static inline bool in_rom(const void *p) { return (uintptr_t)p - (uintptr_t)g_rom < 0x400000u; }

/* the 68000's view of n bytes at s (the arena by its tags; the ROM image and host memory as they are) */
static void read_be(uint8_t *d, const void *s, size_t n)
{
    uint32_t off;
    if (in_arena(s, n, &off)) {
        for (size_t i = 0; i < n; i++) d[i] = be_byte(off + (uint32_t)i);
        return;
    }
    ::memcpy(d, s, n);
}

void *mem_set(void *d, int v, size_t n)
{
    uint32_t off;
    if (!n) return d;
    if (in_arena(d, n, &off)) {
        s_tagged_copies++;
        if (census_on()) census_note('C', off, 1);
        unit_to_bytes(off);
        unit_to_bytes(off + (uint32_t)n - 1u);
        for (size_t i = 0; i < n; i++) put_byte(off + (uint32_t)i, (uint8_t)v);
        return d;
    }
    if (in_rom(d)) fatal("a fill of the ROM image at $%06X", (unsigned)((uintptr_t)d - (uintptr_t)g_rom));
    return ::memset(d, v, n);
}

void *mem_copy(void *d, const void *s, size_t n)
{
    uint32_t off;
    if (!n) return d;
    if (in_arena(d, n, &off)) {
        s_tagged_copies++;
        if (census_on()) census_note('C', off, 1);
        uint8_t buf[256];
        size_t done = 0;
        unit_to_bytes(off);
        unit_to_bytes(off + (uint32_t)n - 1u);
        while (done < n) {   /* the bytes as the 68000 sees them, stored as plain bytes */
            size_t k = n - done > sizeof buf ? sizeof buf : n - done;
            read_be(buf, (const uint8_t *)s + done, k);
            for (size_t i = 0; i < k; i++) put_byte(off + (uint32_t)(done + i), buf[i]);
            done += k;
        }
        return d;
    }
    if (in_rom(d)) fatal("a copy into the ROM image at $%06X", (unsigned)((uintptr_t)d - (uintptr_t)g_rom));
    read_be((uint8_t *)d, s, n);
    return d;
}

}  /* namespace dk */

/* ---- the runtime's side ------------------------------------------------------------------ */

static uint8_t s_branch_ram[0x10000], s_branch_tags[0x10000];

extern "C" {

/* Power-on: the arena zeroed (as the stepped runners' RAM at reset), every byte plain. Before the
   game's first instruction (spec.c game_start). */
void dk_arena_init(void)
{
    if (dk_arena) return;
    const uint16_t one = 1;
    if (*(const uint8_t *)&one != 1) dk::fatal("the native arena keeps game memory in host order: a little-endian host");
    uint8_t *a = (uint8_t *)dk::map_twice();
    if (!a) dk::fatal("cannot map the native arena twice");
    dk_arena = a;
    dk_arena_ffff = a + 0x10000;
    memset(dk_arena, 0, 0x10000);
    memset(dk_tags, 0, sizeof dk_tags);
}

/* The 68000's image of work RAM ($FF0000-$FFFFFF), the per-unit byte swap of R1: 8 bytes at a
   time where they are plain bytes, four word units or two long units (the common shapes), byte by
   byte by their tags elsewhere. */
void dk_arena_image(uint8_t *out)
{
    static const uint64_t PLAIN = 0, WORDS = 0x2120212021202120ull, LONGS = 0x4342414043424140ull;
    for (uint32_t off = 0; off < 0x10000u; off += 8) {
        uint64_t t, v;
        memcpy(&t, dk_tags + off, 8);
        memcpy(&v, dk_arena + off, 8);
        if (t == PLAIN) {
        } else if (t == WORDS) {
            v = ((v & 0x00FF00FF00FF00FFull) << 8) | ((v >> 8) & 0x00FF00FF00FF00FFull);
        } else if (t == LONGS) {
            v = ((v & 0x00FF00FF00FF00FFull) << 8) | ((v >> 8) & 0x00FF00FF00FF00FFull);
            v = ((v & 0x0000FFFF0000FFFFull) << 16) | ((v >> 16) & 0x0000FFFF0000FFFFull);
        } else {
            for (uint32_t k = 0; k < 8; k++) out[off + k] = dk::be_byte(off + k);
            continue;
        }
        memcpy(out + off, &v, 8);
    }
}

/* n bytes from the 68000 address's RAM offset, big-endian as the bus delivers them: the VDP's DMA
   from work RAM (platform/platform_bus.cpp stages them for the engine's VDP until P6; P6's native
   VDP reads its DMA source words through this); the offset wraps in the 64 KB as the 68000's RAM
   mirror. */
void dk_arena_read_be(uint32_t off, uint8_t *out, uint32_t n)
{
    for (uint32_t i = 0; i < n; i++) out[i] = dk::be_byte((off + i) & 0xFFFFu);
}

/* The arena's shape and the layout-dependent accesses so far (the counters line). */
int dk_arena_stats(char *buf, int n)
{
    uint32_t units = 0;
    for (uint32_t off = 0; off < 0x10000u; off++) units += dk_tags[off] != 0;
    return snprintf(buf, (size_t)n, "{\"host_order_bytes\": %u, \"plain_bytes\": %u, "
                    "\"slow_loads\": %llu, \"slow_stores\": %llu, \"tagged_copies\": %llu}", (unsigned)units,
                    (unsigned)(0x10000u - units),
                    (unsigned long long)dk::s_slow_loads, (unsigned long long)dk::s_slow_stores,
                    (unsigned long long)dk::s_tagged_copies);
}

/* A branch plan's snapshot (the runner's --branch-plan) holds the engine's machine; the game's
   RAM is here, so it goes with it (spec.c game_branch_save / restore). */
void dk_arena_branch_save(void)
{
    memcpy(s_branch_ram, dk_arena, sizeof s_branch_ram);
    memcpy(s_branch_tags, dk_tags, sizeof s_branch_tags);
}

void dk_arena_branch_restore(void)
{
    memcpy(dk_arena, s_branch_ram, sizeof s_branch_ram);
    memcpy(dk_tags, s_branch_tags, sizeof s_branch_tags);
}

}  /* extern "C" */
