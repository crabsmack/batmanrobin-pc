#include "game.h"

/* Op2_SetBlinkTimer - a2 op: sets the sprite's blink timer: while it runs Op2_Draw skips drawing
 * on alternate frames (the player's spawn invulnerability uses the same byte).
 * In:  script = word frames (the low byte used)
 * Out: frames (d0) = the word; spr->invulnerable; handler = NEXT's
 * Evidence: move.w (a2)+,d0 / move.b d0,$16(a5) / NEXT; Op2_Draw: subq.b #1,$16(a5) /
 *           tst.b g_FrameParity; $101CE sets $16(a5) = $78 on spawn, Hitbox_CanHitPlayer tests it;
 *           132 corpus runs; $132EA `4326 0005`
 */
void Op2_SetBlinkTimer(uint16_t *frames, uint32_t *handler, const u16 **script, Vm2Sprite *spr)
{
    *frames = *(*script)++;
    spr->invulnerable = (uint8_t)*frames;
    *handler = dk::code((int16_t)(uint16_t)*(*script)++);
    dk::jump(*handler);
}
