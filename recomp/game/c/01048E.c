#include "game.h"

/* Player_LoseLife, the players' switch natives and the section-exit natives ($01048E-$010549). */

/* the sprite attribute bit for a mirrored (facing left) object: bit 3 of the attribute's high byte */
static constexpr uint16_t ATTR_FLIP_H = 0x0800;

/* Player_LoseLife - takes a life: with lives left, refills the health, halves the weapon level and
 * clears the rapid-charge countdown (d0 = 0); with none, zeroes lives and health and switches the
 * HUD to GAME OVER (d0 = 1).
 * In:  self = the player object (a6: +$1E record)
 * Out: d0 = 0 still playing / 1 out (moveq: its flags); a1 = HUD_PlayerOut's when out
 * Evidence: movea.l $1E(a6),a0 / subq.w #1,$6(a0) / bmi / move.w #$100,$4(a0) / lsr.w $2A(a0) /
 *           clr.w $26(a0) / moveq #0,d0 / clr.w $6(a0) / clr.w $4(a0) / bsr HUD_PlayerOut / moveq
 *           #1,d0; bsr from Player_HurtFixed, dc.l after op $2D40 ($4B15C, $4CFDC, $4FB18 ...)
 */
void Player_LoseLife(uint32_t *d0, uint32_t *a1, const Vm2Obj *self, dk::Flags *ccr)
{
    PTR(PlayerRec) rec;
    rec.v = self->player;
    rec->lives = (uint16_t)(rec->lives - 1);
    if ((int16_t)(uint16_t)rec->lives >= 0) {
        rec->health = 0x100;
        rec->weapon_level = (uint16_t)((uint16_t)rec->weapon_level >> 1);
        rec->rapid_charge = 0;
        *d0 = 0;
    } else {
        rec->lives = 0;
        rec->health = 0;
        *a1 = HUD_PlayerOut(rec);
        *d0 = 1;
    }
    *ccr = dk::Flags::test(*d0, 4, ccr->x);
}

/* Player_CaseSprVar26 - a2 switch native: the sprite block's variable 2 (+$26, set by the script)
 * as the switch case.
 * In:  spr = the sprite block (a5)
 * Out: d0 = the case
 * Evidence: move.w $26(a5),d0 / rts; dc.l after op $2D40 at 22 sites $4B91E-$4B9DE (Batman)
 */
uint16_t Player_CaseSprVar26(const Vm2Sprite *spr)
{
    return (uint16_t)spr->var[2];
}

/* one stepping cycle: the counter (sprite variable 0) += 4 modulo `period`, case = counter / 16 +
 * `first` */
static uint16_t step_cycle(Vm2Sprite *spr, uint16_t period, uint16_t first)
{
    uint16_t c = (uint16_t)((uint16_t)spr->var[0] + 4);
    if ((int16_t)c >= (int16_t)period)
        c = (uint16_t)(c - period);
    spr->var[0] = (int16_t)c;
    return (uint16_t)((c >> 4) + first);
}

/* Player_CaseStepFrame - a2 switch native: the stepping animation case: 0 when none of bits $380
 * of script variable 1 is set; 1-6 cycling (mod $60) for bit $100 facing left or $200 facing right
 * (stepping forward, inferred), else 7-11 (mod $50).
 * In:  vm = the script block (a4: variable 1); spr = the sprite block (a5: +$38 facing, var 0)
 * Out: d0 = the case
 * Evidence: andi.w #$380 / btst #3,$38(a5) / andi.w #$100 or #$200 / the counter +4, cmpi.w #$60 or
 *           #$50, lsr.w #4, +1 or +7; dc.l after op $2D40 at 18 sites ($4C5CC ... $4F108)
 */
uint16_t Player_CaseStepFrame(const Vm2Block *vm, Vm2Sprite *spr)
{
    uint16_t bits = (uint16_t)vm->var[1] & 0x380;
    if (!bits)
        return 0;
    uint16_t forward = (spr->attr & ATTR_FLIP_H) ? 0x100 : 0x200;
    if (bits & forward)
        return step_cycle(spr, 0x60, 1);
    return step_cycle(spr, 0x50, 7);
}

/* Player_CaseCycle5 - a2 switch native: steps sprite variable 1 through 0-9 and returns it halved:
 * a 5-case cycle that changes every 2 calls.
 * In:  spr = the sprite block (a5)
 * Out: d0 = 0-4 (the wrap's moveq clears the upper word)
 * Evidence: move.w $24(a5),d0 / addq.w #1 / cmpi.w #$A / blt / moveq #0,d0 / move.w d0,$24(a5) /
 *           lsr.w #1,d0; dc.l after op $2D40 at $4D5A0 and $500DC
 */
void Player_CaseCycle5(uint32_t *d0, Vm2Sprite *spr)
{
    uint16_t c = (uint16_t)((uint16_t)spr->var[1] + 1);
    *d0 = (*d0 & 0xFFFF0000u) | c;
    if ((int16_t)c >= 0xA)
        *d0 = 0;
    spr->var[1] = (int16_t)(uint16_t)*d0;
    *d0 = (*d0 & 0xFFFF0000u) | (uint16_t)((uint16_t)*d0 >> 1);
}

/* The common tail of the section-exit natives ($010534): g_PlayMode bit $800 and
 * Player_SetExitFlag for player 1, then the jump into it for player 2 (a0 handed on). */
static void start_exit(uint32_t *a0, dk::Flags *ccr)
{
    g_PlayMode |= 0x800;
    Player_SetExitFlag((PlayerRec *)g_Player1, ccr);
    *a0 = dk::addr(g_Player2);
    dk::jump(at_Player_SetExitFlag);
}

/* Section_StartExit - a2 native: starts the end of a section with the exit timer at $82.
 * In:  none
 * Out: a0 = g_Player2; then Player_SetExitFlag's
 * Evidence: move.w #$82,(g_ExitTimer).w / bra.s $010534; dc.l after op $2CBC in $2964E, $3BB80 and
 *           other natives ($1D318, $343DA ...)
 */
void Section_StartExit(uint32_t *a0, dk::Flags *ccr)
{
    g_ExitTimer = 0x82;
    start_exit(a0, ccr);
}

/* Section_StartExitN - a2 native: starts the end of a section with the exit timer from the script
 * word: g_PlayMode bit $800 and, for each player in play, script variable 1 bit $800 and the frame
 * callback SceneCb_WaitPlayersExit (Player_SetExitFlag).
 * In:  script = the operand (a2: word timer)
 * Out: script past it; a0 = g_Player2; then Player_SetExitFlag's
 * Evidence: move.w (a2)+,(g_ExitTimer).w / ori.w #$800,(g_PlayMode).w / lea g_Player1,a0 / bsr
 *           Player_SetExitFlag / lea g_Player2,a0 and the fall into Player_SetExitFlag; dc.l after
 *           op $2CBC at $19A7C, $1D8BC, $1F01E ...
 */
void Section_StartExitN(uint32_t *a0, const u16 **script, dk::Flags *ccr)
{
    g_ExitTimer = *(*script)++;
    start_exit(a0, ccr);
}
