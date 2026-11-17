#include "game.h"

/* Rand_Signed - a signed random number in [-range/2, range/2): the high word of the signed random
 * word times the range.
 * In:  value = the range (its low word)
 * Out: value = (random * range) >> 16, sign-extended to a long
 * Evidence: move.w d0,-(a7) / bsr Rand_Next / muls.w (a7)+,d0 / swap / ext.l; 5 callers ($1736,
 *           $3E58, $10D12, $13F5E, $2417E)
 */
void Rand_Signed(uint32_t *value)
{
    int16_t range = (int16_t)(uint16_t)*value;
    int32_t product = (int32_t)(int16_t)Rand_Next() * range;
    *value = (uint32_t)(int32_t)(int16_t)(uint16_t)((uint32_t)product >> 16);
}
