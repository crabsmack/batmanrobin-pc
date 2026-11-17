#include "game.h"

/* the muzzle offsets per aim direction (8 bytes each: x, y words, the heading long; ROM data) */
static constexpr uint32_t Tbl_01A28E = 0x01A28Eu;

static constexpr int32_t HARLEY_RUMBLE_SPEED = 0x6000;     /* faster than 0.375 px/frame: the cart rumbles */
static constexpr uint32_t HARLEY_SHAKE_VEL = 0x20000u;     /* 2.0 */
static constexpr uint32_t HARLEY_SHAKE_GRAVITY = 0x10000u; /* 1.0 */
static constexpr uint16_t HARLEY_AIM_DIRS = 0x18;          /* 24 aim directions, stored doubled (48 steps) */
static constexpr uint16_t HARLEY_PICK_DIRS = 8;
static constexpr int16_t HARLEY_SCREEN_MID = 0xA0;
static constexpr uint16_t HARLEY_AIM_RING = 0x30;
static constexpr uint16_t HARLEY_FLASH_CASE = 0x18;        /* the flashing frames follow the 24 normal ones */
static constexpr uint8_t HARLEY_FLASH_RED = 2;
static constexpr uint16_t HARLEY_TINT_ATTR = 0xE000;       /* priority + palette line 3 */
static constexpr int32_t HARLEY_POLE_MAX = 0x820000;       /* 130 px */
static constexpr uint16_t HARLEY_POLE_REST = 0x14;         /* 20 px: the retracted pole */

/* script variable 0's flag bits (the low byte, $23(a4)) */
enum {
    HARLEY_VAR0_ROLLING = 1 << 0,
    HARLEY_VAR0_ROLL_BY_PARITY = 1 << 1,
    HARLEY_VAR0_FLASHING = 1 << 2,
};

static void start_rumble(void)
{
    g_ShakeVel = HARLEY_SHAKE_VEL;
    g_ShakeGravity = HARLEY_SHAKE_GRAVITY;
    g_ShakeY = 0;
}

static HarleyPole *pole_of(const HarleyObj *self) { return dk::handle<HarleyPole>(self->pole); }

/* the targeted player record, when there is one and it can be targeted ($3A nonzero), else null */
static const PlayerRec *target_player(const Vm2Block *vm)
{
    if (vm->tracked == 0)
        return nullptr;
    const PlayerRec *p = dk::handle<const PlayerRec>(vm->tracked);
    return p->targetable ? p : nullptr;
}

/* Boss_Harley_RumbleWhenFast - while the Harley cart moves faster than $6000 (0.375 px/frame) it
 * kicks a small screen shake every 4-13 frames (script variable 1 counts down)
 * In:  mot = the motion block (a3: vel_x), vm = the VM block (a4: var 1)
 * Out: d0 = |vel_x|, or on expiry the new count; on expiry: g_ShakeVel 2.0, g_ShakeGravity 1.0,
 *      g_ShakeY 0, var 1 = 4 + Rand_Below(10)
 * Evidence: move.l $18(a3),d0 / bpl / neg.l / cmpi.l #$6000 / bgt / rts; subq.w #1,$24(a4) / bpl /
 *           the shake / moveq #$A,d0 / jsr Rand_Below / addq.w #4,d0 / move.w d0,$24(a4); TAS
 *           f4540-f4916 (the cart rolling in, 360 frames)
 */
uint32_t Boss_Harley_RumbleWhenFast(const Vm2Motion *mot, Vm2Block *vm)
{
    uint32_t speed = (uint32_t)(int32_t)mot->vel_x;
    if ((int32_t)speed < 0)
        speed = 0u - speed;
    if ((int32_t)speed <= HARLEY_RUMBLE_SPEED)
        return speed;
    Boss_Harley_RumbleCountdown(&speed, vm);
    return speed;
}

