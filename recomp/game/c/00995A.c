#include "game.h"

/* Rand_Next - steps the game's pseudo-random generator, a rotate/xor mix of a word and a long of
 * state (both written back), and returns the new word sign-extended.
 * In:  g_RandState (word), g_RandState2 (long); seeded $79 / $004B00D3 by Main_Start.
 * Out: the new g_RandState as a signed long; the caller branches on its sign (tst(d0.l)).
 * Evidence: src/f/00995A.asm; Frame_End calls it once per frame, 70+ sites for random choices. */
int32_t Rand_Next(void)
{
    uint16_t s = g_RandState;
    uint32_t t = g_RandState2;

    t = (t << 1) | (t >> 31);
    t ^= s;                                  /* the state word into the long's low half */
    t = (t << 16) | (t >> 16);
    s ^= (uint16_t)t;
    s = (uint16_t)((s << 1) | (s >> 15));
    t ^= s;
    t = (t << 16) | (t >> 16);
    s ^= (uint16_t)t;
    g_RandState2 = t;
    g_RandState = s;
    return (int16_t)s;
}
