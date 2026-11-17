#include "game.h"

/* The two number commands ($014440-$01447F): the word a long operand points at, written as up to
 * 5 decimal digits without leading zeros (the last digit always). */

/* the digit chain both share (loc_014450): four divu.w steps through TextMap_PutDigit, then the
   units digit; the font map's '0' is entry $10 (offset $20) */
static void put_number(uint32_t *d0, uint32_t *d1, uint16_t base, uint32_t font, uint32_t *dst)
{
    static const uint16_t divisors[4] = { 10000, 1000, 100, 10 };
    *d1 = 0;
    for (uint16_t divisor : divisors) {
        uint32_t quotient = *d0 / divisor, remainder = *d0 % divisor;   /* never over $FFFF: d0 <= $10000 */
        *d0 = (remainder << 16) | quotient;  /* divu.w: the remainder in the upper word */
        uint16_t seen = (uint16_t)*d1;
        TextMap_PutDigit(d0, &seen, base, font, dst);
        *d1 = (*d1 & 0xFFFF0000u) | seen;
    }
    PTR(const u16) map;
    map.v = font;
    uint16_t tile = (uint16_t)(map.get()[0x10 + (uint16_t)*d0] + base);
    *d0 = tile;                              /* the upper word is PutDigit's 0 */
    PTR(u16) out;
    out.v = *dst;
    *out.get() = tile;
    *dst += 2;
    dk::jump(at_TextMap_NextCmd);
}

/* TextMap_CmdNumberPlus1 - text-map command $96: writes the word a long operand points at, plus
 * one, as a decimal number (the 1-based setting numbers).
 * In:  list = the long; font / base / dst as TextMap_Run sets them
 * Out: list past the long, a1 = past the word; d0/d1 the digit chain's; dst advanced; then
 *      TextMap_NextCmd
 * Evidence: moveq #0,d0 / movea.l (a0)+,a1 / move.w (a1)+,d0 / addq.l #1,d0 / bra $14450; the options
 *           screen shows the music / sound test numbers $F60A/$F60C plus one; scenario-options
 */
void TextMap_CmdNumberPlus1(uint32_t *d0, uint32_t *d1, uint16_t *base, uint16_t *d7, const u8 **list,
                            uint32_t *value_at, uint32_t *a2, uint32_t *a3, uint32_t *a4, uint32_t *font,
                            uint32_t *dst)
{
    *value_at = *(const u32 *)*list;
    *list += 4;
    PTR(const u16) value;
    value.v = *value_at;
    *d0 = (uint32_t)(uint16_t)*value.get() + 1;
    *value_at += 2;
    put_number(d0, d1, *base, *font, dst);
}

/* TextMap_CmdNumber - text-map command $A0: writes the word a long operand points at as a decimal
 * number of up to 5 digits without leading zeros.
 * In:  list = the long; font / base / dst as TextMap_Run sets them
 * Out: list past the long, a1 = past the word; d0/d1 the digit chain's; dst advanced; then
 *      TextMap_NextCmd
 * Evidence: moveq #0,d0 / movea.l (a0)+,a1 / move.w (a1)+,d0, then divu #10000/#1000/#100/#10 through
 *           TextMap_PutDigit; computed entry in scenario-continue
 */
void TextMap_CmdNumber(uint32_t *d0, uint32_t *d1, uint16_t *base, uint16_t *d7, const u8 **list, uint32_t *value_at,
                       uint32_t *a2, uint32_t *a3, uint32_t *a4, uint32_t *font, uint32_t *dst)
{
    *value_at = *(const u32 *)*list;
    *list += 4;
    PTR(const u16) value;
    value.v = *value_at;
    *d0 = (uint16_t)*value.get();
    *value_at += 2;
    put_number(d0, d1, *base, *font, dst);
}
