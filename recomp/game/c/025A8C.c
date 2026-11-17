#include "game.h"

/* the sparks' a4 effect script (batmanrobin.asm) */
static constexpr uint32_t Scr4_025AC0 = 0x025AC0u;

enum { POLETHUG_SPARK_SPEED = 0x8000 };   /* 0.5 px a frame (16.16): slower slides make no sparks */

/* swap: the 68000's halves exchanged */
static uint32_t PoleThug_Swap(uint32_t v)
{
    return (v << 16) | (v >> 16);
}

/* Enemy_PoleThug_SlideSparks - pole-thug native with two inline offset words: while it slides
 * down faster than 0.5 px a frame, spawns the spark effect Scr4_025AC0 at its position + (dx, dy),
 * dx mirrored when the sprite is flipped
 * In:  script = the inline dx, dy words (a2); mot = the thug's motion block (vel_y), spr = its
 *      sprite block (attr bit $800 = H flip), self = the thug
 * Out: script past the two words; sliding fast: d4/d5 = the spark's position, 16.16 (integer in
 *      the high word, the old high word below), a0 = Scr4_025AC0, then Fx_RunScript
 * Evidence: cmpi.l #$8000,$1C(a3) / bgt / addq.l #4,a2 / rts; move.w (a2)+,d4 / (a2)+,d5 /
 *           btst #3,$38(a5) / neg.w d4 / add.w $3E(a6),d4 / add.w $42(a6),d5 / swap / lea
 *           Scr4_025AC0,a0 / jmp Fx_RunScript; op $2CB2 at $025818 (dx $E, dy $18); TAS
 *           f21284-f29651 (69 frames). The prototype gains &a0 (measured a clobber) so the jump
 *           hands the effect script on to Fx_RunScript
 */
void Enemy_PoleThug_SlideSparks(uint32_t d0, uint32_t d1, uint32_t d2, uint32_t d3, uint32_t *d4, uint32_t *d5, uint32_t d6,
                                uint32_t d7, uint32_t *fx, uint32_t a1, const s16 **script, const Vm2Motion *mot, uint32_t a4,
                                const Vm2Sprite *spr, const Vm2Obj *self)
{
    if (mot->vel_y <= POLETHUG_SPARK_SPEED) {   /* bgt: signed */
        *script += 2;
        return;
    }
    int16_t dx = *(*script)++;
    int16_t dy = *(*script)++;
    if (spr->attr & 0x0800)   /* btst #3 of the attribute word's high byte: H flip */
        dx = (int16_t)-dx;
    *d4 = PoleThug_Swap((*d4 & 0xFFFF0000u) | (uint16_t)(dx + self->x));
    *d5 = PoleThug_Swap((*d5 & 0xFFFF0000u) | (uint16_t)(dy + self->y));
    *fx = Scr4_025AC0;
    dk::jump(at_Fx_RunScript);
}
