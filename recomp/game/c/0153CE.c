#include "game.h"

/* The 3D helpers ($0153CE-$015633): a point through a 3D record's matrix and translation (with
 * or without the perspective divide), and the matrix from the record's three angles. */

/* the sine table (a word per 2 units of angle, 1.15; the cosine $100 bytes on) */
static constexpr uint32_t Tbl_059B22 = 0x059B22u;

/* the low word of a data register, the upper word kept (a .w move) */
static inline void set_low_word(uint32_t *r, uint16_t v) { *r = (*r & 0xFFFF0000u) | v; }
static inline uint32_t swap(uint32_t v) { return (v << 16) | (v >> 16); }
static inline uint32_t muls(uint32_t a, uint32_t b) { return (uint32_t)((int32_t)(int16_t)a * (int16_t)b); }

/* divs.w by,r: the quotient in the low word, the remainder above it; an overflowing quotient
   leaves r as it was */
static void divs(uint32_t *r, uint16_t by)
{
    int64_t n = (int32_t)*r, d = (int16_t)by;
    int64_t q = n / d;
    if (q < -32768 || q > 32767)
        return;
    *r = ((uint32_t)(uint16_t)(int16_t)(n % d) << 16) | (uint16_t)(int16_t)q;
}

/* the products both point functions share: x' / y' / z' (d4 / d5 / d7) = the translation + the
   matrix times the point, a zero coordinate skipped (its products not taken) */
static void transform(uint32_t *d0, uint32_t *d1, uint32_t *x, uint32_t *y, uint32_t *z, const Rot3D *rec,
                      const s16 **point)
{
    *z = (uint32_t)(int32_t)rec->tz;
    *x = (uint32_t)(int32_t)rec->tx;
    *y = (uint32_t)(int32_t)rec->ty;
    for (int i = 0; i < 3; i++) {
        uint16_t p = (uint16_t)*(*point)++;
        set_low_word(d0, p);
        if (p == 0)
            continue;
        *d1 = muls(rec->m[i], p);
        *x += *d1;
        *d1 = muls(rec->m[3 + i], p);
        *y += *d1;
        *d0 = muls(rec->m[6 + i], p);
        *z += *d0;
    }
}

/* Rot3D_ProjectPoint - transforms a 3D point by a 3D record's matrix and translation, then
 * divides x and y by the depth (perspective).
 * In:  rec = the 3D record (Rot3D_BuildMatrix's matrix); point = words x, y, z
 * Out: x/y = x'/8 and y'/8 divided by the depth (divs.w: quotient low, remainder high; an
 *      overflow leaves the shifted sum); depth = z'/4096 (swapped: the fraction above), 1 when 0
 *      (then no divide); point past the 3 words; d0/d1 = the last products
 * Evidence: muls.w of each coordinate with the matrix columns $18/$1E/$24, $1A/$20/$26, $1C/$22/$28,
 *           zero coordinates skipped; asr.l #3 d4/d5 / asl.l #4,d7 / swap / tst.w / divs.w d7; callers
 *           $1EB2C/$1EB86 with the record $F3C4, $471C0; TAS f10512-f118790
 */
void Rot3D_ProjectPoint(uint32_t *d0, uint32_t *d1, uint32_t *x, uint32_t *y, uint32_t *depth, const Rot3D *rec,
                        const s16 **point)
{
    transform(d0, d1, x, y, depth, rec, point);
    *x = (uint32_t)((int32_t)*x >> 3);
    *y = (uint32_t)((int32_t)*y >> 3);
    *depth = swap(*depth << 4);
    if ((uint16_t)*depth == 0) {
        *depth = 1;
        return;
    }
    divs(x, (uint16_t)*depth);
    divs(y, (uint16_t)*depth);
}

/* Rot3D_TransformPoint - rotates and translates a 3D point by a 3D record, without perspective.
 * In:  rec = the 3D record; point = words x, y, z
 * Out: x/y/z = (the sums x 2) swapped: the 1.15 products' integer part in the low word; point
 *      past the 3 words; d0/d1 = the last products
 * Evidence: the same products as Rot3D_ProjectPoint, then add.l d4,d4 / swap d4 (d5, d7 the same);
 *           caller $1EB06 (3 transformed points, +$10E on y), $471C0
 */
void Rot3D_TransformPoint(uint32_t *d0, uint32_t *d1, uint32_t *x, uint32_t *y, uint32_t *z, const Rot3D *rec,
                          const s16 **point)
{
    transform(d0, d1, x, y, z, rec, point);
    *x = swap(*x * 2);
    *y = swap(*y * 2);
    *z = swap(*z * 2);
}

