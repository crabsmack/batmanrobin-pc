#include "game.h"

/* the turret coroutine's script after the native's jsr $7FE.w (Op2_Return) */
static constexpr uint32_t Scr2_0430C2 = 0x0430C2u;

enum {
    GUNTANK_HEADING_MASK = 0x3FF,   /* 1024 heading units a turn, $100 = straight down */
    GUNTANK_DOWN = 0x100,
    GUNTANK_LEFT = 0x200,
    GUNTANK_AIM_LOWER_LEFT = 0x202,  /* a lower-left aim becomes just above the left horizontal */
    GUNTANK_AIM_LOWER_RIGHT = 0x3FE, /* a lower-right aim becomes just above the right horizontal */
};

/* Enemy_GunTank_ClampAimUp - native fragment of the gun tank's turret coroutine 3: after
 * Op2_AimAtTracked it keeps the target heading in the upper half (the twin gun never aims into the
 * floor): ($100,$200] -> $202, [0,$100] -> $3FE, the rest left as they are; then the script goes on
 * In:  turret = the turret's motion block (a3: +$A target heading); the coroutine's registers
 * Out: d0.w = the heading (masked, or the clamped value), which the script continues with at
 *      Scr2_0430C2 (jsr $7FE.w); d0's upper word stays
 * Evidence: move.w $A(a3),d0 / andi.w #$3FF / cmpi.w #$200 / bgt (no store) / cmpi.w #$100 / ble
 *           $3FE / else $202 / move.w d0,$A(a3) / jsr $7FE.w; word $07FC at $4309A after
 *           Op2_StartCoroutine3Rel, Op2_AimAtTracked in the gun tank body $42FEC; TAS 922 frames
 *           119574-120799 (stage 4 lab)
 * The prototype hands d0 on (measured: an input only): the asm leaves the heading in d0's low word
 * for the script.
 */
void Enemy_GunTank_ClampAimUp(uint32_t *d0, uint32_t d1, uint32_t d2, uint32_t d3, uint32_t d4, uint32_t d5,
                              uint32_t d6, uint32_t d7, uint32_t a1, Vm2Motion *turret, uint32_t a4, uint32_t a5,
                              uint32_t a6)
{
    int16_t heading = (int16_t)(turret->heading_target & GUNTANK_HEADING_MASK);
    if (heading <= GUNTANK_LEFT) {
        heading = heading <= GUNTANK_DOWN ? GUNTANK_AIM_LOWER_RIGHT : GUNTANK_AIM_LOWER_LEFT;
        turret->heading_target = (uint16_t)heading;
    }
    *d0 = (*d0 & 0xFFFF0000u) | (uint16_t)heading;
    dk::jump_script(at_VM2_Enter, Scr2_0430C2);
}
