#include "game.h"

/* the ball sprite frame for each size / depth step (32 words, ROM data at $01EBCE: indexed by
   twice the quotient, pc-relative, so nothing in the corpus logs it as read) */
static constexpr uint32_t Tbl_ClawBallFrames = 0x01EBCEu;

/* the loop shared by both entries: Boss_Claw_DrawPoints projects a point and falls into
 * Boss_Claw_DrawProjected, which draws it, pops the ball count the projection pushed and loops back
 * (dbra) to project the next one. `projected` = enter at the draw (the chain head, projected by
 * Boss_Claw_DrawChain). The count lives on the 68000 stack, as in the asm, so the calls in between
 * see the same stack. */
static void claw_draw_balls(bool projected, uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4,
                            uint32_t *d5, uint16_t *d6, uint32_t *d7, uint32_t *a0, uint32_t *a1, uint32_t a2,
                            uint32_t *a5, uint32_t a6, dk::Flags *ccr)
{
    PTR(ClawObj) self;
    self.v = a6;
    ClawObj *claw = self;
    for (;;) {
        if (!projected) {
            dk::push_word((uint16_t)*d7);          /* move.w d7,-(a7): the balls left - 1 */
            Rot3D *view = (Rot3D *)g_Rot3DOuter;
            *a0 = ADDR68(view);
            PTR(const s16) list;
            list.v = *a1;
            const s16 *point = list;
            Rot3D_ProjectPoint(d0, d1, d4, d5, d7, view, &point);
            *a1 = ADDR68(point);
            *d7 = (*d7 & 0xFFFF0000u) | (uint16_t)((int16_t)*d7 >> 1);   /* asr.w #1: depth / 2 */
            *d4 = (*d4 & 0xFFFF0000u) | (uint16_t)(*d4 + claw->x);
            *d5 = (*d5 & 0xFFFF0000u) | (uint16_t)(*d5 + claw->y);
        }
        projected = false;

        /* the point's look: size word, attribute word */
        PTR(const u16) look;
        look.v = *a1;
        const u16 *word = look;
        uint16_t size = word[0];
        uint16_t attr = word[1];
        *a1 += 4;
        /* divu.w d7,d1: quotient low, remainder high (depth / 2 = 0 would take the 68000's
           zero-divide trap; the claw's points stay far behind the camera plane, never 0) */
        uint16_t divisor = (uint16_t)*d7;
        uint16_t quotient = (uint16_t)(size / divisor);
        uint16_t remainder = (uint16_t)(size % divisor);
        int16_t index = (int16_t)(uint16_t)(quotient * 2);           /* add.w d1,d1 / (pc,d1.w) */
        PTR(const u16) frame;
        frame.v = Tbl_ClawBallFrames + (uint32_t)(int32_t)index;
        *d1 = ((uint32_t)remainder << 16) | *frame;                  /* move.w: the remainder stays */
        *d6 = attr;
        PTR(u16) slot;
        slot.v = *a5;
        *d2 = (*d2 & 0xFFFF0000u) | *slot;                            /* the slot's current frame */
        Spr_ChangeFrame(d0, d1, d2, d3, a0, a1);
        *slot = (uint16_t)*d1;
        *a5 += 2;
        *d7 = (*d7 & 0xFFFF0000u) | (uint16_t)(claw->sort - (uint16_t)*d7);   /* nearer = larger key */
        *d6 |= claw->priority;
        uint16_t w0 = (uint16_t)*d0;
        dk::Flags spr_ccr{};   /* its &ccr (Spr_* hand their condition codes back) */
        Spr_AddSorted(&w0, d1, d2, (uint16_t)*d4, (uint16_t)*d5, *d6, (uint16_t)*d7, *a0, *a1, a2, &spr_ccr);
        *d0 = (*d0 & 0xFFFF0000u) | w0;

        /* move.w (a7)+,d7 / dbra d7: the count back, the condition codes of the move */
        PTR(const u16) top;
        top.v = dk::stack_pointer();
        uint16_t count = *top;
        dk::set_stack_pointer(top.v + 2);
        *ccr = dk::Flags::test(count, 2, ccr->x);
        count--;
        *d7 = (*d7 & 0xFFFF0000u) | count;
        if (count == 0xFFFF)
            return;
    }
}

/* Boss_Claw_DrawPoints - draws d7+1 3D points of the claw as depth-scaled ball sprites: each point
 * projected through g_Rot3DOuter and moved by the claw's position, its frame Tbl_ClawBallFrames[size /
 * (depth / 2)], sorted by the claw's sort base - depth / 2 with its priority bits
 * In:  a1 = the point list (x, y, z, size, attr words), d7 = count - 1, a5 = the sprite frame slots,
 *      a6 = the claw (ClawObj: sort, priority, x, y); a2 for Spr_AddSorted
 * Out: a1/a5 past the points and slots; d7 low word = $FFFF; d0-d6 as the last ball's drawing left them
 * Evidence: move.w d7,-(a7) / lea g_Rot3DOuter,a0 / jsr Rot3D_ProjectPoint / asr.w #1,d7 / add.w
 *           $3E/$42(a6) then falls into Boss_Claw_DrawProjected, which dbra's back here; called with
 *           each finger's list (d7 = 2) by Boss_Claw_Draw; 2375 TAS frames
 */
void Boss_Claw_DrawPoints(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5,
                          uint16_t *d6, uint32_t *d7, uint32_t *a1, uint32_t a2, uint32_t *a5, uint32_t a6)
{
    uint32_t a0 = 0;                    /* the projection record, then the frame entry (clobbered) */
    dk::Flags ccr{};                    /* the last pop's condition codes: no caller reads them here */
    claw_draw_balls(false, d0, d1, d2, d3, d4, d5, d6, d7, &a0, a1, a2, a5, a6, &ccr);
}

/* Boss_Claw_DrawProjected - the Boss_Claw_DrawPoints loop after its projection: draws the projected
 * point d4/d5 (depth / 2 in d7) as a depth-scaled ball, pops the count and loops back to project the
 * next point
 * In:  d4/d5 = the projected world x/y, d7 = depth / 2, a1 = the point's size word, a5 = the sprite
 *      slots, a6 = the claw; the ball count (- 1) pushed as a word on the stack
 * Out: a1/a5 advanced, the count popped (d7 low word = $FFFF at the end); the condition codes of the
 *      last `move.w (a7)+,d7` (Z: the count was 0)
 * Evidence: bra target of Boss_Claw_DrawChain (which pushes the count and projects the chain head);
 *           moveq #0,d1 / move.w (a1)+,d1 / divu.w d7,d1 / Tbl_01EBCE / Spr_ChangeFrame / neg.w d7 /
 *           add.w $2A(a6),d7 / or.w $2C(a6),d6 / Spr_AddSorted / move.w (a7)+,d7 / dbra; 2349 TAS frames
 */
void Boss_Claw_DrawProjected(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5,
                             uint16_t *d6, uint32_t *d7, uint32_t *a0, uint32_t *a1, uint32_t a2, uint32_t *a5,
                             uint32_t a6, dk::Flags *ccr)
{
    claw_draw_balls(true, d0, d1, d2, d3, d4, d5, d6, d7, a0, a1, a2, a5, a6, ccr);
}