/* Rot3D_BuildMatrix - builds a 3D record's rotation matrix from its three angles with the sine
 * table, each product of two sines / cosines as the half sum or difference of the sum and
 * difference angles' (sin a sin b = (cosine(a-b) - cosine(a+b)) / 2 ...), the third angle by muls.
 * In:  rec = the record (angle words a +0, b +4, c +8; masked to even 0-$3FE)
 * Out: rec->m; d0-d3 as the last steps leave them; sine = Tbl_059B22, cosine = it + $100; a2 =
 *      cosine(a+c) (sign-extended)
 * Evidence: lea Tbl_059B22,a3 / lea $100(a3),a4 / move.w #$3FE,d7; the sum / difference lookups,
 *           asr.l #1 and neg.w per entry, muls.w (a4,d6.w) / (a3,d6.w) / add.l / swap for the c terms;
 *           caller $1EB06 before Rot3D_TransformPoint, $1E9A4, $471C0
 */
void Rot3D_BuildMatrix(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, Rot3D *rec, uint32_t *a2,
                       uint32_t *sine_at, uint32_t *cosine_at)
{
    PTR(const s16) table;
    table.v = Tbl_059B22;
    const s16 *t = table.get();
    *sine_at = Tbl_059B22;
    *cosine_at = Tbl_059B22 + 0x100;
    /* byte offsets 0-$3FE into the tables: sine(o) / cosine(o) of the angle o */
    auto sine = [t](uint16_t o) { return (int32_t)(int16_t)t[o >> 1]; };
    auto cosine = [t](uint16_t o) { return (int32_t)(int16_t)t[(o + 0x100) >> 1]; };
    const uint16_t mask = 0x3FE;
    uint16_t a = (uint16_t)(rec->angle_a & mask);
    uint16_t b = (uint16_t)(rec->angle_b & mask);
    uint16_t c = (uint16_t)(rec->angle_c & mask);

    rec->m[3] = (int16_t)sine(b);
    int32_t s_apb = sine((uint16_t)((a + b) & mask)), c_apb = cosine((uint16_t)((a + b) & mask));
    int32_t s_amb = sine((uint16_t)((a - b) & mask)), c_amb = cosine((uint16_t)((a - b) & mask));
    rec->m[0] = (int16_t)((c_amb + c_apb) >> 1);          /* cos a cos b */
    int32_t sa_sb = (c_amb - c_apb) >> 1;                 /* sin a sin b */
    rec->m[6] = (int16_t)-(int16_t)((s_apb + s_amb) >> 1); /* -sin a cos b */
    int32_t ca_sb = (s_apb - s_amb) >> 1;                 /* cos a sin b */

    int32_t s_bpc = sine((uint16_t)((b + c) & mask)), c_bpc = cosine((uint16_t)((b + c) & mask));
    int32_t s_bmc = sine((uint16_t)((b - c) & mask)), c_bmc = cosine((uint16_t)((b - c) & mask));
    rec->m[4] = (int16_t)((c_bpc + c_bmc) >> 1);          /* cos b cos c */
    rec->m[5] = (int16_t)-(int16_t)((s_bpc - s_bmc) >> 1); /* -cos b sin c */

    int32_t s_apc = sine((uint16_t)((a + c) & mask)), c_apc = cosine((uint16_t)((a + c) & mask));
    int32_t s_amc = sine((uint16_t)((a - c) & mask)), c_amc = cosine((uint16_t)((a - c) & mask));
    *a2 = (uint32_t)c_apc;
    uint32_t r0 = (uint32_t)((c_amc + c_apc) >> 1);       /* cos a cos c */
    uint32_t r4 = (uint32_t)((c_amc - c_apc) >> 1);       /* sin a sin c */
    uint32_t r5 = (uint32_t)((s_apc + s_amc) >> 1);       /* sin a cos c */
    uint32_t r3 = (uint32_t)((s_apc - s_amc) >> 1);       /* cos a sin c */

    uint32_t cc = (uint32_t)cosine(c), sc = (uint32_t)sine(c);
    uint32_t p = swap(muls(cc, (uint32_t)sa_sb) * 2);     /* sin a sin b cos c */
    set_low_word(&r3, (uint16_t)(r3 + p));
    rec->m[7] = (int16_t)(uint16_t)r3;
    p = swap(muls(cc, (uint32_t)ca_sb) * 2);              /* cos a sin b cos c */
    set_low_word(&r4, (uint16_t)(r4 - p));
    rec->m[1] = (int16_t)(uint16_t)r4;
    *d2 = swap(muls((uint32_t)sa_sb, sc) * 2);            /* sin a sin b sin c */
    set_low_word(&r0, (uint16_t)(r0 - *d2));
    rec->m[8] = (int16_t)(uint16_t)r0;
    *d1 = swap(muls((uint32_t)ca_sb, sc) * 2);            /* cos a sin b sin c */
    set_low_word(&r5, (uint16_t)(r5 + *d1));
    rec->m[2] = (int16_t)(uint16_t)r5;
    *d0 = r0;
    *d3 = r3;
}
