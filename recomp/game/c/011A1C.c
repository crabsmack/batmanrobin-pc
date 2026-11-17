#include "game.h"

/* The GO! sign's per-frame update and the boss meter's start ($011A1C-$011AED). */

static constexpr uint32_t GOSIGN_BRAKE = 0x1A00;     /* speed change a frame (16.16): braking in, speeding out */
static constexpr int32_t GOSIGN_LEAVE_TOP = 0x40000; /* 4.0 px a frame: leaving at this speed it is gone */
static constexpr uint16_t GOSIGN_FRAME_BASE = 0x2C88;
static constexpr uint16_t GOSIGN_ANIM_FRAMES = 0x20; /* 8 frames x 4 updates each */
static constexpr uint16_t GOSIGN_ANIM_STEP = 0xA;    /* sprite frames are $A apart */
static constexpr uint16_t GOSIGN_Y = 0x90;
static constexpr uint16_t GOSIGN_ATTR = 0x8000;      /* priority */
static constexpr uint32_t Scr2_011B3E = 0x011B3Eu;   /* the meter object's main script */

/* HUD_UpdateGoSign - the GO! sign every frame: state -1 moves it (entering it brakes to a stop,
 * which clears g_CameraLockX unless g_GoSignKeepLock; stopped, it waits for the camera to scroll
 * past g_GoSignCameraX, then HUD_GoSignLeave; leaving it speeds up until 4.0 px a frame, when the
 * sign is switched off and its sprite frame released), a positive state is the hold timer
 * (counted down by 2 into -1). While on it is drawn: frame $2C88 + $A per 4 updates (8 frames),
 * at x + $20, y $90.
 * In:  none (g_GoSignState: 0 off, -1 moving, > 0 holding)
 * Out: g_GoSign* updated; d1 = the frame (Spr_ChangeFrame), d4/d5/d6 = x/y/attr for
 *      Spr_DrawDirect (jumped to); the end of the leave jumps to Spr_ReleaseFrame (d2 = the frame)
 * Evidence: move.w (g_GoSignState).w,d0 / beq rts / bmi / subq.w #2 / move.l (g_GoSignSpeed).w,d0
 *           / bne / cmp.w (g_CameraX).w / bcc / bsr.s HUD_GoSignLeave / bpl / addi.l #$1A00 / bmi /
 *           moveq #0 / tst.b g_GoSignKeepLock / move.l d0,(g_CameraLockX).w; cmpi.l #$40000 blt /
 *           clr.w state / jmp Spr_ReleaseFrame; add.l d0,(g_GoSignX).w; the animation addq / cmpi.w
 *           #$20 / lsr.w #2 / mulu.w #$A / jsr Spr_ChangeFrame / beq / addi.w #$20,d4 / move.w
 *           #$90,d5 / move.w #$8000,d6 / jmp Spr_DrawDirect; called by HUD_Update every frame
 */