/* Boss_Harley_RumbleCountdown - Boss_Harley_RumbleWhenFast past its speed test: counts script
 * variable 1 down and on expiry kicks the small screen shake and reloads the count with 4-13
 * In:  d0 = kept unless the count expires; vm = the VM block (a4: var 1)
 * Out: on expiry: g_ShakeVel 2.0, g_ShakeGravity 1.0, g_ShakeY 0, d0 = var 1 = 4 + Rand_Below(10)
 *      (d0's high word Rand_Below's)
 * Evidence: entry $01A05E (subq.w #1,$24(a4) / bpl / the shake / moveq #$A,d0 / jsr Rand_Below /
 *           addq.w #4,d0 / move.w d0,$24(a4)); entered directly in fuzz-05400-s1 at sync 6,093
 *           (the stepped run stopped there: "entry $01A05E ... has no C")
 */
void Boss_Harley_RumbleCountdown(uint32_t *d0, Vm2Block *vm)
{
    vm->var[1] = (uint16_t)(vm->var[1] - 1);
    if ((int16_t)vm->var[1] >= 0)
        return;
    start_rumble();
    *d0 = 10;
    Rand_Below(d0);
    *d0 = (*d0 & 0xFFFF0000u) | (uint16_t)(*d0 + 4);
    vm->var[1] = (uint16_t)*d0;
}

/* Boss_Harley_RumbleEvery4 - kicks a small screen shake every 4 frames (script variable 1 counts
 * 3..0), regardless of speed
 * In:  vm = the VM block (a4)
 * Out: on expiry: g_ShakeVel 2.0, g_ShakeGravity 1.0, g_ShakeY 0, var 1 = 3
 * Evidence: subq.w #1,$24(a4) / bpl / the shake / move.w #3,$24(a4); computed entry in 3 fuzz runs
 */
void Boss_Harley_RumbleEvery4(Vm2Block *vm)
{
    vm->var[1] = (uint16_t)(vm->var[1] - 1);
    if ((int16_t)vm->var[1] >= 0)
        return;
    start_rumble();
    vm->var[1] = 3;
}

/* Boss_Harley_AimAtPlayer - sets the aim target (sprite variable 1) toward the targeted player (24
 * directions, stored doubled), or 0 with no targetable player
 * In:  vm = the VM block (a4: tracked), spr = the sprite block (a5), self = a6 (x/y)
 * Out: spr->var[1] = d2 = 2 * Mot_DirToPlayer(24) or 0; d0 low word = the tracked handle; d1/d3/d4
 *      as Mot_DirToPlayer took and left them
 * Evidence: moveq #0,d2 / move.w $10(a4),d0 / beq / movea.w d0,a0 / tst.b $3A(a0) / beq / move.w
 *           $3E(a6),d2 / move.w $42(a6),d3 / move.w #$18,d4 / jsr Mot_DirToPlayer / add.w d2,d2 /
 *           move.w d2,$24(a5); computed entry in 3 fuzz runs
 */
void Boss_Harley_AimAtPlayer(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint16_t *d3, uint16_t *d4,
                             const Vm2Block *vm, Vm2Sprite *spr, const HarleyObj *self)
{
    *d2 = 0;
    *d0 = (*d0 & 0xFFFF0000u) | (uint16_t)vm->tracked;
    const PlayerRec *player = target_player(vm);
    if (player) {
        uint16_t dir = (uint16_t)self->x;
        *d3 = (uint16_t)self->y;
        *d4 = HARLEY_AIM_DIRS;
        Mot_DirToPlayer(d0, d1, &dir, *d3, *d4, player);
        *d2 = (uint16_t)(dir + dir);
    }
    spr->var[1] = (uint16_t)*d2;
}

/* Boss_Harley_PickDir8 - picks one of 8 compass directions by script variable 2: 0 gives 0 (up); 1
 * aims at the targeted player; 2 or more clears the variable and aims up toward the screen centre
 * (7 up-left from the right half, 1 up-right from the left)
 * In:  vm = the VM block (a4: var 2, tracked); self = a6 (x/y)
 * Out: d0 = direction 0-7 (for a native switch op); d2-d4 as Mot_DirToPlayer took them
 * Evidence: move.w $26(a4),d0 / beq moveq #0 / cmpi.w #1 / beq; clr.w $26(a4) / $3E(a6) - g_CameraX
 *           / cmpi.w #$A0 / blt moveq #1 / moveq #7; the aim: tst.b $3A(a0) / d4 = 8 / jsr
 *           Mot_DirToPlayer / move.w d2,d0; computed entry in 3 fuzz runs
 */
