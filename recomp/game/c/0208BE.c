#include "game.h"

/* the 18 pieces' start records (ROM data, batmanrobin.asm) */
static constexpr uint32_t Tbl_SignDebrisInit = 0x02095Eu;
static constexpr uint16_t SIGN_DEBRIS_BLOCKS = 7;     /* the object and 6 piece blocks */
static constexpr int16_t SIGN_DEBRIS_LIFE = 0x46;     /* 70 frames */
static constexpr int SIGN_DEBRIS_PIECES_PER_BLOCK = 3;

/* Stage1_SpawnSignDebris - spawns the debris burst of a blasted bank sign: one 7-block run-list
 * object (Stage1_SignDebrisRun, 70 frames) holding 18 pieces initialized from Tbl_02095E
 * In:  d4/d5 = the burst centre, d6 = the sprite attribute
 * Out: a6 = the debris object, its frame slot 0 and life $46; a3 past the records, a4 past the 6
 *      handles, a5 past the last block's pieces, d7 low word = $FFFF, d1 as the last piece's
 *      init left it. Nothing (d7 = 7, d0 = -1) when the pool is short
 * Evidence: moveq #7,d7 / lea Stage1_SignDebrisRun,a0 / jsr Obj_NewMulti / bne / clr.w $18(a6) /
 *           move.w #$46,$1A(a6) / lea Tbl_02095E,a3 / lea $8(a6),a4 / moveq #5,d7 / 6 x (movea.w
 *           (a4)+,a5 / 3 x bsr Stage1_InitSignDebris); called only by Stage1_BlastSignAt
 */
void Stage1_SpawnSignDebris(uint32_t *d1, uint16_t d4, uint16_t d5, uint16_t d6, uint32_t *d7, uint32_t *a0,
                            uint32_t *a1, uint32_t *a2, uint32_t *a3, uint32_t *a4, uint32_t *a5, uint32_t *a6)
{
    *d7 = SIGN_DEBRIS_BLOCKS;         /* moveq: the whole register */
    *a0 = at_Stage1_SignDebrisRun;
    uint32_t d0 = 0;
    uint16_t w1 = (uint16_t)*d1, w7 = (uint16_t)*d7;
    Obj_NewMulti(&d0, &w1, &w7, a0, a1, a2, a6);
    *d1 = (*d1 & 0xFFFF0000u) | w1;
    *d7 = (*d7 & 0xFFFF0000u) | w7;
    if (d0 != 0)                      /* bne: the pool is short (d0 = -1) */
        return;
    PTR(SignDebrisObj) obj;
    obj.v = *a6;
    SignDebrisObj *debris = obj;
    debris->frame = 0;
    debris->frames_left = SIGN_DEBRIS_LIFE;
    *a3 = Tbl_SignDebrisInit;
    *a4 = ADDR68(debris->block);
    for (int block = 0; block < 6; block++) {
        *a5 = (uint32_t)(int32_t)debris->block[block];   /* movea.w: sign-extended */
        *a4 += 2;
        for (int i = 0; i < SIGN_DEBRIS_PIECES_PER_BLOCK; i++)
            Stage1_InitSignDebris(d1, d4, d5, d6, a3, a5);
    }
    *d7 = 0x0000FFFFu;                /* moveq #5 / dbra: the low word ends at -1 */
}
