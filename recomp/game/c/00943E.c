#include "game.h"

/* $00943E-$009583: the camera follows the players: right only in x (toward the players' mean x,
 * or the lock point), both ways in y within 0..$A8 (one stage loop). Positions are 16.16. */

enum {
    TRACK_IN_PLAY = 1 << 0,          /* PlayerRec.flags bit 0 */
    TRACK_CENTRE_X = 0xA0 << 16,    /* the players are kept at the screen's centre x ($A0) */
    TRACK_SNAP_X = 0xC000,           /* closer than 3/4 px: the camera snaps there */
    TRACK_EASE_X = 0x3000,           /* the x speed changes by at most this per frame */
    TRACK_MAX_SPEED = 0x40000,       /* 4 px per frame */
    TRACK_CENTRE_Y = 0x70 << 16,    /* the players are kept $70 below the top */
    TRACK_LIMIT_Y = 0xA8 << 16,     /* the camera's lowest y */
    TRACK_MIN_SPEED_Y = 0x8000,      /* the y speed limit near an end: distance / 16 + 0.5 */
    TRACK_UNUSED_D4 = 0x4000,        /* loaded into d4 and never used */
};

/* a player's x / y as the 16.16 longs the record holds */
static uint32_t player_x(const PlayerRec *p)
{
    return (uint32_t)(uint16_t)p->x << 16 | (uint16_t)p->x_frac;
}

static uint32_t player_y(const PlayerRec *p)
{
    return (uint32_t)(uint16_t)p->y << 16 | (uint16_t)p->y_frac;
}

/* Camera_TrackPlayersX - per frame: scrolls the camera right toward the players' x (their mean
 * when both play) or the lock point g_CameraLockX, never left; then Camera_UpdateView.
 * In:  g_Player1/2 (+$25 bit 0 in play, +$2E x); g_CameraLockX; g_CameraSpeedX
 * Out: the distance (target - $A0 - camera) <= 0 or nobody in play: speed 0, no scroll; <= $C000:
 *      the camera snaps to the target (speed kept); else speed = 3/32 of the distance, eased toward
 *      by $3000 per frame, at most 4 px; camera x clamped at 0. d1 = the last speed / lock value.
 *      Then Camera_UpdateView
 * Evidence: btst #0,g_P1Flags / move.l $F67E,d0 / btst #0,g_P2Flags / lsr.l #1 both / add.l /
 *           g_CameraLockX override / subi.l #$A00000,d0 / sub.l g_CameraX,d0 / bgt / cmpi.l #$C000 /
 *           asr.l #3 / asr.l #2 / sub.l / the $3000 easing and the $40000 cap / add.l d0,g_CameraX /
 *           bpl / clr.l / bra.w Camera_UpdateView; the main gameplay loop $9BC0 every frame
 */
void Camera_TrackPlayersX(uint32_t *d1, uint32_t *d2)
{
    (void)d2;
    const PlayerRec *p1 = (const PlayerRec *)g_Player1, *p2 = (const PlayerRec *)g_Player2;
    uint32_t target;
    bool any = true;
    if (g_P1Flags & TRACK_IN_PLAY) {
        target = player_x(p1);
        if (g_P2Flags & TRACK_IN_PLAY) {
            *d1 = player_x(p2) >> 1;
            target = (target >> 1) + *d1;   /* the mean, each halved first */
        }
    } else {
        target = player_x(p2);
        any = (g_P2Flags & TRACK_IN_PLAY) != 0;
    }

    bool scroll = false;
    int32_t dist = 0;
    if (any) {
        *d1 = g_CameraLockX;
        if (*d1 != 0)
            target = *d1;
        uint32_t aim = target - TRACK_CENTRE_X;
        scroll = (int32_t)aim > (int32_t)(uint32_t)g_CameraX;   /* sub.l / bgt: the signed compare */
        dist = (int32_t)(aim - (uint32_t)g_CameraX);
    }
    if (!scroll) {
        g_CameraSpeedX = 0;
        dk::jump(at_Camera_UpdateView);
        return;
    }

    if (dist > TRACK_SNAP_X) {
        dist >>= 3;
        dist -= dist >> 2;   /* 3/32 of the distance */
        int32_t speed = (int32_t)g_CameraSpeedX;
        if (speed >= dist) {   /* slow down by at most $3000 */
            speed -= TRACK_EASE_X;
            if (speed <= dist)
                speed = dist;
        } else {               /* speed up by at most $3000, to at most 4 px */
            speed += TRACK_EASE_X;
            if (speed >= dist)
                speed = dist;
            if (speed >= TRACK_MAX_SPEED)
                speed = TRACK_MAX_SPEED;
        }
        *d1 = (uint32_t)speed;
        g_CameraSpeedX = (uint32_t)speed;
        dist = speed;
    }
    g_CameraX = (uint32_t)g_CameraX + (uint32_t)dist;
    if ((int32_t)(uint32_t)g_CameraX < 0)
        g_CameraX = 0;
    dk::jump(at_Camera_UpdateView);
}

