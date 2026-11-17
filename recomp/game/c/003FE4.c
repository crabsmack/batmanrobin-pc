#include "game.h"

/* Op2_AnimFrame - animation op: shows a frame: the sprite's draw list = the inline block after
 * the offset word (sprite ops ended by Op2_DrawEnd), the animation continues at the offset next
 * time, and the frame holds for the display time.
 * In:  script = word offset past the inline draw block (from the word itself); spr->anim_delay
 * Out: draw (a0) = the block; delay (d0) = spr->anim_delay; spr->draw; spr->anim_timer += delay;
 *      script = the target; ccr = the add.w's condition codes; return to Op2_RunAnim
 * Evidence: movea.l a2,a0 / adda.w (a0)+,a2 / move.l a0,$2A(a5) / move.w $1C(a5),d0 /
 *           add.w d0,$1E(a5) / rts; Op2_Draw runs $2A(a5); 1664 script uses, e.g. $1345E
 *           `3FE4 0008 4524 2A80 44F2`; 153 runs; verify compares the flags it returns with
 */
void Op2_AnimFrame(uint16_t *delay, uint32_t *draw, const u16 **script, Vm2Sprite *spr, dk::Flags *ccr)
{
    const u16 *offset = *script;
    *draw = ADDR68(offset + 1);
    *script = (const u16 *)((const u8 *)offset + (int16_t)(uint16_t)*offset);
    spr->draw = offset + 1;
    *delay = spr->anim_delay;
    uint16_t timer = spr->anim_timer;
    uint16_t sum = (uint16_t)(timer + *delay);
    spr->anim_timer = sum;
    bool carry = (uint32_t)timer + *delay > 0xFFFFu;
    /* add.w: N, Z of the sum, V a signed overflow, C (and X) the carry out */
    *ccr = dk::Flags{ carry, (sum & 0x8000) != 0, sum == 0, ((~(timer ^ *delay) & (timer ^ sum)) & 0x8000) != 0,
                      carry };
}
