#include "game.h"

/* the finger's 3 points (x, y, z words, then the long copied as it is; ROM data, batmanrobin.asm) */
static constexpr uint32_t Tbl_ClawFingerPoints = 0x01EC86u;
static constexpr int CLAW_FINGER_POINTS = 3;
static constexpr int16_t CLAW_FINGER_DROP = 0x10E;   /* the fingers hang this far below the frame's origin */

/* one entry of a finger's point list (10 bytes, read by Boss_Claw_DrawPoints) */
typedef struct ClawPoint {
    s16 x, y, z;
    u32 look;                      /* +6: size word, attribute word */
} ClawPoint;
static_assert(sizeof(ClawPoint) == 10, "ClawPoint");

/* Boss_Claw_PlaceFinger - builds the finger frame's matrix and transforms the 3 points of a finger
 * (Tbl_ClawFingerPoints) into its point list as 10-byte entries (x, y + $10E, z, size word, attr
 * word) for Boss_Claw_DrawPoints
 * In:  frame = g_Rot3DInner (a0); list = the finger's point list (a5)
 * Out: the list's 3 entries; a1 past the 3 source points; d4/d5/d7 = the last point (d5 + $10E);
 *      d6 = $FFFF (the count's end); d0-d3 as Rot3D_TransformPoint left them
 * Evidence: jsr Rot3D_BuildMatrix / lea Tbl_ClawFingerPoints,a1 / moveq #2,d6 / 3 x (jsr
 *           Rot3D_TransformPoint / addi.w #$10E,d5 / move.w d4,(a5)+ / move.w d5,(a5)+ / move.w
 *           d7,(a5)+ / move.l (a1)+,(a5)+) / dbra; called 3 times per Boss_Claw_PlaceFingers
 */
void Boss_Claw_PlaceFinger(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5,
                           uint32_t *d6, uint32_t *d7, Rot3D *frame, uint32_t *a1, uint32_t list)
{
    uint32_t a2 = 0, a3 = 0, a4 = 0;   /* the matrix build's scratch (clobbers here) */
    Rot3D_BuildMatrix(d0, d1, d2, d3, frame, &a2, &a3, &a4);
    PTR(const s16) src;
    src.v = Tbl_ClawFingerPoints;
    const s16 *point = src;
    PTR(ClawPoint) dst;
    dst.v = list;
    ClawPoint *out = dst;
    for (int i = 0; i < CLAW_FINGER_POINTS; i++) {
        Rot3D_TransformPoint(d0, d1, d4, d5, d7, frame, &point);
        *d5 = (*d5 & 0xFFFF0000u) | (uint16_t)(*d5 + CLAW_FINGER_DROP);
        out->x = (int16_t)*d4;
        out->y = (int16_t)*d5;
        out->z = (int16_t)*d7;
        out->look = *(const u32 *)point;   /* the long after the point's 3 words */
        point += 2;
        out++;
    }
    *d6 = 0xFFFF;                      /* moveq #2,d6 / dbra: the count ends at -1 (word) */
    *a1 = ADDR68(point);
}
