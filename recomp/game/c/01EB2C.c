#include "game.h"

/* the chain head's depth (z' / 4096, halved) at which the claw is in the players' plane */
static constexpr int16_t CLAW_HIT_DEPTH_MIN = 0x700;
static constexpr int16_t CLAW_HIT_DEPTH_MAX = 0x800;
static constexpr int16_t CLAW_FRONT_DEPTH = 0x780;    /* nearer than this: drawn in front */
static constexpr uint16_t CLAW_SORT_FRONT = 0x4000;
static constexpr uint16_t CLAW_SORT_BACK = 0x1000;
static constexpr uint16_t CLAW_ATTR_FRONT = 0x8000;   /* high priority */

/* Boss_Claw_DrawChain - projects the chain head to set the claw's drawn position and its hittable
 * flag (depth $700-$800), picks front (sort $4000, priority) or back (sort $1000) by depth <= $780,
 * then draws d7+1 chain balls through Boss_Claw_DrawPoints' loop
 * In:  a1 = the point list (x, y, z, size, attr), d7 = balls - 1, a5 = the sprite slots, a6 = the claw
 * Out: the claw's drawn_x/y, hittable, sort, priority; g_UnkClaw_F6D2 = 0; d7's count pushed for
 *      the loop; d4/d5 = the head's position, d7 low word = its depth / 2; then as
 *      Boss_Claw_DrawProjected (the loop's rts returns to this function's caller)
 * Evidence: move.w d7,-(a7) / lea g_Rot3DOuter,a0 / jsr Rot3D_ProjectPoint / asr.w #1,d7 / add.w
 *           $3E/$42(a6) / move.w d4,$4E(a6) / move.w d5,$50(a6) / clr.w $2E(a6) / $700 <= d7 <= $800:
 *           addq.w #1,$2E(a6) / clr.w $F6D2 / cmpi.w #$780,d7 / bgt / $4000/$8000 or $1000/0 into
 *           $2A/$2C(a6) / bra loc_01EB9E (Boss_Claw_DrawProjected); 2349 TAS frames
 */
void Boss_Claw_DrawChain(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5,
                         uint16_t *d6, uint32_t *d7, uint32_t *a0, uint32_t *a1, uint32_t a2, uint32_t *a5,
                         ClawObj *claw)
{
    dk::push_word((uint16_t)*d7);      /* the ball count, popped by the loop's move.w (a7)+,d7 */
    Rot3D *view = (Rot3D *)g_Rot3DOuter;
    *a0 = ADDR68(view);
    PTR(const s16) list;
    list.v = *a1;
    const s16 *point = list;
    Rot3D_ProjectPoint(d0, d1, d4, d5, d7, view, &point);
    *a1 = ADDR68(point);
    int16_t depth = (int16_t)((int16_t)*d7 >> 1);
    *d7 = (*d7 & 0xFFFF0000u) | (uint16_t)depth;
    *d4 = (*d4 & 0xFFFF0000u) | (uint16_t)(*d4 + claw->x);
    *d5 = (*d5 & 0xFFFF0000u) | (uint16_t)(*d5 + claw->y);
    claw->drawn_x = (int16_t)*d4;
    claw->drawn_y = (int16_t)*d5;
    claw->hittable = 0;
    if (depth >= CLAW_HIT_DEPTH_MIN && depth <= CLAW_HIT_DEPTH_MAX)
        claw->hittable = 1;
    g_UnkClaw_F6D2 = 0;
    if (depth <= CLAW_FRONT_DEPTH) {
        claw->sort = CLAW_SORT_FRONT;
        claw->priority = CLAW_ATTR_FRONT;
    } else {
        claw->sort = CLAW_SORT_BACK;
        claw->priority = 0;
    }
    dk::jump(at_Boss_Claw_DrawProjected);
}
