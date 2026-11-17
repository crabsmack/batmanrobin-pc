#include "game.h"

/* TextMap_ResumeTyping - typewriter step of TextMap_Run: adds the speed to the accumulator; a carry
 * resumes the saved text run one character on, else the caller's a1-a6 come back and it returns.
 * In:  g_TextMapTypeSpeed / g_TextMapTypeAcc; the run saved by TextMap_PutChars (g_TextMapTypeSrc,
 *      g_TextMapTypeCount, g_TextMapTypeDst); the six longs TextMap_Run pushed
 * Out: d0 low word = the speed; g_TextMapTypeAcc += it, the add's condition codes; a carry: list /
 *      count / dst = the saved run, then TextMap_PutCharsNext (the dbra step); no carry: a1-a6
 *      popped, return to TextMap_Run's caller
 * Evidence: move.w (g_TextMapTypeSpeed).w,d0 / add.w d0,(g_TextMapTypeAcc).w / bcs / movem.l
 *           (a7)+,a1-a6 / rts; movea.l $F3B6,a6 / movea.l $F3C0,a0 / move.w $F3BE,d7 / bra $1442E;
 *           the ending texts (scenario-ending)
 */
void TextMap_ResumeTyping(uint32_t *d0, uint16_t *d6, uint16_t *count, const u8 **list, uint32_t *a1, uint32_t *a2,
                          uint32_t *a3, uint32_t *a4, uint32_t *a5, uint32_t *dst, dk::Flags *ccr)
{
    uint16_t speed = g_TextMapTypeSpeed;
    *d0 = (*d0 & 0xFFFF0000u) | speed;
    uint16_t acc = g_TextMapTypeAcc;
    uint32_t sum = (uint32_t)acc + speed;
    uint16_t r = (uint16_t)sum;
    g_TextMapTypeAcc = r;
    bool carry = sum > 0xFFFFu;
    bool overflow = ((~(acc ^ speed) & (acc ^ r)) & 0x8000u) != 0;
    *ccr = dk::Flags{ carry, (r & 0x8000u) != 0, r == 0, overflow, carry };
    if (carry) {
        *dst = g_TextMapTypeDst;
        PTR(const u8) src;
        src.v = g_TextMapTypeSrc;
        *list = src;
        *count = g_TextMapTypeCount;
        dk::jump(at_TextMap_PutCharsNext);
        return;
    }
    *a1 = dk::pop_long();                    /* movem.l (a7)+,a1-a6 */
    *a2 = dk::pop_long();
    *a3 = dk::pop_long();
    *a4 = dk::pop_long();
    *a5 = dk::pop_long();
    *dst = dk::pop_long();
}
