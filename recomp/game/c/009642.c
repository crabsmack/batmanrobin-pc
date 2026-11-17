#include "game.h"

enum {
    SHAKE2_REST = 0x8000,   /* the second oscillator stops once its speed is under 0.5 px at a crossing */
};

/* Shake_Update - steps the screen-shake bounce and the second oscillator, and sets the view
 * position: the camera, moved down by the shake.
 * In:  g_ShakeY/Vel/Gravity, g_Shake2Pos/Vel/Pull, g_CameraX/Y
 * Out: the bounce: speed -= gravity, y += speed; going above the top (y < 0) bounces it back down
 *      at 3/4 the speed from y = 0. g_ViewY = g_CameraY + g_ShakeY, g_ViewX = g_CameraX. The second
 *      oscillator is pulled toward 0 and loses a quarter of its speed at each crossing, stopping
 *      (pos = speed = 0) once that speed is within 0.5. d1 = the last position long, d2 = the last
 *      quarter taken off (unchanged when no crossing happened)
 * Evidence: move.l g_ShakeY,d0 / move.l g_ShakeVel,d1 / sub.l g_ShakeGravity,d1 / add.l d1,d0 / bpl /
 *           neg.l d1 / d2 = d1 asr 2 / sub.l d2,d1 / moveq #0,d0 / ... / the oscillator's bpl/bmi
 *           crossing tests, cmpi.l #$8000 / #-$8000; only caller Camera_UpdateView (and Shake_Rumble
 *           falling in)
 */
void Shake_Update(uint32_t *d1, uint32_t *d2)
{
    int32_t y = (int32_t)g_ShakeY;
    int32_t speed = (int32_t)((uint32_t)g_ShakeVel - (uint32_t)g_ShakeGravity);
    y = (int32_t)((uint32_t)y + (uint32_t)speed);
    if (y < 0) {   /* the bounce: back down at 3/4 the speed */
        speed = (int32_t)(0u - (uint32_t)speed);
        *d2 = (uint32_t)(speed >> 2);
        speed = (int32_t)((uint32_t)speed - *d2);
        y = 0;
    }
    g_ShakeY = (uint32_t)y;
    g_ShakeVel = (uint32_t)speed;
    g_ViewY = (uint32_t)y + (uint32_t)g_CameraY;
    g_ViewX = g_CameraX;

    /* the second oscillator: the pull points at 0; a step that crosses 0 damps the speed */
    int32_t vel = (int32_t)g_Shake2Vel, pos = (int32_t)g_Shake2Pos;
    bool crossed;
    if (pos >= 0) {
        vel = (int32_t)((uint32_t)vel - (uint32_t)g_Shake2Pull);
        pos = (int32_t)((uint32_t)pos + (uint32_t)vel);
        crossed = pos < 0;
    } else {
        vel = (int32_t)((uint32_t)vel + (uint32_t)g_Shake2Pull);
        pos = (int32_t)((uint32_t)pos + (uint32_t)vel);
        crossed = pos >= 0;
    }
    if (crossed) {
        *d2 = (uint32_t)(vel >> 2);
        vel = (int32_t)((uint32_t)vel - *d2);
        bool slow = vel >= 0 ? vel <= SHAKE2_REST : vel >= -SHAKE2_REST;
        if (slow) {   /* at rest */
            vel = 0;
            pos = 0;
        }
    }
    g_Shake2Vel = (uint32_t)vel;
    g_Shake2Pos = (uint32_t)pos;
    *d1 = (uint32_t)pos;
}