void HUD_UpdateGoSign(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5,
                      uint32_t *d6, uint32_t *d7, uint32_t *a0, uint32_t *a1, uint32_t *a2)
{
    (void)d7, (void)a2;
    uint16_t state = g_GoSignState;
    *d0 = (*d0 & 0xFFFF0000u) | state;
    if (state == 0)
        return;
    if ((int16_t)state > 0) {
        state = (uint16_t)(state - 2);
        *d0 = (*d0 & 0xFFFF0000u) | state;
        g_GoSignState = state;
    } else {
        *d0 = g_GoSignSpeed;
        bool move = true;
        if (*d0 == 0) {
            /* stopped: leave once the camera has scrolled past where it was at the start */
            *d0 = (*d0 & 0xFFFF0000u) | g_GoSignCameraX;
            if ((uint16_t)g_GoSignCameraX < (uint16_t)(g_CameraX >> 16))
                HUD_GoSignLeave();
            move = false;
        } else if ((int32_t)*d0 < 0) {
            *d0 += GOSIGN_BRAKE;
            if ((int32_t)*d0 >= 0) {   /* arrived */
                *d0 = 0;
                if (!g_GoSignKeepLock)
                    g_CameraLockX = 0;
            }
        } else {
            *d0 += GOSIGN_BRAKE;
            if ((int32_t)*d0 >= GOSIGN_LEAVE_TOP) {   /* gone */
                g_GoSignState = 0;
                *d2 = (*d2 & 0xFFFF0000u) | g_GoSignFrame;
                g_GoSignFrame = 0;
                dk::jump(at_Spr_ReleaseFrame);
                return;
            }
        }
        if (move) {
            g_GoSignSpeed = *d0;
            g_GoSignX = (uint32_t)g_GoSignX + *d0;
        }
    }

    *d1 = (*d1 & 0xFFFF0000u) | GOSIGN_FRAME_BASE;
    uint16_t anim = (uint16_t)(g_GoSignAnim + 1);
    if ((int16_t)anim >= (int16_t)GOSIGN_ANIM_FRAMES)
        anim = (uint16_t)(anim - GOSIGN_ANIM_FRAMES);
    g_GoSignAnim = anim;
    *d0 = (uint32_t)(uint16_t)(anim >> 2) * GOSIGN_ANIM_STEP;   /* mulu.w: the whole register */
    *d1 = (*d1 & 0xFFFF0000u) | (uint16_t)(*d1 + *d0);
    *d2 = (*d2 & 0xFFFF0000u) | g_GoSignFrame;
    Spr_ChangeFrame(d0, d1, d2, d3, a0, a1);
    g_GoSignFrame = (uint16_t)*d1;
    if ((uint16_t)*d1 == 0)
        return;
    *d4 = (*d4 & 0xFFFF0000u) | (uint16_t)((g_GoSignX >> 16) + 0x20);
    *d5 = (*d5 & 0xFFFF0000u) | GOSIGN_Y;
    *d6 = (*d6 & 0xFFFF0000u) | GOSIGN_ATTR;
#if DK_MODE_NATIVE
    ws_hud_direct_begin(WS_HUD_SIGN);   /* phase 6 (E3): Spr_DrawDirect tags the sign's entries (ws_draw.h) */
#endif
    dk::jump(at_Spr_DrawDirect);
}

/* BossMeter_Show - native callback for a boss: the meter's full and current energy = the boss's
 * energy; restarts the meter object's script when it exists, else spawns it (BossMeter_Spawn).
 * In:  a6 = the boss object (+$52 energy); d1/d4/d5/d7/a4 as BossMeter_Spawn takes them
 * Out: g_BossEnergyMax = g_BossEnergy = the energy; d0 = 0 (tst.l: the flags), or as
 *      BossMeter_Spawn
 * Evidence: move.w $52(a6),d0 / move.w d0,(g_BossEnergyMax).w / move.w d0,(g_BossEnergy).w /
 *           move.w (g_BossMeterObj).w,d0 / beq.s BossMeter_Spawn / movea.w d0,a0 / movea.w $A(a0),a0
 *           / move.l #$11B3E,$2A(a0) / moveq #0,d0; op $2CBC pointers in 12 boss scripts
 */
void BossMeter_Show(uint32_t *d0, uint16_t *d1, uint16_t d4, uint16_t d5, uint32_t *d7, uint32_t a4, uint32_t a6)
{
    (void)d1, (void)d4, (void)d5, (void)d7, (void)a4;
    PTR(const Vm2Obj) boss;
    boss.v = a6;
    uint16_t energy = (uint16_t)boss.get()->energy;
    *d0 = (*d0 & 0xFFFF0000u) | energy;
    g_BossEnergyMax = energy;
    g_BossEnergy = energy;
    uint16_t meter = g_BossMeterObj;
    *d0 = (*d0 & 0xFFFF0000u) | meter;
    if (meter == 0) {
        dk::jump(at_BossMeter_Spawn);
        return;
    }
    Vm2Obj *obj = dk::handle<Vm2Obj>((int16_t)meter);
    Vm2Block *vm = dk::handle<Vm2Block>(obj->vm);
    vm->resume.v = Scr2_011B3E;
    *d0 = 0;
}
