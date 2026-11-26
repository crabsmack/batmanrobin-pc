/* game_mem.cpp - work RAM as the native machine sees it (phase 5 P6; native_machine.h): a DMA's
 * source, the Z80's bank window, power-on and a branch snapshot's copy. All game RAM is the native
 * arena (game/runtime/arena.cpp, ruling R1: the original layout in host byte order with per-byte
 * store tags): the machine reads it big-endian, as the 68000's bus delivers it (dk_arena_read_be),
 * and writes it through dk_mem.h as the C does (a plain byte store, its tag kept right). Compiled
 * into the native game library (DK_NATIVE).
 */
#include <string.h>

#include "dk_mem.h"
#include "native_machine.h"
#include "state.h"

extern "C" {
void dk_arena_init(void);
void dk_arena_read_be(uint32_t off, uint8_t *out, uint32_t n);
void dk_arena_branch_save(void);
void dk_arena_branch_restore(void);
}

extern "C" void nm_ram_init(void) { dk_arena_init(); }

extern "C" uint8_t nm_ram_read8(uint32_t a)
{
    uint8_t b;
    dk_arena_read_be(a & 0xFFFFu, &b, 1);
    return b;
}

extern "C" uint16_t nm_ram_read16(uint32_t a)
{
    uint8_t b[2];
    dk_arena_read_be(a & 0xFFFFu, b, 2);
    return (uint16_t)(b[0] << 8 | b[1]);
}

extern "C" void nm_ram_write8(uint32_t a, uint8_t v) { *AT68(u8, 0xFF0000u | (a & 0xFFFFu)) = v; }
extern "C" void nm_ram_write16(uint32_t a, uint16_t v) { *AT68(u16, 0xFF0000u | (a & 0xFFFEu)) = v; }
extern "C" void nm_ram_write32(uint32_t a, uint32_t v) { *AT68(u32, 0xFF0000u | (a & 0xFFFEu)) = v; }

/* A branch snapshot: the arena keeps its own copy, values and tags (one snapshot at a time, as the
   runner's branch plans take them). */
extern "C" size_t nm_ram_save(void *dst, size_t cap)
{
    (void)cap;
    if (dst) dk_arena_branch_save();
    return 0;
}

extern "C" int nm_ram_load(const void *src, size_t len)
{
    (void)src;
    (void)len;
    dk_arena_branch_restore();
    return 1;
}

/* Quick saves and rewind (state.cpp): the arena's 64 KB of values and its store tags, as they are
   (host order, the tags saying which bytes hold which units). After nm_ram_init: the arena is mapped
   once and stays where it is. */
extern "C" void nm_ram_state_init(void)
{
    state_section_range("arena", dk_arena, 0x10000);
    state_section_range("arena", dk_tags, sizeof dk_tags);
}