void Boss_Harley_PickDir8(uint32_t *d0, uint32_t *d1, uint16_t *d2, uint16_t *d3, uint16_t *d4, Vm2Block *vm,
                          const HarleyObj *self)
{
    uint16_t mode = vm->var[2];
    *d0 = (*d0 & 0xFFFF0000u) | mode;
    if (mode == 0) {
        *d0 = 0;
        return;
    }
    if (mode != 1) {
        vm->var[2] = 0;
        int16_t screen_x = (int16_t)(self->x - (int16_t)(g_CameraX >> 16));
        *d0 = screen_x < HARLEY_SCREEN_MID ? 1 : 7;
        return;
    }
    *d0 = (*d0 & 0xFFFF0000u) | (uint16_t)vm->tracked;
    const PlayerRec *player = target_player(vm);
    if (!player) {
        *d0 = 0;
        return;
    }
    *d2 = (uint16_t)self->x;
    *d3 = (uint16_t)self->y;
    *d4 = HARLEY_PICK_DIRS;
    Mot_DirToPlayer(d0, d1, d2, *d3, *d4, player);
    *d0 = (*d0 & 0xFFFF0000u) | *d2;
}

/* Boss_Harley_TurnToAim - turns the aim (sprite variable 0) one step (ring of 48) toward the target
 * (variable 1) and returns its frame case; while var 0 bit 2 is set it flashes on alternate 2-frame
 * periods (frame case +24, palette line 2 red +2)
 * In:  vm = the VM block (a4: var 0), spr = the sprite block (a5)
 * Out: d0 = aim / 2 (0-23), +$18 flashing; g_Pal2TintR = 2 flashing, else 0; d1 = $30; d2/d3 as
 *      Mot_StepDirToward left them
 * Evidence: move.w $22(a5),d0 / move.w $24(a5),d2 / move.w #$30,d1 / jsr Mot_StepDirToward / move.w
 *           d0,$22(a5) / lsr.w #1,d0 / btst #2,$23(a4) / btst #1,$F5FB (g_VIntCount's low byte) /
 *           addi.w #$18,d0 / move.b #2,g_Pal2TintR, else the fall into Boss_Harley_ClearTint;
 *           computed entry in 3 fuzz runs
 */
void Boss_Harley_TurnToAim(uint16_t *d0, uint16_t *d1, uint16_t *d2, uint16_t *d3, const Vm2Block *vm,
                           Vm2Sprite *spr)
{
    *d0 = (uint16_t)spr->var[0];
    *d2 = (uint16_t)spr->var[1];
    *d1 = HARLEY_AIM_RING;
    Mot_StepDirToward(d0, *d1, d2, d3);
    spr->var[0] = (int16_t)*d0;
    *d0 = (uint16_t)(*d0 >> 1);
    if ((vm->var[0] & HARLEY_VAR0_FLASHING) && (g_VIntCount & 2)) {
        *d0 = (uint16_t)(*d0 + HARLEY_FLASH_CASE);
        g_Pal2TintR = HARLEY_FLASH_RED;
        return;
    }
    Boss_Harley_ClearTint();
}

/* Boss_Harley_ClearTint - clears the red tint of palette line 2 (g_Pal2TintR = 0)
 * In:  none
 * Out: g_Pal2TintR = 0
 * Evidence: clr.b g_Pal2TintR / rts; the tail of Boss_Harley_TurnToAim, also entered directly by the
 *           scripts (computed entry after an rts, 3 fuzz runs)
 */
void Boss_Harley_ClearTint(void)
{
    g_Pal2TintR = 0;
}

