#include "game.h"

/* Op5_LockCameraHere - a5 op: locks the camera where it is (g_CameraLockX = the camera's x + $A0
 * as 16.16), so Camera_TrackPlayersX holds the view for an arena fight.
 * In:  a5 = script (no operands)
 * Out: d0 = the lock value; NEXT
 * Evidence: moveq #0,d0 / move.w (g_CameraX).w,d0 / addi.w #$A0,d0 / swap d0 / move.l
 *           d0,(g_CameraLockX).w / NEXT; 35 uses, e.g. $16B40 (`6E74 6ECE`); 9 corpus runs
 */
void Op5_LockCameraHere(uint32_t *d0, uint32_t *a0, uint32_t *a5)
{
    *d0 = (uint32_t)(uint16_t)((g_CameraX >> 16) + 0xA0) << 16;
    g_CameraLockX = *d0;
    vm5_next(a0, a5);
}
