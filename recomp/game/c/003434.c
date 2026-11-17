#include "game.h"

/* Op2_BranchPlayerInBox and Op2_SetPlayerWindowFromArgs ($003434-$0034A5). */

/* the low word of a data register, the upper word kept (a .w operation) */
static void set_low_word(uint32_t *r, uint16_t v) { *r = (*r & 0xFFFF0000u) | v; }

/* neg.w on a register's low word */
static void neg_w(uint32_t *r) { set_low_word(r, (uint16_t)(0u - (uint16_t)*r)); }

/* exg.l */
static void exg(uint32_t *a, uint32_t *b)
{
    uint32_t t = *a;
    *a = *b;
    *b = t;
}

/* NEXT: the handler the script's next word names, jumped to */
static void next(const u16 **script)
{
    dk::jump(dk::code((int16_t)(uint16_t)*(*script)++));
}

/* the sprite attributes' high byte: bit 3 = H flip, bit 4 = V flip */
enum { ATTR_HI_HFLIP = 1 << 3, ATTR_HI_VFLIP = 1 << 4 };

/* Op2_BranchPlayerInBox - a2 op: branches when the targeted player's collision box (its id = the
 * record's pad number +$3E) overlaps a rectangle relative to the object, mirrored by its H/V flip
 * bits. No target: the operands are skipped.
 * In:  script = words left, top, right, bottom (relative to self->x/y), word offset;
 *      vm->tracked = the targeted player record; spr->attr flips
 * Out: d0 = the record's handle (word), then -1 when found; d1/d2 = x/y, d3 = the box id;
 *      d4-d7 = the operand words (sign-extended), then the world rectangle (low words);
 *      found: the branch, else NEXT after the operands
 * Evidence: move.w $10(a4),d0 / beq / move.w $3E(a0),d3 / movem.w (a0)+,d4-d7 / btst #3,$38(a5)
 *           ... / bsr.w Hitbox_FindPlayerBoxOfId / bne.s (movea.l a0,a2 / adda.w (a2),a2) /
 *           lea $2(a0),a2; no target: adda.w #$A,a2; 43 runs; $1B09A `3434 FF6A FF79 0096 FFA1 023C`
 */
void Op2_BranchPlayerInBox(uint32_t *d0, uint16_t *d1, uint16_t *d2, uint16_t *d3, uint32_t *d4, uint32_t *d5,
                           uint32_t *d6, uint32_t *d7, const u16 **script, const Vm2Block *vm, const Vm2Sprite *spr,
                           const Vm2Obj *self)
{
    set_low_word(d0, (uint16_t)vm->tracked);
    if ((uint16_t)*d0 == 0) {
        *script += 5;
        next(script);
        return;
    }
    *d3 = dk::handle<PlayerRec>(vm->tracked)->pad;
    *d1 = (uint16_t)self->x;
    *d2 = (uint16_t)self->y;
    const u16 *op = *script;
    uint32_t *words[4] = { d4, d5, d6, d7 };
    for (int i = 0; i < 4; i++)
        *words[i] = (uint32_t)(int32_t)(int16_t)(uint16_t)op[i];   /* movem.w: sign-extended */
    const u16 *offset = op + 4;   /* a0 */
    uint8_t attr_hi = (uint8_t)(spr->attr >> 8);
    if (attr_hi & ATTR_HI_HFLIP) {
        neg_w(d4);
        neg_w(d6);
        exg(d4, d6);
    }
    if (attr_hi & ATTR_HI_VFLIP) {
        neg_w(d5);
        neg_w(d7);
        exg(d5, d7);
    }
    set_low_word(d4, (uint16_t)(*d4 + *d1));
    set_low_word(d6, (uint16_t)(*d6 + *d1));
    set_low_word(d5, (uint16_t)(*d5 + *d2));
    set_low_word(d7, (uint16_t)(*d7 + *d2));
    uint32_t box = 0;   /* a2 the box found: the op's own a2 (the script) is set below either way */
    Hitbox_FindPlayerBoxOfId(d0, *d3, (uint16_t)*d4, (uint16_t)*d5, (uint16_t)*d6, (uint16_t)*d7, &box);
    /* found = Z clear, which comes only with moveq #-1,d0; not found leaves d0 = the record's
       handle (never $FFFF in the low word), so d0 tells which */
    if (*d0 == 0xFFFFFFFFu) {
        *script = (const u16 *)((const u8 *)offset + (int16_t)(uint16_t)*offset);
        next(script);
        return;
    }
    *script = offset + 1;
    next(script);
}

/* Op2_SetPlayerWindowFromArgs - a2 op: sets the player-relative window of Op2_HoldPlayerWindow
 * from the subroutine arguments (x near, x far, y near, y far), then Mot_StorePlayerWindow.
 * In:  the high words of g_VM2Arg0..3 (from Op2_GosubArgs)
 * Out: d0-d3 = the four arguments; then Mot_StorePlayerWindow
 * Evidence: move.w (g_VM2Arg0).w,d0 ... move.w (g_VM2Arg3).w,d3 / bra.s Mot_StorePlayerWindow; 51 runs
 */
void Op2_SetPlayerWindowFromArgs(uint16_t *d0, uint16_t *d1, uint16_t *d2, uint16_t *d3, const u16 **script,
                                 Vm2Motion *mot)
{
    *d0 = (uint16_t)(g_VM2Arg0 >> 16);
    *d1 = (uint16_t)(g_VM2Arg1 >> 16);
    *d2 = (uint16_t)(g_VM2Arg2 >> 16);
    *d3 = (uint16_t)(g_VM2Arg3 >> 16);
    dk::jump(at_Mot_StorePlayerWindow);
}