/* Boss_Harley_RollFrame - returns the 2-frame rolling animation case: 0 unless var 0 bit 0; then
 * frame parity (var 0 bit 1) or bit 1 of x (moves with the cart)
 * In:  vm = the VM block (a4: var 0), self = a6 (x)
 * Out: d0 low word = 0 / 1 (moveq: the whole register on the parity path); d1 low word = var 0, or
 *      var 0 & 2 when rolling
 * Evidence: move.w $22(a4),d0 / move.w d0,d1 / andi.w #1,d0 / beq / andi.w #2,d1 / beq x / moveq #0 /
 *           tst.b g_FrameParity / beq / moveq #1 / x: move.w $3E(a6),d0 / lsr.w #1 / andi.w #1,d0;
 *           computed entry in the TAS
 */
void Boss_Harley_RollFrame(uint32_t *d0, uint16_t *d1, const Vm2Block *vm, const HarleyObj *self)
{
    uint16_t var0 = vm->var[0];
    *d1 = var0;
    uint32_t r = (*d0 & 0xFFFF0000u) | (uint16_t)(var0 & HARLEY_VAR0_ROLLING);
    if (var0 & HARLEY_VAR0_ROLLING) {
        *d1 = (uint16_t)(var0 & HARLEY_VAR0_ROLL_BY_PARITY);
        if (*d1 == 0)
            r = (r & 0xFFFF0000u) | (uint16_t)((uint16_t)self->x >> 1);
        else
            r = g_FrameParity ? 1 : 0;
    }
    *d0 = (r & 0xFFFF0000u) | (uint16_t)(r & 1);
}

/* Boss_Harley_TintSprite - steps the palette line 3 tint flash and, while it is active, switches the
 * sprite to priority + palette line 3
 * In:  spr = the sprite block (a5)
 * Out: spr->attr |= $E000 when Tint_StepLine3 returns a nonzero level; d0/d1 as it left them
 * Evidence: jsr Tint_StepLine3 (its last move.b d0 sets Z) / beq / ori.w #$E000,$38(a5); computed
 *           entry in 3 fuzz runs
 */
void Boss_Harley_TintSprite(uint8_t *d0, uint32_t *d1, Vm2Sprite *spr)
{
    dk::Flags level{};
    Tint_StepLine3(d0, d1, &level);
    if (!level.z)
        spr->attr |= HARLEY_TINT_ATTR;
}

/* Boss_Harley_PoleAtTarget - returns 1 when the Harley pole's height has reached its target (integer
 * parts compared)
 * In:  self = a6 (its pole block)
 * Out: d0 = 0 / 1
 * Evidence: movea.w $10(a6),a0 / move.w (a0),d0 / cmp.w $4(a0),d0 / beq moveq #1 / moveq #0;
 *           computed entry in the TAS (pole phase)
 */
uint32_t Boss_Harley_PoleAtTarget(const HarleyObj *self)
{
    const HarleyPole *pole = pole_of(self);
    return (uint16_t)((uint32_t)pole->target >> 16) == (uint16_t)((uint32_t)pole->height >> 16) ? 1 : 0;
}

/* Boss_Harley_SetPoleTarget - sets the Harley pole's target height and its acceleration toward it
 * from two script words
 * In:  script = a2: word target height, word acceleration; self = a6
 * Out: pole target = height.16, accel = +-acceleration (the sign toward the target); d0/d1 those
 *      values; script past the words
 * Evidence: moveq #0,d0 / move.w (a2)+,d0 / swap / move.w (a2)+,d1 / ext.l / cmp.l $4(a0),d0 / bgt /
 *           neg.l d1 / move.l d0,(a0) / move.l d1,$C(a0); Boss_Harley_StepPole adds +$C to the
 *           velocity; computed entry in fuzz-05400-s2
 */
void Boss_Harley_SetPoleTarget(uint32_t *d0, uint32_t *d1, const u16 **script, const HarleyObj *self)
{
    HarleyPole *pole = pole_of(self);
    const u16 *w = *script;
    *d0 = (uint32_t)(uint16_t)w[0] << 16;
    *d1 = (uint32_t)(int32_t)(int16_t)w[1];
    *script = w + 2;
    if ((int32_t)*d0 <= pole->height)
        *d1 = 0u - *d1;
    pole->target = (int32_t)*d0;
    pole->accel = (int32_t)*d1;
}

