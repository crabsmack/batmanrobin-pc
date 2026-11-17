#include "game.h"

/* The weapon-drop director's two placement natives ($013F5E-$013F91): each picks a screen position
 * and goes on to WeaponDrop_SpawnOnScreen, which calls the spawner in the script's next dc.l. */

/* the low word of a data register, the upper word kept (a .w move) */
static inline void set_low_word(uint32_t *r, uint16_t v) { *r = (*r & 0xFFFF0000u) | v; }

/* WeaponDrop_FromTop - director native: drops a pickup from above the screen, at a random x offset
 * of the operand table from $A0 (the screen's centre), y = -10.
 * In:  script = a count word n, n x offsets (words), then the spawner's dc.l
 * Out: d0 = the chosen offset's byte index, d1 = 2n; x = offset + $A0, y = -10 (screen); script at
 *      the dc.l; then WeaponDrop_SpawnOnScreen
 * Evidence: move.w (a2)+,d0 / move.w d0,d1 / jsr Rand_Below / add.w d0,d0 / add.w d1,d1 / move.w
 *           (a2,d0.w),d4 / adda.w d1,a2 / addi.w #$A0,d4 / move.w #$FFF6,d5 / bra; op $2CB2 in the
 *           director scripts at $13FDA/$14002 (offsets -$80/+$80, Item_SpawnPowerUpFalling)
 */
void WeaponDrop_FromTop(uint32_t *d0, uint16_t *d1, uint16_t *x, uint16_t *y, uint32_t *d7, const s16 **script,
                        uint32_t a4, uint32_t a6)
{
    uint16_t n = (uint16_t)*(*script)++;
    set_low_word(d0, n);
    *d1 = n;
    Rand_Below(d0);
    set_low_word(d0, (uint16_t)(*d0 * 2));
    *d1 = (uint16_t)(*d1 * 2);
    const u8 *table = (const u8 *)*script;
    *x = (uint16_t)*(const s16 *)(table + (int16_t)(uint16_t)*d0);
    *script = (const s16 *)(table + (int16_t)*d1);   /* past the offsets */
    *x = (uint16_t)(*x + 0xA0);
    *y = 0xFFF6;
    dk::jump(at_WeaponDrop_SpawnOnScreen);
}

/* WeaponDrop_FromRight - director native: spawns a pickup just past the right screen edge (x =
 * $14A) at a random height ($70 +/- $5F).
 * In:  script = at the spawner's dc.l
 * Out: d0 = Rand_Signed($BE)'s value; x = $14A, y = it + $70 (screen); then WeaponDrop_SpawnOnScreen
 * Evidence: move.w #$BE,d0 / jsr Rand_Signed / move.w d0,d5 / move.w #$14A,d4 / addi.w #$70,d5 / bra;
 *           op $2CB2 at $1402A/$1404C with Item_SpawnPowerUpDrifting (scenario-level-skip)
 */
void WeaponDrop_FromRight(uint32_t *d0, uint16_t *d1, uint16_t *x, uint16_t *y, uint32_t *d7, const s16 **script,
                          uint32_t a4, uint32_t a6)
{
    set_low_word(d0, 0xBE);
    Rand_Signed(d0);
    *y = (uint16_t)*d0;
    *x = 0x14A;
    *y = (uint16_t)(*y + 0x70);
    dk::jump(at_WeaponDrop_SpawnOnScreen);
}
