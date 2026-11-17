#include "game.h"

/* Stage1_InitSignDebris - initializes one $18-byte sign-debris piece from a Tbl_02095E record: the
 * first tumble frame, the x/y offsets from the burst, the x and y launch speeds plus random spread,
 * a random spin rate
 * In:  a3 = the record (SignDebrisInit), a5 = the piece, d4/d5 = the burst centre, d6 = the attribute
 * Out: the piece: vx = rand + vx * 256, x/y integer parts = centre + offset (the fractions stay),
 *      vy = vy * 256 - (rand & $FFFF) * 6, spin_rate $80-$BF, tumble 0, frame, attr; a3 += 10,
 *      a5 += $18; d1 = vy * 256 (ext.l / asl.l #8)
 * Evidence: move.w (a3)+,$14(a5) / Rand_Next / andi.b #$3F / addi.b #$80 / move.b d0,$10(a5) / clr.w
 *           $12(a5) / dx + d4 -> $4(a5) / dy + d5 -> $C(a5) / Rand_Next + vx << 8 -> (a5) / Rand_Next
 *           & $FFFF * 6, negated, + vy << 8 -> $8(a5) / move.w d6,$16(a5); called 18 times by
 *           Stage1_SpawnSignDebris
 */
void Stage1_InitSignDebris(uint32_t *d1, uint16_t d4, uint16_t d5, uint16_t d6, uint32_t *a3, uint32_t *a5)
{
    PTR(const SignDebrisInit) rec_at;
    rec_at.v = *a3;
    const SignDebrisInit *rec = rec_at;
    PTR(SignDebrisPiece) piece_at;
    piece_at.v = *a5;
    SignDebrisPiece *piece = piece_at;

    piece->frame = rec->frame;
    piece->spin_rate = (uint8_t)(((uint8_t)Rand_Next() & 0x3F) + 0x80);
    piece->tumble = 0;
    piece->x = (int16_t)(rec->dx + d4);
    piece->y = (int16_t)(rec->dy + d5);
    *d1 = (uint32_t)(int32_t)rec->vx << 8;   /* ext.l / asl.l #8 */
    piece->vx = (int32_t)((uint32_t)Rand_Next() + *d1);
    uint32_t spread = ((uint32_t)Rand_Next() & 0xFFFFu) * 6;
    *d1 = (uint32_t)(int32_t)rec->vy << 8;
    piece->vy = (int32_t)(*d1 - spread);
    piece->attr = d6;
    *a3 += sizeof(SignDebrisInit);
    *a5 += sizeof(SignDebrisPiece);
}
