#include "game.h"

/* the arc projectile's a4 effect script (ROM data; batmanrobin.asm) */
static constexpr uint32_t Scr4_01AB76 = 0x01AB76u;

static constexpr int16_t HARLEY_LOB_RISE = 0x41;   /* the lob starts this far above the drawn position */

/* Boss_Harley_LobAtPlayer - Harley mid-boss native: lobs an arc projectile (a4 script Scr4_01AB76)
 * from the pole top toward the targeted player; does nothing with no target
 * In:  vm = the Harley object's VM block (a4; tracked = the targeted player record or 0);
 *      self = the Harley object (a6); the rest pass through to Fx_RunScript
 * Out: d0 low word = the tracked handle; with a target: g_ArcTargetX/Y = the player's x/y, then
 *      Fx_RunScript with a0 = Scr4_01AB76, d4/d5 = the drawn position (y - $41) as 16.16, d2/d3 = 0
 * Evidence: move.w $10(a4),d0 / beq rts / movea.w d0,a0 / move.w $2E(a0),g_ArcTargetX /
 *           move.w $32(a0),g_ArcTargetY / lea Scr4_01AB76,a0 / moveq #0 d2-d5 / move.w $4E(a6),d4 /
 *           move.w $50(a6),d5 / subi.w #$41,d5 / swap d4 / swap d5 / jmp Fx_RunScript; called by op
 *           $2CB2 (CallNativeRel) at $19BAC in the Harley script; TAS f5765-f5930 (4 frames). a0 an
 *           output (measured as a clobber): Fx_RunScript, jumped to, takes the script in it
 */
void Boss_Harley_LobAtPlayer(uint32_t *d0, uint32_t d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5,
                             uint32_t d6, uint32_t d7, uint32_t *a0, uint32_t a1, uint32_t a2, uint32_t a3,
                             const Vm2Block *vm, uint32_t a5, const HarleyObj *self)
{
    uint16_t tracked = (uint16_t)vm->tracked;
    *d0 = (*d0 & 0xFFFF0000u) | tracked;
    if (tracked == 0)
        return;
    const PlayerRec *player = dk::handle<const PlayerRec>((int16_t)tracked);
    g_ArcTargetX = (uint16_t)player->x;
    g_ArcTargetY = (uint16_t)player->y;
    *a0 = Scr4_01AB76;
    *d2 = 0;
    *d3 = 0;
    *d4 = (uint32_t)(uint16_t)self->drawn_x << 16;
    *d5 = (uint32_t)(uint16_t)(self->drawn_y - HARLEY_LOB_RISE) << 16;
    dk::jump(at_Fx_RunScript);
}
