#include "game.h"

enum {
    CAMERA_HALF_WIDTH = 0xA0,   /* the screen centre's offset from the camera (320 / 2) */
};

/* Camera_IsLocked - native callback for the switch op $2D40: 1 while g_CameraLockX holds a lock
 * point, else 0.
 * In:  none
 * Out: d0 = 1 or 0
 * Evidence: tst.l g_CameraLockX / beq / moveq #1,d0 / rts / moveq #0,d0 / rts; script uses after
 *           op $2D40 at $1E94E, $1EE8C
 */
uint32_t Camera_IsLocked(void)
{
    return g_CameraLockX != 0 ? 1 : 0;
}

/* Camera_LockHere - native callback: locks the camera where it is (the lock point is the screen
 * centre's x), so Camera_TrackPlayersX stops following the players (an arena).
 * In:  none
 * Out: g_CameraLockX = (camera x + $A0) << 16; d0 = 0 (a clobber)
 * Evidence: moveq #0,d0 / move.w g_CameraX,d0 / addi.w #$A0,d0 / swap d0 / move.l d0,g_CameraLockX /
 *           moveq #0,d0; callers the stage loops $32F14, $33274, $3FFBE, $401DA, $479EE, $47BD0
 */
void Camera_LockHere(void)
{
    uint16_t centre = (uint16_t)((g_CameraX >> 16) + CAMERA_HALF_WIDTH);
    g_CameraLockX = (uint32_t)centre << 16;
}

/* Camera_LockRightEdge - level trigger callback: locks the camera so it scrolls on until the
 * trigger's x is at the screen's right edge.
 * In:  d4 = the trigger's x (low word)
 * Out: d4 = g_CameraLockX = (x - $A0) << 16; d0 = 0 (a clobber): the moveq's flags
 * Evidence: subi.w #$A0,d4 / swap d4 / clr.w d4 / move.l d4,g_CameraLockX / moveq #0,d0; uses in the
 *           stage trigger tables ($16138...). Prototype: &ccr added (the moveq's Z, its ccout)
 */
void Camera_LockRightEdge(uint32_t *d4, dk::Flags *ccr)
{
    *d4 = (uint32_t)(uint16_t)((uint16_t)*d4 - CAMERA_HALF_WIDTH) << 16;
    g_CameraLockX = *d4;
    *ccr = dk::Flags::test(0, 4, ccr->x);
}
