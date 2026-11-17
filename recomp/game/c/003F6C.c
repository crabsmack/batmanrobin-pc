#include "game.h"

/* Op2_SwitchHeading / Op2_SwitchHeadingClamp ($003F6C-$003FAF): switches on the motion block's
 * current heading (0-$3FF a full turn) split into n equal sectors. */

/* the low word of a data register, the upper word kept (a .w operation) */
static void set_low_word(uint32_t *r, uint16_t v) { *r = (*r & 0xFFFF0000u) | v; }

/* the sector of (heading + rotation) among n: ((h & $3FE) * n) << 6, the halves swapped (so the
 * low word is the product / 1024) */
static void heading_sector(uint32_t *d0, const u16 **script, const Vm2Motion *mot)
{
    set_low_word(d0, (uint16_t)(mot->heading >> 16));
    set_low_word(d0, (uint16_t)(*d0 + *(*script)++));
    set_low_word(d0, (uint16_t)(*d0 & 0x3FE));
    *d0 = (uint32_t)(uint16_t)*d0 * *(*script)++;
    *d0 <<= 6;
    *d0 = (*d0 << 16) | (*d0 >> 16);
}

/* the case the word table entry names: entry = table + 2 * index, then its offset (from itself) */
static void take_case(uint32_t *d0, const u16 **script)
{
    set_low_word(d0, (uint16_t)(*d0 * 2));
    const u16 *entry = (const u16 *)((const u8 *)*script + (int16_t)(uint16_t)*d0);
    *script = (const u16 *)((const u8 *)entry + (int16_t)(uint16_t)*entry);
    dk::jump(dk::code((int16_t)(uint16_t)*(*script)++));
}

/* Op2_SwitchHeading - a2 op: a switch on the current heading split into n equal sectors:
 * index = ((heading + rotation) & $3FE) * n / 1024.
 * In:  script = word rotation, word n, then n word offsets (each from itself); mot->heading
 * Out: d0 = the swapped product, its low word 2 x the index; NEXT at the chosen case
 * Evidence: move.w $C(a3),d0 / add.w (a2)+,d0 / andi.w #$3FE,d0 / mulu.w (a2)+,d0 / lsl.l #6,d0 /
 *           swap d0 / add.w d0,d0 / adda.w d0,a2 / adda.w (a2),a2 / NEXT; 45 corpus runs;
 *           $2FA14 `3F6C 0120 0010` (16 sectors)
 */
void Op2_SwitchHeading(uint32_t *d0, const u16 **script, const Vm2Motion *mot)
{
    heading_sector(d0, script, mot);
    take_case(d0, script);
}

/* Op2_SwitchHeadingClamp - a2 op: as Op2_SwitchHeading, the sector index minus a base clamped to
 * [0, max] before indexing the table.
 * In:  script = word rotation, word n, word base, word max, then max+1 word offsets
 * Out: d0 as Op2_SwitchHeading (0 when the index was below the base); NEXT at the chosen case
 * Evidence: sub.w (a2)+,d0 / bpl.s / moveq #0,d0 / cmp.w (a2)+,d0 / ble.s / move.w -2(a2),d0 /
 *           add.w d0,d0 / adda.w d0,a2 / adda.w (a2),a2 / NEXT; 26 corpus runs;
 *           $26322 `3F86 010C 0028 000F 000A`
 */
void Op2_SwitchHeadingClamp(uint32_t *d0, const u16 **script, const Vm2Motion *mot)
{
    heading_sector(d0, script, mot);
    set_low_word(d0, (uint16_t)(*d0 - *(*script)++));
    if ((int16_t)*d0 < 0)
        *d0 = 0;
    uint16_t max = *(*script)++;
    if ((int16_t)*d0 > (int16_t)max)
        set_low_word(d0, max);
    take_case(d0, script);
}
