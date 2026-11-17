#include "game.h"

/* the rubble rain's piece records: a list of record pointers ended by 0 (ROM data) */
static constexpr uint32_t Tbl_RubbleRecords = 0x020C26u;

static constexpr int32_t SIGN_DEBRIS_GRAVITY = 0x7000;
static constexpr uint16_t SIGN_TUMBLE_STEP = 0xA;
static constexpr int16_t SIGN_TUMBLE_FRAMES = 0x3C;    /* 6 frames of $A */

static constexpr int16_t RUBBLE_RAIN_FRAMES = 0x578;
static constexpr int16_t RUBBLE_RAIN_DELAY = 6;        /* a piece every 7th frame */
static constexpr int16_t RUBBLE_SPREAD = 0x12C;        /* random x within +-150 px (Rand_Next's word) */
static constexpr uint32_t RUBBLE_CENTRE_X = 160u << 16;  /* $A00000: 160 px, the screen centre, 16.16 */
static constexpr int16_t RUBBLE_BOUNCE_FALL = 0x32;
static constexpr uint16_t RUBBLE_FLOOR_MASK = 3;       /* the terrain routine's floor kinds (d7) */
static constexpr int16_t RUBBLE_GONE_Y = 0x100;
static constexpr uint16_t RUBBLE_ATTR = 0x8000;        /* high priority */

/* a rubble piece record (Tbl_RubbleRecords' entries): the animation (rate, period, frames) follows */
typedef struct RubbleRecord {
    s16 fall;
    s32 vy;
    s32 gravity;
} RubbleRecord;
static_assert(sizeof(RubbleRecord) == 10, "RubbleRecord");

/* a 16.16 world coordinate stored as an integer word and a fraction word */
static uint32_t fixed_get(s16 &whole, u16 &frac) { return ((uint32_t)(uint16_t)whole << 16) | frac; }
static void fixed_set(s16 &whole, u16 &frac, uint32_t v)
{
    whole = (int16_t)(v >> 16);
    frac = (uint16_t)v;
}
static uint32_t swap_words(uint32_t v) { return (v << 16) | (v >> 16); }

/* Stage1_StepSignDebris - moves one sign-debris piece (x += vx, vy += $7000 gravity, y += vy),
 * advances its 6-frame tumble when the spin accumulator carries, and draws it in front
 * In:  a5 = the piece (SignDebrisPiece), self = the debris object (a6: the shared frame slot)
 * Out: a5 += $18; d4/d5 = the speeds with the position's integer part in the low word, d6 = attr;
 *      d0/d1/d2/d3/a0/a1 as the drawing left them
 * Evidence: move.l (a5),d4 / add.l d4,$4(a5) / addi.l #$7000,$8(a5) / add.l d5,$C(a5) / add.b
 *           d0,$11(a5) / bcc / addi.w #$A,d1 / cmpi.w #$3C / blt / subi.w #$3C / Spr_ChangeFrame on
 *           $18(a6) with base $14(a5) / Spr_AddFront; called 3 times per block by Stage1_SignDebrisRun
 *           (TAS f1830-f4352)
 */
void Stage1_StepSignDebris(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5,
                           uint16_t *d6, uint32_t *a0, uint32_t *a1, uint32_t *a5, SignDebrisObj *self)
{
    PTR(SignDebrisPiece) at;
    at.v = *a5;
    SignDebrisPiece *piece = at;
    *d4 = (uint32_t)piece->vx;
    fixed_set(piece->x, piece->x_frac, fixed_get(piece->x, piece->x_frac) + *d4);
    piece->vy = (int32_t)((uint32_t)piece->vy + SIGN_DEBRIS_GRAVITY);
    *d5 = (uint32_t)piece->vy;
    fixed_set(piece->y, piece->y_frac, fixed_get(piece->y, piece->y_frac) + *d5);

    uint16_t tumble = piece->tumble;
    *d0 = (*d0 & 0xFFFFFF00u) | piece->spin_rate;
    unsigned acc = (unsigned)piece->spin_acc + piece->spin_rate;
    piece->spin_acc = (uint8_t)acc;
    if (acc > 0xFF) {                 /* add.b's carry: the next tumble frame */
        tumble = (uint16_t)(tumble + SIGN_TUMBLE_STEP);
        if ((int16_t)tumble >= SIGN_TUMBLE_FRAMES)
            tumble = (uint16_t)(tumble - SIGN_TUMBLE_FRAMES);
        piece->tumble = tumble;
    }
    *d1 = (*d1 & 0xFFFF0000u) | (uint16_t)(tumble + piece->frame);
    *d2 = (*d2 & 0xFFFF0000u) | self->frame;
    Spr_ChangeFrame(d0, d1, d2, d3, a0, a1);
    self->frame = (uint16_t)*d1;
    *d4 = (*d4 & 0xFFFF0000u) | (uint16_t)piece->x;
    *d5 = (*d5 & 0xFFFF0000u) | (uint16_t)piece->y;
    *d6 = piece->attr;
    uint16_t w1 = (uint16_t)*d1;
    dk::Flags spr_ccr{};   /* its &ccr (Spr_* hand their condition codes back) */
    Spr_AddFront(d0, &w1, (uint16_t)*d4, (uint16_t)*d5, *d6, a0, &spr_ccr);
    *d1 = (*d1 & 0xFFFF0000u) | w1;
    *a5 += sizeof(SignDebrisPiece);
}