/* Camera_TrackPlayersY - scrolls the camera vertically toward the players' y (their mean when both
 * play; player 2's when player 1 is out, in play or not) with the speed limited near the top (0)
 * and the bottom ($A8): at most the distance to that end / 16 + 0.5, and 4 px.
 * In:  g_Player1/2 (+$25 bit 0, +$32 y); g_CameraY, g_CameraSpeedY
 * Out: g_CameraSpeedY = the step; g_CameraY clamped to 0..$A8; d2 = the step, d3 = the new camera
 *      y, d4 = $4000 (unused), d5 = $A80000, d6 = the up limit (negated when the step is up), d7 =
 *      the down limit
 * Evidence: btst #0,g_P1Flags / move.l $F682,d0 / btst #0,g_P2Flags / lsr.l / add.l / subi.l
 *           #$700000 / d6 = d3 asr 4 + $8000, d7 = ($A80000 - d3) asr 4 + $8000, each capped at
 *           $40000 / sub.l d3,d0 / the clamp / add.l d2,d3 / bpl / cmp.l d5,d3; only caller the
 *           stage loop $33274. Player 1 out: the btst of g_P2Flags falls into the same code either
 *           way (the original's quirk, kept)
 */
void Camera_TrackPlayersY(uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *d7)
{
    const PlayerRec *p1 = (const PlayerRec *)g_Player1, *p2 = (const PlayerRec *)g_Player2;
    uint32_t target;
    if ((g_P1Flags & TRACK_IN_PLAY) && (g_P2Flags & TRACK_IN_PLAY))
        target = (player_y(p1) >> 1) + (player_y(p2) >> 1);
    else if (g_P1Flags & TRACK_IN_PLAY)
        target = player_y(p1);
    else
        target = player_y(p2);
    target -= TRACK_CENTRE_Y;

    *d3 = g_CameraY;
    *d4 = TRACK_UNUSED_D4;
    *d5 = TRACK_LIMIT_Y;
    int32_t up = ((int32_t)*d3 >> 4) + TRACK_MIN_SPEED_Y;
    int32_t down = ((int32_t)(*d5 - *d3) >> 4) + TRACK_MIN_SPEED_Y;
    if (up > TRACK_MAX_SPEED)
        up = TRACK_MAX_SPEED;
    if (down > TRACK_MAX_SPEED)
        down = TRACK_MAX_SPEED;

    int32_t step = (int32_t)(target - *d3);
    if (step >= 0) {
        if (step > down)
            step = down;
    } else {
        up = -up;
        if (step < up)
            step = up;
    }
    *d6 = (uint32_t)up;
    *d7 = (uint32_t)down;
    *d2 = (uint32_t)step;
    g_CameraSpeedY = (uint32_t)step;

    int32_t y = (int32_t)(*d3 + (uint32_t)step);
    if (y < 0)
        y = 0;
    else if (y > (int32_t)*d5)
        y = (int32_t)*d5;
    *d3 = (uint32_t)y;
    g_CameraY = (uint32_t)y;
}