/* Boss_Harley_StepPole - moves the Harley pole one frame (velocity += acceleration, height +=
 * velocity), bouncing back at a quarter speed when it passes the target and capping it at 130 px,
 * then places the object at the pole top
 * In:  self = a6 (its pole block)
 * Out: pole height/velocity; the drawn x = x, then Boss_Harley_PlaceAtPoleTop; d0 = the target with
 *      its low word the drawn y; d1 = the height, d2 = the velocity, d3 = the acceleration
 * Evidence: movem.l (a0),d0-d3 / add.l d3,d2 / add.l d2,d1 / tst.l d3 / bpl: cmp.l d0,d1 / blt keep,
 *           else bgt keep / overshoot: move.l d0,d1 / asr.l #2,d2 / neg.l d2 / cmpi.l #$820000,d1 /
 *           ble / movem.l d1-d2,$4(a0) / move.w $3E(a6),$4E(a6) and the fall into
 *           Boss_Harley_PlaceAtPoleTop; frame f5850 shows Harley on a raised pole; fuzz-05400-s2
 */
void Boss_Harley_StepPole(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, HarleyObj *self)
{
    HarleyPole *pole = pole_of(self);
    int32_t target = pole->target;
    int32_t accel = pole->accel;
    int32_t vel = (int32_t)((uint32_t)(int32_t)pole->vel + (uint32_t)accel);
    int32_t height = (int32_t)((uint32_t)(int32_t)pole->height + (uint32_t)vel);
    bool passed = accel >= 0 ? height >= target : height <= target;
    if (passed) {                      /* past the target: stop there and bounce back at 1/4 speed */
        height = target;
        vel = -(vel >> 2);
    }
    if (height > HARLEY_POLE_MAX)
        height = HARLEY_POLE_MAX;
    pole->height = height;
    pole->vel = vel;
    self->drawn_x = self->x;
    *d0 = (uint32_t)target;
    *d1 = (uint32_t)height;
    *d2 = (uint32_t)vel;
    *d3 = (uint32_t)accel;
    *d0 = (*d0 & 0xFFFF0000u) | Boss_Harley_PlaceAtPoleTop(self);
}

/* Boss_Harley_PlaceAtPoleTop - draws the object at the top of the Harley pole: drawn y = y - pole
 * height
 * In:  self = a6 (its pole block)
 * Out: the drawn y = d0 low word
 * Evidence: movea.w $10(a6),a0 / move.w $42(a6),d0 / sub.w $4(a0),d0 / move.w d0,$50(a6); computed
 *           entry at $1A1FE (inside Boss_Harley_StepPole: a script native call) in the TAS
 */
uint16_t Boss_Harley_PlaceAtPoleTop(HarleyObj *self)
{
    uint16_t y = (uint16_t)(self->y - (int16_t)((uint32_t)pole_of(self)->height >> 16));
    self->drawn_y = (int16_t)y;
    return y;
}

/* Boss_Harley_ResetPole - retracts the Harley pole: height = target = 20 px (integer parts; the
 * fractions stay), velocity and acceleration 0
 * In:  self = a6 (its pole block)
 * Out: the pole reset
 * Evidence: clr.l $8(a0) / clr.l $C(a0) / move.w #$14,$4(a0) / move.w #$14,(a0); computed entry in
 *           fuzz-05400-s2
 */
void Boss_Harley_ResetPole(const HarleyObj *self)
{
    HarleyPole *pole = pole_of(self);
    pole->vel = 0;
    pole->accel = 0;
    pole->height = (int32_t)(((uint32_t)HARLEY_POLE_REST << 16) | ((uint32_t)pole->height & 0xFFFFu));
    pole->target = (int32_t)(((uint32_t)HARLEY_POLE_REST << 16) | ((uint32_t)pole->target & 0xFFFFu));
}

