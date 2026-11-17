/* dk_image.h - the 68000 image of the game state (phase 5: docs/phases/P5-port.md). Written by
 * the kit's recomp template; yours to extend.
 *
 * The native runner computes its sync rows from this image only (spec.c, DK_NATIVE): the
 * exporter (game/runtime/export.cpp) writes it at each sync point from wherever the state lives,
 * and the rows' CRCs are then computed exactly as phase 4's (stack area and [recomp] dont_care
 * masked, nothing else). Every array is in the reference's format (dkit genesis emu harness):
 * RAM and VRAM in 68000 byte order, CRAM 64 words & $0EEE big-endian, VSRAM 40 words & $07FF
 * big-endian, the Z80's 8 KB as it holds them.
 */
#ifndef DK_IMAGE_H
#define DK_IMAGE_H

#include <stddef.h>
#include <stdint.h>

#define DK_IMAGE_RAM_BYTES    0x10000u
#define DK_IMAGE_VRAM_BYTES   0x10000u
#define DK_IMAGE_CRAM_WORDS   64u
#define DK_IMAGE_VSRAM_WORDS  40u
#define DK_IMAGE_Z80_BYTES    0x2000u

typedef struct DkImage {
    uint8_t ram[DK_IMAGE_RAM_BYTES];               /* $FF0000-$FFFFFF */
    uint8_t vram[DK_IMAGE_VRAM_BYTES];
    uint8_t cram[2 * DK_IMAGE_CRAM_WORDS];
    uint8_t vsram[2 * DK_IMAGE_VSRAM_WORDS];
    uint8_t z80[DK_IMAGE_Z80_BYTES];
} DkImage;

#ifdef __cplusplus
extern "C" {
#endif
/* Write the image of the state as it is now (at a sync point). */
void dk_export(DkImage *im);
/* The exporter's counts as a JSON object (the runner's counters: "exported"): native_bytes and
   copied_bytes = the work-RAM bytes the export wrote from native storage and copied from the
   emulated 68000's RAM (each the most any export of the process took: a branch plan's process
   reports its whole run, which fails closed for each branch), exports, selftest. Returns the
   length written. */
int dk_export_stats(char *buf, int n);
#ifdef __cplusplus
}
#endif

#endif /* DK_IMAGE_H */
