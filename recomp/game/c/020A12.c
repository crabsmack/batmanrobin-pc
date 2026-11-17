#include "game.h"

static constexpr int SIGN_DEBRIS_PIECES_PER_BLOCK = 3;

/* Stage1_SignDebrisRun - run-list handler of the sign debris: counts its 70-frame life down, then
 * releases the shared frame and unlinks; otherwise steps and draws the 18 pieces
 * In:  self = the debris object (a6): frames_left, frame, the 6 piece blocks
 * Out: the pieces moved and drawn (a4 past the handles, d7 low word = $FFFF); at the end of its
 *      life Spr_ReleaseFrame, then as Obj_UnlinkFreeWithBlocks
 * Evidence: subq.w #1,$1A(a6) / bgt / move.w $18(a6),d2 / jsr Spr_ReleaseFrame / jmp
 *           Obj_UnlinkFreeWithBlocks / lea $8(a6),a4 / move.w #5,d7 / 6 x (move.w d7,-(a7) / movea.w
 *           (a4)+,a5 / 3 x bsr Stage1_StepSignDebris / move.w (a7)+,d7 / dbra); installed by
 *           Stage1_SpawnSignDebris; TAS f1830-f4353 (744 frames)
 */
void Stage1_SignDebrisRun(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5,
                          uint16_t *d6, uint16_t *d7, uint32_t *a0, uint32_t *a1, uint32_t *a4, SignDebrisObj *self)
{
    int16_t left = self->frames_left;
    self->frames_left = (int16_t)(left - 1);
    if (left <= 1) {                  /* subq.w #1 / bgt: (left - 1) > 0, without the wrap */
        *d2 = (*d2 & 0xFFFF0000u) | self->frame;
        uint16_t w0 = (uint16_t)*d0, w2 = (uint16_t)*d2;
        Spr_ReleaseFrame(&w0, &w2, a0);
        *d0 = (*d0 & 0xFFFF0000u) | w0;
        *d2 = (*d2 & 0xFFFF0000u) | w2;
        dk::jump(at_Obj_UnlinkFreeWithBlocks);
        return;
    }
    *a4 = ADDR68(self->block);
    uint32_t a5 = 0;                  /* the piece cursor (clobbered) */
    for (int block = 0; block < 6; block++) {
        *d7 = (uint16_t)(5 - block);
        dk::push_word(*d7);           /* move.w d7,-(a7): kept across the steps, as the asm */
        a5 = (uint32_t)(int32_t)self->block[block];   /* movea.w: sign-extended */
        *a4 += 2;
        for (int i = 0; i < SIGN_DEBRIS_PIECES_PER_BLOCK; i++)
            Stage1_StepSignDebris(d0, d1, d2, d3, d4, d5, d6, a0, a1, &a5, self);
        PTR(const u16) top;           /* move.w (a7)+,d7 */
        top.v = dk::stack_pointer();
        *d7 = *top;
        dk::set_stack_pointer(top.v + 2);
    }
    *d7 = 0xFFFF;                     /* dbra: the count ends at -1 */
}
