#include "game.h"

static inline void set_low_word(uint32_t *r, uint16_t v) { *r = (*r & 0xFFFF0000u) | v; }

/* WeaponDrop_FromTopRandom - director native: drops a pickup from above the screen at a random x
 * (Rand_Signed($122) + $A0 from the screen's left, so $2F-$110), y = -10, then falls into
 * WeaponDrop_SpawnOnScreen.
 * In:  script = at the spawner's dc.l
 * Out: d0 = Rand_Signed($122)'s value; x = it + $A0, y = -10 (screen); then WeaponDrop_SpawnOnScreen
 * Evidence: move.w #$122,d0 / jsr Rand_Signed / move.w d0,d4 / addi.w #$A0,d4 / moveq #-10,d5,
 *           falling into WeaponDrop_SpawnOnScreen; op $2CB2 at $01406E with Item_SpawnPowerUpFalling
 *           (the weapon-drop director script); no corpus run takes that branch (crane-crash
 *           2026-10-30); 013F92.syn checks it
 */
void WeaponDrop_FromTopRandom(uint32_t *d0, uint16_t *d1, uint16_t *x, uint16_t *y, uint32_t *d7, const s16 **script,
                              uint32_t a4, uint32_t a6)
{
    set_low_word(d0, 0x122);
    Rand_Signed(d0);
    *x = (uint16_t)*d0;
    *x = (uint16_t)(*x + 0xA0);
    *y = 0xFFF6;   /* moveq #-10,d5 */
    dk::jump(at_WeaponDrop_SpawnOnScreen);
}