/* Stage1_StartRubbleRain - bombing native: starts a run-list object that rains rubble pieces on the
 * street for $578 frames (Stage1_RubbleRainRun), with the piece records of Tbl_020C26
 * In:  a6 = the bombing object (Obj_NewClear replaces it)
 * Out: d0 = 0 (Z set), the rain's frames_left $578, delay 6, cursor Tbl_020C26; d0 = -1 when the
 *      pool is full
 * Evidence: lea Stage1_RubbleRainRun,a0 / jsr Obj_NewClear / bne / move.l #$20C26,$1C(a6) / move.w
 *           #$578,$18(a6) / move.w #6,$1A(a6) / moveq #0,d0; Op2_CallNativeRelSaved at $020D3E in
 *           Scr2_020CF6; the TAS and fuzz-01800 (around f1989)
 */
uint32_t Stage1_StartRubbleRain(uint32_t a6)
{
    uint32_t d0 = 0, obj = a6;
    Obj_NewClear(&d0, at_Stage1_RubbleRainRun, &obj);
    if (d0 != 0)
        return d0;
    PTR(RubbleRainObj) at;
    at.v = obj;
    RubbleRainObj *rain = at;
    rain->cursor.v = Tbl_RubbleRecords;
    rain->frames_left = RUBBLE_RAIN_FRAMES;
    rain->delay = RUBBLE_RAIN_DELAY;
    return 0;
}

/* Stage1_RubbleRainRun - run-list handler of the rubble rain: every 7 frames spawns a rubble piece
 * (Stage1_RubblePieceRun) at a random x within +-150 px of the screen centre at the camera top,
 * cycling through the Tbl_020C26 records; frees itself when its frames run out
 * In:  a6 = the rain (RubbleRainObj)
 * Out: a new piece: x/y, vx/ax 0, vy/gravity/fall/anim from the record, frame and phase 0; d0 = the
 *      record's address, a0 = the cursor, a1 = the record's animation, a6 = the rain again. At the
 *      end: as Obj_UnlinkFree
 * Evidence: subq.w #1,$18(a6) / bmi Obj_UnlinkFree / subq.w #1,$1A(a6) / bpl / Obj_New with
 *           Stage1_RubblePieceRun / Rand_Next * $12C + $A00000 + g_CameraX / a zero record pointer
 *           wraps to Tbl_020C26 / movea.l a5,a6; png f002400 shows the pieces falling over the street
 */
void Stage1_RubbleRainRun(uint32_t *d0, uint32_t *a0, uint32_t *a1, uint32_t *a6)
{
    PTR(RubbleRainObj) at;
    at.v = *a6;
    RubbleRainObj *rain = at;
    int16_t left = rain->frames_left;
    rain->frames_left = (int16_t)(left - 1);
    if ((int16_t)(left - 1) < 0) {    /* bmi */
        dk::jump(at_Obj_UnlinkFree);
        return;
    }
    int16_t delay = (int16_t)(rain->delay - 1);
    rain->delay = delay;
    if (delay >= 0)                   /* bpl */
        return;
    rain->delay = RUBBLE_RAIN_DELAY;
    uint32_t a5 = *a6;                /* movea.l a6,a5: the rain, while a6 is the new piece */
    *a0 = at_Stage1_RubblePieceRun;
    Obj_New(d0, a0, a1, a6);
    if (*d0 != 0)                     /* bne: the pool is full (a6 as Obj_New left it) */
        return;
    PTR(RubblePieceObj) piece_at;
    piece_at.v = *a6;
    RubblePieceObj *piece = piece_at;
    *d0 = (uint32_t)((int32_t)(int16_t)Rand_Next() * RUBBLE_SPREAD);   /* muls.w */
    *d0 += RUBBLE_CENTRE_X;
    *d0 += g_CameraX;
    fixed_set(piece->x, piece->x_frac, *d0);
    fixed_set(piece->y, piece->y_frac, g_CameraY);
    PTR(const u32) cursor = rain->cursor;
    *d0 = *cursor;
    cursor += 1;
    if (*d0 == 0) {                   /* the list's end: back to its start */
        cursor.v = Tbl_RubbleRecords;
        *d0 = *cursor;
        cursor += 1;
    }
    *a0 = cursor.raw();
    rain->cursor = cursor;
    PTR(const RubbleRecord) rec_at;
    rec_at.v = *d0;
    const RubbleRecord *rec = rec_at;
    piece->fall = rec->fall;
    piece->vy = rec->vy;
    piece->gravity = rec->gravity;
    *a1 = *d0 + sizeof(RubbleRecord);
    piece->anim.v = *a1;
    piece->frame = 0;
    piece->anim_phase = 0;
    piece->vx = 0;
    piece->ax = 0;
    *a6 = a5;
}

