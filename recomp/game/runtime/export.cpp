/* export.cpp - the native runner's state exporter (phase 5: docs/phases/P5-port.md). Written by
 * the kit's recomp template (from xmen2's game/runtime/export.cpp); yours to extend.
 *
 * Equality with the original is decided by phase 4's sync rows against the stepped-gen
 * reference, so the native runner computes its rows from this image ONLY (spec.c, DK_NATIVE:
 * native_sync_row): never from the emulated machine directly. At each sync point dk_export
 * writes the 68000 image (dk_image.h) of the game state:
 *   1. work RAM from the native arena (game/runtime/arena.cpp; ruling R1: the original layout,
 *      values in host byte order with a per-byte store tag): every one of its 65,536 bytes as the
 *      68000 sees it, a per-unit byte swap driven by the tags (dk_arena_image). All game state
 *      lives there: the globals at their addresses (globals.h), every record and buffer the C
 *      reaches through a 68000 pointer, and the stack (the native flow runtime keeps A7's bytes
 *      at their original addresses through the same types);
 *   2. nothing from the emulated 68000's RAM (copied_bytes, P5's measure: 0, since the native
 *      build keeps no game state there);
 *   3. VRAM, CRAM, VSRAM from the platform's video model, the Z80's RAM from its Z80, in the
 *      reference's formats.
 * The stack area and [recomp] dont_care stay masked in the CRC (spec.c), as phase 4 masks them;
 * nothing else is.
 *
 * Self-test (the exporter -> sync row path is the only one): DKIT_EXPORT_SELFTEST=<RAM address,
 * hex, e.g. FFC5A8> zeroes that byte of every exported image, as if the export dropped it. A trace
 * must then differ from its reference from the first sync point where that byte is non-zero (and
 * outside the masked stack), and only in ram_crc32.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dk_mem.h"
#include "runtime/dk_image.h"
extern "C" {
#include "video/genesis_machine.h"
void dk_arena_image(uint8_t *out);      /* arena.cpp: the arena as the 68000 sees it */
int dk_arena_stats(char *buf, int n);
}

namespace {

struct Stats {
    unsigned long long exports;
    unsigned native_bytes, copied_bytes;   /* the most one export took */
} s_stats;

int s_selftest = -2;   /* -2: not read yet; -1: off; else the RAM offset zeroed */

int selftest(void)
{
    if (s_selftest != -2) return s_selftest;
    s_selftest = -1;
    const char *e = getenv("DKIT_EXPORT_SELFTEST");
    if (e && *e && strcmp(e, "0")) {
        unsigned long a = strtoul(e[0] == '$' ? e + 1 : e, nullptr, 16);
        s_selftest = (int)(a & 0xFFFFu);
        fprintf(stderr, "[export] SELF-TEST: RAM byte $FF%04X is zeroed in every exported image\n", (unsigned)s_selftest);
    }
    return s_selftest;
}

void words_be(uint8_t *out, const uint16_t *w, unsigned n, uint16_t mask)
{
    for (unsigned i = 0; i < n; i++) {
        uint16_t v = (uint16_t)(w[i] & mask);
        out[2 * i] = (uint8_t)(v >> 8);
        out[2 * i + 1] = (uint8_t)v;
    }
}

}  // namespace

extern "C" void dk_export(DkImage *im)
{
    /* 1. the arena, every byte, as the 68000 sees it */
    dk_arena_image(im->ram);
    const unsigned native = DK_IMAGE_RAM_BYTES;
    /* 2. nothing from the emulated RAM */
    const unsigned copied = 0;
    /* 3. the video model and the Z80 */
    const GVDP *v = &g_machine.vdp;
    static_assert(sizeof im->vram == GVDP_VRAM_SIZE, "VRAM size");
    memcpy(im->vram, v->vram, sizeof im->vram);
    words_be(im->cram, v->cram, DK_IMAGE_CRAM_WORDS, 0x0EEE);
    words_be(im->vsram, v->vsram, DK_IMAGE_VSRAM_WORDS, 0x07FF);
    static_assert(sizeof im->z80 == sizeof g_machine.bus.z80_ram, "Z80 RAM size");
    memcpy(im->z80, g_machine.bus.z80_ram, sizeof im->z80);

    if (selftest() >= 0) im->ram[selftest()] = 0;
    s_stats.exports++;
    if (native > s_stats.native_bytes) s_stats.native_bytes = native;
    if (copied > s_stats.copied_bytes) s_stats.copied_bytes = copied;
}

extern "C" int dk_export_stats(char *buf, int n)
{
    char st[16] = "null";
    if (selftest() >= 0) snprintf(st, sizeof st, "\"FF%04X\"", (unsigned)selftest());
    char arena[256];
    dk_arena_stats(arena, (int)sizeof arena);
    return snprintf(buf, (size_t)n,
                    "{\"native_bytes\": %u, \"copied_bytes\": %u, \"exports\": %llu, \"selftest\": %s, \"arena\": %s}",
                    s_stats.native_bytes, s_stats.copied_bytes, s_stats.exports, st, arena);
}
