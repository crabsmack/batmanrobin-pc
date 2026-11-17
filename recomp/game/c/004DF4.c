#include "game.h"

/* Hitbox_TestAttackBoxAt and its kind jump table ($004DF4-$004E31): an attack box's rectangle
 * placed at a position, then by the box's kind word the boxes it may strike are searched. */

/* the low word of a data register, the upper word kept (a .w operation) */
static void set_low_word(uint32_t *r, uint16_t v) { *r = (*r & 0xFFFF0000u) | v; }
/* movem.w into a data register: the word sign-extended */
static uint32_t sext(int16_t w) { return (uint32_t)(int32_t)w; }

/* Hitbox_TestAttackBoxAt - tests one attack box at a position: its spec rectangle mirrored by the
 * sprite's flip bits (attr bits 11 / 12: the edges negated and swapped), placed at d1 / d2, then
 * the kind word picks the search through the jump table at $4E22 (0 first player box, 4 every
 * player box, 8 first enemy box).
 * In:  a0 = the attack box (kind, spec rectangle); d1 / d2 = the position; a5 = the sprite block
 * Out: d3 = the kind, d4-d7 = the absolute rectangle (left, top, right, bottom); jumped to the
 *      kind's entry
 * Evidence: movem.w $6(a0),d3-d7 / btst #3,$38(a5) + neg.w d4 / neg.w d6 / exg.l d4,d6, the same
 *           on bit 4 for d5 / d7 / add.w d1 / d2 / jmp $4E22(pc,d3.w); callers $4DEC,
 *           Op2_TestAttackBoxAtDrawn
 */
void Hitbox_TestAttackBoxAt(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5,
                            uint32_t *d6, uint32_t *d7, uint32_t a0, uint32_t *a1, uint32_t a3, uint32_t *a4,
                            uint32_t a5, uint32_t *a6)
{
    (void)d0, (void)a1, (void)a3, (void)a4, (void)a6;
    PTR(const HitBox) box;
    box.v = a0;
    PTR(const Vm2Sprite) spr;
    spr.v = a5;
    *d3 = sext(box->kind);
    *d4 = sext(box->spec_left);
    *d5 = sext(box->spec_top);
    *d6 = sext(box->spec_right);
    *d7 = sext(box->spec_bottom);
    if (spr->attr & 0x0800) {             /* H flip: left / right mirrored */
        set_low_word(d4, (uint16_t)-*d4);
        set_low_word(d6, (uint16_t)-*d6);
        uint32_t t = *d4;
        *d4 = *d6;
        *d6 = t;
    }
    if (spr->attr & 0x1000) {             /* V flip: top / bottom mirrored */
        set_low_word(d5, (uint16_t)-*d5);
        set_low_word(d7, (uint16_t)-*d7);
        uint32_t t = *d5;
        *d5 = *d7;
        *d7 = t;
    }
    set_low_word(d4, (uint16_t)(*d4 + *d1));
    set_low_word(d6, (uint16_t)(*d6 + *d1));
    set_low_word(d5, (uint16_t)(*d5 + *d2));
    set_low_word(d7, (uint16_t)(*d7 + *d2));
    dk::jump(at_Hitbox_KindFirstPlayer + (uint32_t)(int32_t)(int16_t)*d3);   /* jmp $4E22(pc,d3.w) */
}

/* Hitbox_KindFirstPlayer - kind-table entry 0: Hitbox_HitFirstPlayerBox.
 * In:  as Hitbox_TestAttackBoxAt leaves them
 * Out: as Hitbox_HitFirstPlayerBox
 * Evidence: bra.w Hitbox_HitFirstPlayerBox; reached by jmp $4E22(pc,d3.w) (call-site log)
 */
void Hitbox_KindFirstPlayer(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5,
                            uint32_t *d6, uint32_t *d7, uint32_t a0, uint32_t *a1, uint32_t a3, uint32_t *a4,
                            uint32_t a5, uint32_t *a6)
{
    (void)d0, (void)d1, (void)d2, (void)d3, (void)d4, (void)d5, (void)d6, (void)d7;
    (void)a0, (void)a1, (void)a3, (void)a4, (void)a5, (void)a6;
    dk::jump(at_Hitbox_HitFirstPlayerBox);
}

/* Hitbox_KindAllPlayers - kind-table entry 4: Hitbox_HitAllPlayerBoxes.
 * In:  as Hitbox_TestAttackBoxAt leaves them
 * Out: as Hitbox_HitAllPlayerBoxes
 * Evidence: bra.w Hitbox_HitAllPlayerBoxes; reached by jmp $4E22(pc,d3.w) with d3 = 4
 */
void Hitbox_KindAllPlayers(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5,
                           uint32_t *d6, uint32_t *d7, uint32_t a0, uint32_t *a1, uint32_t a3, uint32_t *a4,
                           uint32_t a5, uint32_t *a6)
{
    (void)d0, (void)d1, (void)d2, (void)d3, (void)d4, (void)d5, (void)d6, (void)d7;
    (void)a0, (void)a1, (void)a3, (void)a4, (void)a5, (void)a6;
    dk::jump(at_Hitbox_HitAllPlayerBoxes);
}

/* Hitbox_KindFirstEnemy - kind-table entry 8 (the players' attacks): Hitbox_HitFirstEnemyBox.
 * In:  as Hitbox_TestAttackBoxAt leaves them
 * Out: as Hitbox_HitFirstEnemyBox
 * Evidence: bra.w Hitbox_HitFirstEnemyBox; reached by jmp $4E22(pc,d3.w) with d3 = 8
 */
void Hitbox_KindFirstEnemy(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5,
                           uint32_t *d6, uint32_t *d7, uint32_t a0, uint32_t *a1, uint32_t a3, uint32_t *a4,
                           uint32_t a5, uint32_t *a6)
{
    (void)d0, (void)d1, (void)d2, (void)d3, (void)d4, (void)d5, (void)d6, (void)d7;
    (void)a0, (void)a1, (void)a3, (void)a4, (void)a5, (void)a6;
    dk::jump(at_Hitbox_HitFirstEnemyBox);
}
