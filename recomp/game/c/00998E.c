#include "game.h"

/* Rand_Below - an unsigned random number in [0, range): the high word of the unsigned random word
 * times the range.
 * In:  value = the range (its low word)
 * Out: value = (random * range) >> 16, zero-extended
 * Evidence: move.w d0,-(a7) / bsr Rand_Next / mulu.w (a7)+,d0 / swap / andi.l #$FFFF; 23 callers
 *           (a2/a4 ops $ACC, $53D4, $5450 ..., Shake $95EC)
 */
void Rand_Below(uint32_t *value)
{
    uint16_t range = (uint16_t)*value;
    uint32_t product = (uint32_t)(uint16_t)Rand_Next() * range;
    *value = product >> 16;
}
