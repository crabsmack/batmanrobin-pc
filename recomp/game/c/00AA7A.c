#include "game.h"

/* the condition codes of addq.b / subq.b #n on a byte (X = C) */
static dk::Flags flicker_addq_b(uint8_t a, uint8_t n)
{
    uint8_t r = (uint8_t)(a + n);
    bool c = a + n > 0xFF;
    bool v = ((~(a ^ n) & (a ^ r)) & 0x80) != 0;
    return dk::Flags{ c, (r & 0x80) != 0, r == 0, v, c };
}

static dk::Flags flicker_subq_b(uint8_t a, uint8_t n)
{
    dk::Flags f = dk::Flags::compare(a, n, 1);
    f.x = f.c;
    return f;
}

/* Fade_FlickerTarget - moves g_FadeTarget one step (2) up or down at random, bouncing off -2 and
 * +$C: a flickering brightness when a script calls it every frame.
 * In:  g_FadeTarget
 * Out: g_FadeTarget +-2; d0 = Rand_Next's result; ccr of the addq / subq
 * Evidence: bsr Rand_Next / bpl; negative: cmpi.b #$FE / ble add, else subq.b #2; positive:
 *           cmpi.b #$C / bge sub, else addq.b #2; the pointers at $02D314 and $049516 (a2 natives,
 *           the latter in a per-frame loop); 1 corpus run
 */
uint32_t Fade_FlickerTarget(dk::Flags *ccr)
{
    uint32_t r = (uint32_t)Rand_Next();
    int8_t target = (int8_t)g_FadeTarget;
    bool up = (int32_t)r < 0 ? target <= -2 : target < 0x0C;
    uint8_t old = g_FadeTarget;
    if (up) {
        g_FadeTarget = (uint8_t)(old + 2);
        *ccr = flicker_addq_b(old, 2);
    } else {
        g_FadeTarget = (uint8_t)(old - 2);
        *ccr = flicker_subq_b(old, 2);
    }
    return r;
}