/* Stage1_RubblePieceRun - run-list handler of one rubble piece: falls freely while its fall counter
 * runs, then moves with the scene's terrain routine, bouncing off the floor at half speed with a
 * random x drift; animates from its table and is freed below y $100
 * In:  self = the piece (a6: RubblePieceObj)
 * Out: drawn as Spr_AddBack (attr $8000), or Spr_ReleaseFrame then as Obj_UnlinkFree. d1/d2 = the
 *      new speeds (free fall) or the terrain routine's (moved); d4/d5 the position's integer part
 *      in the low word; d0-d7/a0/a1 as the drawing or the terrain routine left them
 * Evidence: movea.l g_TerrainFn,a0 / jsr (a0) with d1/d2 = the move in pixels, d6 = 0, d7 = 3; a
 *           floor contact (d7 != 0): $38 = $32, vy = -vy/2, vx = Rand_Next * 2; Spr_ReleaseFrame +
 *           Obj_UnlinkFree when y > $100; the TAS and fuzz-01800/03600
 */
void Stage1_RubblePieceRun(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5,
                           uint32_t *d6, uint32_t *d7, uint32_t *a0, uint32_t *a1, RubblePieceObj *self)
{
    *d1 = (uint32_t)self->ax + (uint32_t)self->vx;
    self->vx = (int32_t)*d1;
    *d2 = (uint32_t)self->gravity + (uint32_t)self->vy;
    self->vy = (int32_t)*d2;
    *d4 = fixed_get(self->x, self->x_frac);   /* movem.l $20(a6),d4-d5 */
    *d5 = fixed_get(self->y, self->y_frac);
    int16_t fall = (int16_t)(self->fall - 1);
    self->fall = fall;
    if (fall >= 0) {                  /* bpl: free fall */
        *d4 += *d1;
        *d5 += *d2;
        fixed_set(self->x, self->x_frac, *d4);
        fixed_set(self->y, self->y_frac, *d5);
        *d4 = swap_words(*d4);
        *d5 = swap_words(*d5);
    } else {                          /* the terrain routine moves it by the whole pixels */
        *d1 += *d4;
        *d2 += *d5;
        self->x_frac = (uint16_t)*d1;
        self->y_frac = (uint16_t)*d2;
        *d1 = swap_words(*d1);
        *d2 = swap_words(*d2);
        *d4 = swap_words(*d4);
        *d5 = swap_words(*d5);
        *d1 = (*d1 & 0xFFFF0000u) | (uint16_t)(*d1 - *d4);
        *d2 = (*d2 & 0xFFFF0000u) | (uint16_t)(*d2 - *d5);
        *d6 = 0;
        *d7 = (*d7 & 0xFFFF0000u) | RUBBLE_FLOOR_MASK;
        *a0 = g_TerrainFn;
        Stage1RubbleTerrainFn(dk::code(*a0), d0, d1, d2, d3, d4, d5, d6, d7, a0, a1, self);
        self->x = (int16_t)*d4;
        self->y = (int16_t)*d5;
        if ((uint16_t)*d7 != 0) {     /* landed: bounce */
            self->fall = RUBBLE_BOUNCE_FALL;
            *d0 = (uint32_t)-((int32_t)self->vy >> 1);   /* asr.l #1 / neg.l */
            self->vy = (int32_t)*d0;
            *d0 = (uint32_t)Rand_Next() * 2;
            self->vx = (int32_t)*d0;
        }
    }
    if ((int16_t)*d5 > RUBBLE_GONE_Y) {   /* fell off the bottom */
        *d2 = (*d2 & 0xFFFF0000u) | self->frame;
        uint16_t w0 = (uint16_t)*d0, w2 = (uint16_t)*d2;
        Spr_ReleaseFrame(&w0, &w2, a0);
        *d0 = (*d0 & 0xFFFF0000u) | w0;
        *d2 = (*d2 & 0xFFFF0000u) | w2;
        dk::jump(at_Obj_UnlinkFree);
        return;
    }
    const s16 *anim = self->anim;
    *d0 = (uint32_t)(int32_t)anim[0];   /* movem.w (a0)+,d0-d1: sign-extended */
    *d1 = (uint32_t)(int32_t)anim[1];
    *a0 = ADDR68(anim + 2);
    uint16_t phase = (uint16_t)(*d0 + self->anim_phase);
    if ((int16_t)phase >= (int16_t)*d1)  /* cmp.w d1,d0 / blt */
        phase = (uint16_t)(phase - (uint16_t)*d1);
    self->anim_phase = phase;
    uint16_t index = (uint16_t)((phase >> 8) * 2);   /* lsr.w #8 / add.w d0,d0 */
    *d0 = (*d0 & 0xFFFF0000u) | index;
    *d1 = (*d1 & 0xFFFF0000u) | (uint16_t)anim[2 + (int16_t)index / 2];
    *d2 = (*d2 & 0xFFFF0000u) | self->frame;
    Spr_ChangeFrame(d0, d1, d2, d3, a0, a1);
    self->frame = (uint16_t)*d1;
    *d6 = (*d6 & 0xFFFF0000u) | RUBBLE_ATTR;
    dk::jump(at_Spr_AddBack);
}
