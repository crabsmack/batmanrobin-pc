#include "game.h"

/* the off-screen window ToyChain_SteerAtPlayer tests both ends of the chain against (screen x) */
enum { TOYCHAIN_OFF_LEFT = -100, TOYCHAIN_OFF_RIGHT = 0x1A4 };

/* the switch cases ToyChain_SteerAtPlayer returns in d0 (Op2_CallNativeRelSwitch in the chain
   body's coroutine 2) */
enum { TOYCHAIN_ON_SCREEN = 0, TOYCHAIN_MIRRORED = 1, TOYCHAIN_OFF_SCREEN = 2 };

/* ToyChain_SteerAtPlayer - switch native of a chain's steering coroutine: after a lost link the
 * sway speeds up by a quarter (once per lost link); while the targeted player is level with or
 * above the chain's tip the sway cruises toward the player's side; when the tip drops below the
 * base the chain is mirrored (drawn rotations + half a turn, sway negated, base moved to the tip);
 * otherwise reports whether both ends are off screen on the same side
 * In:  mot = the chain's motion block (accel_x, target_x, cruise_x), vm = its VM block (var 0,
 *      tracked = the targeted player record), chain = the chain object (chain block, sway, base)
 * Out: d0 = 0 on screen, 1 mirrored, 2 both ends left of screen x -100 or right of $1A4 (the
 *      case); d1 = the cruise speed when it steered, then the screen x of the left end; a1 = the
 *      player record when one is tracked
 * Evidence: eori #$10,$22(a4); asr.l #2 accel/cruise; record +$2E/+$32 vs tip +$30/+$32;
 *           eori.w #$200,$2E(a0) / neg.l $3E(a6); exg d0,d1 / sub.w g_CameraX; Op2_CallNativeRelSwitch
 *           $037A32 in coroutine 2 of the chain body (case 1: dust puffs $10D9A/$10DAC and sound
 *           $B; case 2: ToyChain_RemoveAllLinks and Op2_Kill); TAS f83917-f110830 (1319 frames)
 */
void ToyChain_SteerAtPlayer(uint32_t *d0, uint32_t *d1, uint32_t *a1, Vm2Motion *mot, Vm2Block *vm, ToyChainObj *chain)
{
    uint16_t lost = vm->var[0] & TOYCHAIN_LOST_LINK;
    *d0 = (*d0 & 0xFFFF0000u) | lost;
    if (lost) {
        vm->var[0] ^= TOYCHAIN_LOST_LINK;
        int32_t accel = mot->accel_x;
        mot->accel_x = (int32_t)((uint32_t)accel + (uint32_t)(accel >> 2));
        int32_t cruise = mot->cruise_x;
        cruise = (int32_t)((uint32_t)cruise + (uint32_t)(cruise >> 2));
        mot->cruise_x = cruise;
        *d0 = (uint32_t)cruise;
        /* a running sway keeps its direction at the new speed; a stopped one stays stopped */
        if (mot->target_x != 0) {
            if (mot->target_x < 0)
                *d0 = 0u - *d0;
            mot->target_x = (int32_t)*d0;
        }
    }

    ToyChainBlock *blk = dk::handle<ToyChainBlock>(chain->chain_block);
    int16_t tracked = vm->tracked;
    *d0 = (*d0 & 0xFFFF0000u) | (uint16_t)tracked;
    if (tracked != 0) {
        *a1 = (uint32_t)(int32_t)tracked;   /* movea.w: the handle, sign-extended */
        const PlayerRec *rec = dk::handle<const PlayerRec>(tracked);
        *d0 = (*d0 & 0xFFFF0000u) | (uint16_t)rec->y;
        if (rec->y <= blk->tip_y) {
            /* the player is level with or above the tip: sway toward the player's side */
            *d1 = (uint32_t)(int32_t)mot->cruise_x;
            *d0 = (*d0 & 0xFFFF0000u) | (uint16_t)rec->x;
            if (rec->x <= blk->tip_x)
                *d1 = 0u - *d1;
            mot->target_x = (int32_t)*d1;
        }
    }

    *d0 = (*d0 & 0xFFFF0000u) | (uint16_t)blk->tip_y;
    if (blk->tip_y > chain->base_y) {
        /* the tip swung below the base: flip the chain over, the tip becomes the base */
        blk->frame_turn ^= 0x200;
        chain->sway = (int32_t)(0u - (uint32_t)chain->sway);
        chain->base_x = blk->tip_x;
        *d0 = TOYCHAIN_MIRRORED;
        return;
    }

    /* d0 = the right end, d1 = the left end (exg swaps the whole registers) */
    *d0 = (*d0 & 0xFFFF0000u) | (uint16_t)chain->base_x;
    *d1 = (*d1 & 0xFFFF0000u) | (uint16_t)blk->tip_x;
    if ((int16_t)*d0 < (int16_t)*d1) {
        uint32_t t = *d0;
        *d0 = *d1;
        *d1 = t;
    }
    uint16_t camera_x = (uint16_t)(g_CameraX >> 16);
    *d0 = (*d0 & 0xFFFF0000u) | (uint16_t)((uint16_t)*d0 - camera_x);
    *d1 = (*d1 & 0xFFFF0000u) | (uint16_t)((uint16_t)*d1 - camera_x);
    /* native: the kill window widened to the view on each side (the gameplay mode, R4, R20) */
    if ((int16_t)*d0 < TOYCHAIN_OFF_LEFT - (int16_t)WS_WIN_L() || (int16_t)*d1 > TOYCHAIN_OFF_RIGHT + (int16_t)WS_WIN_R())
        *d0 = TOYCHAIN_OFF_SCREEN;
    else
        *d0 = TOYCHAIN_ON_SCREEN;
}