/* a pole segment's drawn y: y - height * fraction >> 16 (mulu.w / swap / neg.w / add.w); d0 as left */
static uint32_t place_at_pole(HarleyObj *self, uint16_t fraction)
{
    uint32_t product = (uint32_t)(uint16_t)((uint32_t)pole_of(self)->height >> 16) * fraction;
    uint32_t d0 = (product << 16) | (product >> 16);
    uint16_t y = (uint16_t)(self->y - (uint16_t)d0);
    self->drawn_y = (int16_t)y;
    return (d0 & 0xFFFF0000u) | y;
}

/* Boss_Harley_PlaceAtPole81 - draws a pole segment at 13/16 of the Harley pole's height: drawn y =
 * y - height * $D000 >> 16
 * In:  self = a6 (the pole block it shares with the boss)
 * Out: the drawn y; d0 = the product's low word << 16 | the drawn y
 * Evidence: movea.w $10(a6),a0 / move.w $4(a0),d0 / mulu.w #$D000 / swap / neg.w / add.w $42(a6) /
 *           move.w d0,$50(a6); computed entry in fuzz-05400-s2
 */
uint32_t Boss_Harley_PlaceAtPole81(HarleyObj *self)
{
    return place_at_pole(self, 0xD000);
}

/* Boss_Harley_PlaceAtPole62 - draws a pole segment at 5/8 of the Harley pole's height: drawn y = y -
 * height * $A000 >> 16
 * In:  self = a6 (the pole block it shares with the boss)
 * Out: the drawn y; d0 = the product's low word << 16 | the drawn y
 * Evidence: mulu.w #$A000 / swap / neg.w / add.w $42(a6) / move.w d0,$50(a6); computed entry in
 *           fuzz-05400-s2
 */
uint32_t Boss_Harley_PlaceAtPole62(HarleyObj *self)
{
    return place_at_pole(self, 0xA000);
}

/* Boss_Harley_FireAlongAim - spawns the a4 effect script named by the script's next dc.l at the
 * muzzle of the current aim direction (24 offsets 32 px from the centre, with the heading)
 * In:  spr = the sprite block (a5: var 0 = the aim 0-47), self = a6 (x/y), a2 = the script at the dc.l
 * Out: d0 low word = the entry's offset; d2 = 0, d3 = the heading, d4/d5 = the muzzle x/y (16.16);
 *      then Fx_RunScriptArg
 * Evidence: move.w $22(a5),d0 / andi.w #$FFFE / add.w d0,d0 x2 / lea Tbl_01A28E,a0 / adda.w d0,a0 /
 *           moveq #0 d4/d5 / move.w $3E/$42(a6) / add.w (a0)+ x2 / move.l (a0)+,d3 / moveq #0,d2 /
 *           swap d4/d5 / jmp Fx_RunScriptArg; computed entry in 3 fuzz runs
 */
void Boss_Harley_FireAlongAim(uint32_t *d0, uint32_t d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5,
                              uint32_t d6, uint32_t d7, uint32_t a1, uint32_t *a2, uint32_t a3, uint32_t a4,
                              const Vm2Sprite *spr, const HarleyObj *self)
{
    uint16_t offset = (uint16_t)(((uint16_t)spr->var[0] & 0xFFFE) * 4);   /* 8 bytes per direction pair */
    *d0 = (*d0 & 0xFFFF0000u) | offset;
    PTR(const u16) muzzle;
    muzzle.v = Tbl_01A28E + (uint32_t)(int32_t)(int16_t)offset;
    const u16 *m = muzzle;
    uint16_t x = (uint16_t)(self->x + (int16_t)m[0]);
    uint16_t y = (uint16_t)(self->y + (int16_t)m[1]);
    *d3 = ((uint32_t)(uint16_t)m[2] << 16) | (uint16_t)m[3];
    *d2 = 0;
    *d4 = (uint32_t)x << 16;
    *d5 = (uint32_t)y << 16;
    dk::jump(at_Fx_RunScriptArg);
}
