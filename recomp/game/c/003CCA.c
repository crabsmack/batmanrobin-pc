#include "game.h"

/* Op2_SwitchAimDir - a2 op: a switch on the sprite's aim direction: a table of word offsets, one
 * per direction (each relative to itself), indexed by aim_dir / 4.
 * In:  script = the offset table; spr->aim_dir (direction x 4, Op2_AimDirAtPlayer)
 * Out: script = the chosen case; NEXT there
 * Evidence: move.w $1A(a5),d0 / lsr.w #1,d0 / andi.w #$FFFE,d0 / adda.w d0,a2 / adda.w (a2),a2 /
 *           NEXT; 14 corpus runs; $1C25A `3CCA 0012 0018 001E ...`
 */
void Op2_SwitchAimDir(const u16 **script, const Vm2Sprite *spr)
{
    uint16_t byte_index = (uint16_t)((spr->aim_dir >> 1) & 0xFFFE);   /* (aim_dir / 4) words */
    const u16 *entry = (const u16 *)((const u8 *)*script + (int16_t)byte_index);
    *script = (const u16 *)((const u8 *)entry + (int16_t)(uint16_t)*entry);
    dk::jump(dk::code((int16_t)(uint16_t)*(*script)++));
}
