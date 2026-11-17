/* types/VM6.h - the a6 loader scripts (docs/NOTES.md "a6 loader scripts"): one global script at a
 * time (g_A6ScriptPc), its ops' buffer operands and the resume hooks Frame_End runs. An operand
 * naming a buffer is a byte offset into g_BufPtrs (16 longs); a buffer starts with self-relative
 * longs to its entries, named by a word "entry offset" operand. */
#pragma once
#include "dk_mem.h"

/* the next word of an a6 script (an operand or a handler word) */
static inline uint16_t vm6_word(const u16 **script)
{
    return (uint16_t)*(*script)++;
}

/* the next long of an a6 script (a routine or an LZ block address) */
static inline uint32_t vm6_long(const u16 **script)
{
    uint32_t hi = vm6_word(script);
    return hi << 16 | vm6_word(script);
}

/* the g_BufPtrs slot a word operand names (adda.w: the offset sign-extends) */
static inline PTR(u8) *vm6_slot(uint16_t slot_off)
{
    return (PTR(u8) *)(g_BufPtrs + (int16_t)slot_off);
}

/* an entry of a buffer: the entry offset's long, plus that long (`adda.w (a6)+,a0 / adda.l
   (a0),a0`), as a 68000 address */
static inline uint32_t vm6_entry(uint32_t buf, uint16_t entry_off)
{
    PTR(const u32) at;
    at.v = buf + (uint32_t)(int32_t)(int16_t)entry_off;
    return at.raw() + (uint32_t)*at;
}

/* the VDP VRAM-write command of a byte address (`lsl.l #2 / lsr.w #2 / ori.w #$4000 / swap`): the
   whole register as the original computes it (bits above the address's 16 kept in the low word) */
static inline uint32_t vm6_vram_write_cmd(uint32_t a)
{
    a <<= 2;
    a = (a & 0xFFFF0000u) | ((a & 0xFFFFu) >> 2);
    a |= 0x4000u;
    return a << 16 | a >> 16;
}

/* the next row's VDP command: the destination advanced by one plane row (`swap / rol.w #2 / add.l
   / ror.w #2 / swap`, `row` = the row bytes << 2) */
static inline uint32_t vm6_next_row_cmd(uint32_t cmd, uint32_t row)
{
    cmd = cmd << 16 | cmd >> 16;
    uint16_t lo = (uint16_t)cmd;
    lo = (uint16_t)(lo << 2 | lo >> 14);
    cmd = (cmd & 0xFFFF0000u) | lo;
    cmd += row;
    lo = (uint16_t)cmd;
    lo = (uint16_t)(lo >> 2 | lo << 14);
    cmd = (cmd & 0xFFFF0000u) | lo;
    return cmd << 16 | cmd >> 16;
}
