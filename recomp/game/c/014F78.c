#include "game.h"

/* Compass directions on a ring of n ($014F78-$015045): the direction toward a player, and a turn
 * one step toward a target direction. */

/* the low word of a data register, the upper word kept (a .w move) */
static inline void set_low_word(uint32_t *r, uint16_t v) { *r = (*r & 0xFFFF0000u) | v; }

/* cmp.w b,a / bmi: the 16-bit a - b is negative */
static inline bool minus(uint32_t a, uint32_t b) { return ((uint16_t)(a - b) & 0x8000u) != 0; }

/* mulu.w base,a / divu.w by,a: the octant's share of base, the quotient in a's low word (the
   remainder above it); a quotient over $FFFF would leave a as the product */
static uint16_t share(uint32_t *a, uint16_t base, uint32_t by)
{
    uint32_t v = (uint32_t)(uint16_t)*a * base;
    uint16_t divisor = (uint16_t)by;
    uint32_t q = v / divisor;
    if (q <= 0xFFFFu)
        v = ((v % divisor) << 16) | q;
    *a = v;
    return (uint16_t)v;
}

/* Mot_DirToPlayer - which of n compass directions points from (x, y) to the player (its record's
 * x, and y + 16): 0 = up, n/4 = right, clockwise; the octant's ratio, rounded, wrapping at n.
 * In:  player = the targeted player record; *d2 = from x, y = from y, n = the number of directions
 * Out: d2 = the direction 0..n-1; d0/d1 = dx/dy (1 for a zero; the one divided holds the divu's
 *      remainder:quotient, plus the adds below in its low word)
 * Evidence: move.w $2E(a0),d0 / move.w $32(a0),d1 / addi.w #$10,d1 / sub.w d2,d0 / bne / moveq #1;
 *           base = d4 << 5 per octant; mulu/divu ratio; addi.w #$80 / lsr.w #8 / cmp.w d4 / blt /
 *           sub.w d4; Boss_Harley_AimAtPlayer (24 directions), Boss_Harley_PickAttackCase (8); TAS
 *           f1571-f15702
 */
void Mot_DirToPlayer(uint32_t *d0, uint32_t *d1, uint16_t *d2, uint16_t y, uint16_t n, const PlayerRec *player)
{
    set_low_word(d0, (uint16_t)player->x);
    set_low_word(d1, (uint16_t)(player->y + 0x10));
    set_low_word(d0, (uint16_t)(*d0 - *d2));
    if ((uint16_t)*d0 == 0)
        *d0 = 1;
    set_low_word(d1, (uint16_t)(*d1 - y));
    if ((uint16_t)*d1 == 0)
        *d1 = 1;
    uint16_t base = (uint16_t)(n << 5);      /* 32 x n: an eighth of the ring, x 256 */
    uint16_t dir;
    if ((int16_t)*d0 >= 0) {
        if ((int16_t)*d1 >= 0) {             /* right and down */
            if (!minus(*d0, *d1))
                dir = (uint16_t)(2 * base + share(d1, base, *d0));
            else
                dir = (uint16_t)(4 * base - share(d0, base, *d1));
        } else {                             /* right and up */
            set_low_word(d1, (uint16_t)-(int16_t)*d1);
            if (!minus(*d0, *d1))
                dir = (uint16_t)(2 * base - share(d1, base, *d0));
            else
                dir = share(d0, base, *d1);
        }
    } else {
        set_low_word(d0, (uint16_t)-(int16_t)*d0);
        if ((int16_t)*d1 >= 0) {             /* left and down */
            if (!minus(*d0, *d1)) {
                share(d1, base, *d0);
                set_low_word(d1, (uint16_t)(*d1 + 2 * base));
                dir = (uint16_t)(8 * base - (uint16_t)*d1);
            } else {
                dir = (uint16_t)(4 * base + share(d0, base, *d1));
            }
        } else {                             /* left and up */
            set_low_word(d1, (uint16_t)-(int16_t)*d1);
            if (!minus(*d0, *d1)) {
                share(d1, base, *d0);
                set_low_word(d1, (uint16_t)(*d1 + 2 * base));
                dir = (uint16_t)(4 * base + (uint16_t)*d1);
            } else {
                dir = (uint16_t)(8 * base - share(d0, base, *d1));
            }
        }
    }
    dir = (uint16_t)((uint16_t)(dir + 0x80) >> 8);
    if ((int16_t)dir >= (int16_t)n)
        dir = (uint16_t)(dir - n);
    *d2 = dir;
}

/* Mot_StepDirToward - turns a direction one step toward a target the short way round a ring of n
 * directions (wrapping at 0 and n).
 * In:  dir = the current direction, n = the ring's size, *target = the target direction
 * Out: dir = the new one (unchanged when equal); target = target - dir; half = n / 2 (negated on
 *      the backward side), unless they were equal
 * Evidence: sub.w d0,d2 / beq / bpl; move.w d1,d3 / lsr.w #1,d3 (/ neg.w d3) / cmp.w d3,d2 / bgt (ble);
 *           subq.w #1,d0 / bpl / add.w d1,d0; addq.w #1,d0 / cmp.w d1,d0 / blt / sub.w d1,d0; caller
 *           Boss_Harley_TurnCannon (48 directions)
 */
void Mot_StepDirToward(uint16_t *dir, uint16_t n, uint16_t *target, uint16_t *half)
{
    *target = (uint16_t)(*target - *dir);
    if (*target == 0)
        return;
    bool back;
    if (!(*target & 0x8000u)) {
        *half = (uint16_t)(n >> 1);
        back = (int16_t)*target > (int16_t)*half;          /* the long way forward: go back */
    } else {
        *half = (uint16_t)-(int16_t)(uint16_t)(n >> 1);
        back = !((int16_t)*target <= (int16_t)*half);      /* the short way is back */
    }
    if (back) {
        *dir = (uint16_t)(*dir - 1);
        if (*dir & 0x8000u)
            *dir = (uint16_t)(*dir + n);
    } else {
        *dir = (uint16_t)(*dir + 1);
        if (!((int16_t)*dir < (int16_t)n))
            *dir = (uint16_t)(*dir - n);
    }
}
