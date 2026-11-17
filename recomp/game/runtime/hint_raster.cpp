/* hint_raster.cpp - the game's RAM code, in C, for the native build (phase-4 ruling 6: "phase 5
 * makes the handler a raster callback"; phase-5 ruling R3).
 *
 * The H-int vector is $FFFFE560: work RAM the game writes code into (docs/NOTES.md "RAM code";
 * dkit.toml [recomp] ram_code). Two shapes, recognised as the engine's RAM code overlays are:
 * every byte as its template except the live ones, which the code reads when it runs.
 *
 *   A, the per-line V-scroll raster ($00B1B6 copies Tbl_HIntHandlerSrc $00B20C; live: the
 *      move's source word $FFE562 and the bcs.s displacement $FFE56D):
 *        FFE560  move.w ($XXXX).w,($C00000).l   a word of the line table to the VDP data port
 *        FFE568  addq.b #2,($E563).w            the table's next word
 *        FFE56C  bcs.s  disp                    a 256-byte page done: to FFE56E + disp
 *        FFE56E  rte
 *        FFE570  addq.b #1,($E562).w            disp $02: the next page,
 *        FFE574  move.b #$0E,($E56D).w          and the next page end stops the raster
 *        FFE57A  rte
 *        FFE57C  move.w #$8004,($C00004).l      disp $0E: VDP register 0 = $04 (H-int off)
 *        FFE584  rte
 *   B, a jmp stub (`move.w #$4EF9,$FFE560` + `move.l #target,$FFE562` at $01C450, $024506,
 *      $0245B6): jmp target, a C H-int handler (01C468, 024526, 0245E0) through the flow table.
 *
 * Nothing is decoded: anything else at $FFE560 is fatal, as the engine's overlay check would
 * send it to its interpreter. The handler touches no register; its RTE restores SR and returns
 * where the H-int came (the flow runtime checks).
 */
#include "game.h"
#include "platform.h"
#include "runtime/dk_flow_rt.h"

static const uint32_t HINT_RAM = 0xFFFFE560u;   /* the vector's RAM code */
static const uint32_t HINT_SRC = 0x00B20Cu;     /* Tbl_HIntHandlerSrc: shape A's template */
static const unsigned HINT_LEN = 0x26u;

/* Game memory through dk_mem.h's accessors at the widths the code uses (the memory layer decides
   where work RAM lives: the emulated RAM, or the native arena of ruling R1). */
static u8 &byte_at(uint32_t a) { return *AT68(u8, a); }
static uint16_t word_at(uint32_t a) { return *AT68(const u16, a); }

extern "C" int dk_ram_code(uint32_t addr)
{
    if (addr != (HINT_RAM & 0xFFFFFFu))
        return 0;
    if (word_at(HINT_RAM) == 0x4EF9u) {   /* B: jmp (xxx).l */
        dk::jump((uint32_t)*AT68(const u32, HINT_RAM + 2u));
        return 1;
    }
    for (unsigned i = 0; i < HINT_LEN; i++) {   /* A: the template, but for the live bytes */
        bool live = i == 2 || i == 3 || i == 0xD;
        uint8_t have = byte_at(HINT_RAM + i), want = (uint8_t)*AT68(const u8, HINT_SRC + i);
        if (!live && have != want)
            dk::fatal("hint_raster: RAM code at $FFE560 matches no known handler (byte $%02X at +$%02X, the raster "
                      "template has $%02X)", have, i, want);
    }
    /* move.w ($XXXX).w,($C00000).l: the absolute short operand, sign-extended (ROM below $8000) */
    uint32_t src = (uint32_t)(int32_t)(int16_t)word_at(HINT_RAM + 2u);
    hw::port_write16(hw::VDP_DATA, word_at(src));
    unsigned lo = byte_at(HINT_RAM + 3u) + 2u;          /* addq.b #2,($E563).w */
    byte_at(HINT_RAM + 3u) = (uint8_t)lo;
    if (lo > 0xFFu) {                                    /* bcs.s: a carry */
        uint8_t disp = byte_at(HINT_RAM + 0xDu);
        uint32_t to = 0xFFE56Eu + (uint32_t)(int32_t)(int8_t)disp;
        if (to == 0xFFE570u) {
            byte_at(HINT_RAM + 2u) = (uint8_t)(byte_at(HINT_RAM + 2u) + 1u);   /* addq.b #1,($E562).w */
            byte_at(HINT_RAM + 0xDu) = 0x0E;                                  /* move.b #$0E,($E56D).w */
        } else if (to == 0xFFE57Cu) {
            hw::port_write16(hw::VDP_CTRL, 0x8004u);                          /* VDP reg 0 = $04 */
        } else {
            dk::fatal("hint_raster: the raster's bcs.s goes to $%06X (displacement $%02X): no instruction starts there",
                      (unsigned)to, disp);
        }
    }
    dk::rte();
    return 1;
}
