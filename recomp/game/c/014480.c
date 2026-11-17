#include "game.h"

/* TextMap_PutDigit - writes the quotient digit of a divu.w as a font tile, unless it and every
 * digit before it were 0 (no leading zeros); then the remainder becomes the next dividend.
 * In:  d0 = remainder:quotient (the divu's), seen = the digits so far OR'ed; base / font / dst as
 *      TextMap_Run sets them
 * Out: seen |= the digit; dst advanced when written; d0 = the remainder (a long)
 * Evidence: or.w d0,d1 / beq / add.w d0,d0 / move.w $20(a5,d0.w),d0 / add.w d6,d0 / move.w d0,(a6)+ /
 *           clr.w d0 / swap d0; the four divu steps of TextMap_CmdNumber
 */
void TextMap_PutDigit(uint32_t *d0, uint16_t *seen, uint16_t base, uint32_t font, uint32_t *dst)
{
    uint16_t digit = (uint16_t)*d0;
    *seen |= digit;
    if (*seen != 0) {
        PTR(const u16) map;
        map.v = font;
        PTR(u16) out;
        out.v = *dst;
        *out.get() = (uint16_t)(map.get()[0x10 + digit] + base);   /* the font map's '0' + digit */
        *dst += 2;
    }
    *d0 >>= 16;                              /* clr.w d0 / swap d0 (the low word's write is lost) */
}
