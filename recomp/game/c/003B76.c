#include "game.h"

/* $003B76-$003CC9: Op2_GosubVector2Plus, the extra-sprite-block ops (an object can own more
 * sprite blocks than its own a5, their handles in a6 fields named by a word operand: a byte offset
 * into the object), the frame release ops, and three position ops (jitter, push / pop on the
 * 68000 stack) plus Op2_AimDirAtPlayer. */

/* NEXT: the handler the script's next word names, jumped to */
static uint32_t op_next(const u16 **script)
{
    uint32_t handler = dk::code((int16_t)(uint16_t)*(*script)++);
    dk::jump(handler);
    return handler;
}

/* the handle word at a6 + offset (move.w (a6,d0.w)) */
static s16 &handle_field(Vm2Obj *self, uint16_t offset)
{
    return *(s16 *)((u8 *)self + (int16_t)offset);
}

/* a sprite block's first $2A bytes (the frame slots, their count, the blink / shake timers, the
 * flags, the aim, the animation timers and variables) cleared: ten longs and a word */
static void clear_spr_head(Vm2Sprite *spr)
{
    u8 *p = (u8 *)spr;
    for (int i = 0; i < 0x2A; i++)
        p[i] = 0;
}

/* the object's 16.16 position (x at +$3E, y at +$42) */
static uint32_t pos_x(const Vm2Obj *o) { return (uint32_t)(uint16_t)o->x << 16 | o->x_frac; }
static uint32_t pos_y(const Vm2Obj *o) { return (uint32_t)(uint16_t)o->y << 16 | o->y_frac; }
static void set_pos_x(Vm2Obj *o, uint32_t v) { o->x = (int16_t)(v >> 16); o->x_frac = (uint16_t)v; }
static void set_pos_y(Vm2Obj *o, uint32_t v) { o->y = (int16_t)(v >> 16); o->y_frac = (uint16_t)v; }

/* Op2_GosubVector2Plus - a2 op: calls script vector 2 plus a word displacement as a subroutine:
 * the op after the displacement is pushed for Op2_Return.
 * In:  script = word displacement (bytes, signed); self->vector[2]
 * Out: (sp) = the address after the displacement; NEXT at vector 2 + displacement
 * Evidence: pea.l $2(a2) / movea.w (a2),a2 / adda.l $3A(a6),a2 / NEXT; 19 corpus runs;
 *           $1B06A `3B76 0004 2D22 001C`
 */
void Op2_GosubVector2Plus(const u16 **script, Vm2Obj *self)
{
    dk::push_long(ADDR68(*script + 1));
    int16_t displacement = (int16_t)(uint16_t)**script;
    *script = (const u16 *)((const u8 *)(const u16 *)self->vector[2] + displacement);
    op_next(script);
}

/* Op2_AllocSprBlock - a2 op: allocates an extra sprite block from the object pool, stores its
 * handle in the a6 field the operand names and makes it the current a5: depth 1, attribute
 * g_SceneSprAttr, its first $2A bytes cleared. With the pool empty the script backs up to this op
 * and yields, to retry next frame.
 * In:  script = word field offset; self
 * Out: a5 = the block, d0 = 0, handler = NEXT's; pool empty: d0 = -1, script on the op word,
 *      Op2_Yield
 * Evidence: bsr.w ObjPool_Alloc / bne.s (subq.l #2,a2 / bra.w Op2_Yield) / move.w a5,(a6,d0.w) /
 *           move.w #1,$3A(a5) / move.w (g_SceneSprAttr).l,$38(a5) / ten move.l d0,(a0)+ and a
 *           move.w; 13 corpus runs; $180B6 `3B84 000E 0B16 18C0 0B4A 8000 3BF6`
 */
void Op2_AllocSprBlock(uint32_t *d0, uint32_t *a0, const u16 **script, uint32_t a4, uint32_t *a5, Vm2Obj *self)
{
    (void)a4;
    ObjPool_Alloc(d0, a0);
    if (*d0 != 0) {
        (*script)--;                       /* back to the op word: the yield resumes on this op */
        dk::jump(at_Op2_Yield);
        return;
    }
    uint16_t field = *(*script)++;
    *a5 = *a0;
    PTR(Vm2Sprite) block;
    block.v = *a5;
    handle_field(self, field) = (int16_t)(uint16_t)*a5;
    block->depth = 1;
    block->attr = g_SceneSprAttr;
    clear_spr_head(block);
    *d0 = 0;
    *a0 = op_next(script);
}

/* Op2_LoadA5Handle - a2 op: the sprite block whose handle is in the a6 field the operand names
 * becomes the current a5, so the sprite and animation ops act on it (Op2_ResetA5 undoes it).
 * In:  script = word field offset; self
 * Out: field = the offset; a5 = the block (sign-extended handle); NEXT
 * Evidence: move.w (a2)+,d0 / movea.w (a6,d0.w),a5 / NEXT; 27 corpus runs; $18120 `3BC2 000E 3E58 3BF6`
 */
void Op2_LoadA5Handle(uint16_t *field, const u16 **script, uint32_t *a5, Vm2Obj *self)
{
    *field = *(*script)++;
    *a5 = (uint32_t)(int32_t)handle_field(self, *field);
    op_next(script);
}

/* Op2_ResetSprBlock - a2 op: reinitialises the extra sprite block in the a6 field the operand
 * names: attribute g_SceneSprAttr, its first $2A bytes cleared (a5 is kept).
 * In:  script = word field offset; self
 * Out: the block reset; handler = NEXT's
 * Evidence: move.w (a2)+,d0 / movea.w (a6,d0.w),a0 / move.w (g_SceneSprAttr).l,$38(a0) / the
 *           clear of Op2_AllocSprBlock; 1 corpus run; $1C56A `3BCC 000E`
 */
void Op2_ResetSprBlock(uint32_t *handler, const u16 **script, Vm2Obj *self)
{
    Vm2Sprite *block = dk::handle<Vm2Sprite>(handle_field(self, *(*script)++));
    block->attr = g_SceneSprAttr;
    clear_spr_head(block);
    *handler = op_next(script);
}

/* Op2_ResetA5 - a2 op: the object's own sprite block becomes the current a5 again.
 * In:  self->sprite
 * Out: a5 = the block (sign-extended handle); NEXT
 * Evidence: movea.w $C(a6),a5 / NEXT (the handle VM2_RunObject loads); 21 corpus runs; follows
 *           Op2_LoadA5Handle / Op2_AllocSprBlock in $180C2, $18126
 */
void Op2_ResetA5(const u16 **script, uint32_t *a5, const Vm2Obj *self)
{
    *a5 = (uint32_t)(int32_t)self->sprite;
    op_next(script);
}

/* Op2_ReleaseFrames - a2 op: releases every sprite frame (VRAM art) the current sprite block
 * holds and zeroes its frame count (Obj_ReleaseFrames).
 * In:  spr = the sprite block (a5)
 * Out: Obj_ReleaseFrames' registers; NEXT
 * Evidence: bsr.w Obj_ReleaseFrames / NEXT; 2 corpus runs; $291C2 `3BFE 0EA6`, $3D0EE `3BFE 3C06 0010`
 */
void Op2_ReleaseFrames(uint16_t *d0, uint16_t *d2, uint16_t *count, u16 **slot, const u16 **script, Vm2Sprite *spr)
{
    Obj_ReleaseFrames(d0, d2, count, slot, spr);
    op_next(script);
}

/* Op2_ReleaseBlockFrames - a2 op: releases the sprite frames of the extra sprite block in the a6
 * field the operand names (a5 saved on the stack around it).
 * In:  script = word field offset; a5 (pushed, restored); self
 * Out: d0 = the offset, then Obj_ReleaseFrames' registers; handler = NEXT's
 * Evidence: move.l a5,-(a7) / move.w (a2)+,d0 / movea.w (a6,d0.w),a5 / bsr.w Obj_ReleaseFrames /
 *           movea.l (a7)+,a5 / NEXT; 6 corpus runs; $19A3E `3C06 000E`. Prototype: a5 an input
 *           (it was left out as unchanged): the push writes it to the stack
 */
void Op2_ReleaseBlockFrames(uint16_t *d0, uint16_t *d2, uint16_t *count, uint32_t *handler, u16 **slot,
                            const u16 **script, uint32_t a5, Vm2Obj *self)
{
    dk::push_long(a5);
    *d0 = *(*script)++;
    Vm2Sprite *block = dk::handle<Vm2Sprite>(handle_field(self, *d0));
    Obj_ReleaseFrames(d0, d2, count, slot, block);
    dk::pop_long();
    *handler = op_next(script);
}

/* Op2_SetBlockAnim - a2 op: starts an animation on the extra sprite block in an a6 field (as
 * Op2_SetAnim does for a5): its animation script = the relative target, timer cleared, done flag
 * cleared.
 * In:  script = word offset to the animation script (from the word itself), word field offset
 * Out: anim = the animation script's address; field = the field offset; NEXT
 * Evidence: movea.l a2,a0 / adda.w (a2)+,a0 / move.l a0,d0 / move.w (a2)+,d1 /
 *           movea.w (a6,d1.w),a0 / move.l d0,$2E(a0) / clr.w $1E(a0) / bclr #0,$18(a0); 5 corpus
 *           runs, 132 script uses (e.g. $180C8 `3C18 1C6A 000E`)
 */
void Op2_SetBlockAnim(uint32_t *anim, uint16_t *field, const u16 **script, Vm2Obj *self)
{
    const u16 *target = (const u16 *)((const u8 *)*script + (int16_t)(uint16_t)**script);
    (*script)++;
    *anim = ADDR68(target);
    *field = *(*script)++;
    Vm2Sprite *block = dk::handle<Vm2Sprite>(handle_field(self, *field));
    block->anim = target;
    block->anim_timer = 0;
    block->flags = (uint8_t)(block->flags & ~SPR_ANIM_DONE);
    op_next(script);
}

/* Op2_JitterPos - a2 op: moves the object by a random amount: x += Rand_Next * wx and
 * y += Rand_Next * wy in 16.16 (up to half the word in pixels either way).
 * In:  script = word wx, word wy
 * Out: self x / y; rand = the last product; NEXT
 * Evidence: bsr.w Rand_Next / muls.w (a2)+,d0 / add.l d0,$3E(a6), the same for $42(a6); 6 corpus
 *           runs; $3C610 `3C36 012C 0000`
 */
void Op2_JitterPos(uint32_t *rand, const u16 **script, Vm2Obj *self)
{
    *rand = (uint32_t)((int32_t)(int16_t)Rand_Next() * (int16_t)*(*script)++);
    set_pos_x(self, pos_x(self) + *rand);
    *rand = (uint32_t)((int32_t)(int16_t)Rand_Next() * (int16_t)*(*script)++);
    set_pos_y(self, pos_y(self) + *rand);
    op_next(script);
}

/* Op2_PushPos - a2 op: pushes the object's x and y (16.16 longs) on the 68000 stack for
 * Op2_PopPos later in the same run.
 * In:  self x / y
 * Out: (sp) = y, 4(sp) = x; NEXT
 * Evidence: move.l $3E(a6),-(a7) / move.l $42(a6),-(a7) / NEXT; $1D930 `3C4E 3C36 0004 0004 2D10
 *           EC90 3C5A` (save, jitter, gosub, restore); 3 corpus runs
 */
void Op2_PushPos(const u16 **script, const Vm2Obj *self)
{
    dk::push_long(pos_x(self));
    dk::push_long(pos_y(self));
    op_next(script);
}

/* Op2_PopPos - a2 op: restores the object's x and y that Op2_PushPos pushed.
 * In:  (sp) = y, 4(sp) = x
 * Out: self x / y; handler = NEXT's
 * Evidence: move.l (a7)+,$42(a6) / move.l (a7)+,$3E(a6) / NEXT; 3 corpus runs; $48620 `3C5A 0820`
 */
void Op2_PopPos(uint32_t *handler, const u16 **script, Vm2Obj *self)
{
    set_pos_y(self, dk::pop_long());
    set_pos_x(self, dk::pop_long());
    *handler = op_next(script);
}

/* Op2_AimDirAtPlayer - a2 op: points the sprite's aim at the targeted player: aim_dir = 4 x the
 * direction (of n) from the object to the player record's position (Mot_DirToPlayer). With no
 * target the operand is skipped (Op2_SkipWord).
 * In:  script = word n directions; vm->tracked (the targeted player record, 0 = none); self x / y
 * Out: d0 low word = the handle; a0 = the record (sign-extended), d2 = x then the direction x 4,
 *      d3 = y, d4 = n, d0 / d1 as Mot_DirToPlayer leaves them; spr->aim_dir; handler = NEXT's
 * Evidence: move.w $10(a4),d0 / beq.w Op2_SkipWord / movea.w d0,a0 / move.w $3E(a6),d2 /
 *           move.w $42(a6),d3 / move.w (a2)+,d4 / jsr (Mot_DirToPlayer).l / add.w d2,d2 x2 /
 *           move.w d2,$1A(a5); read by Op2_SwitchAimDir; 14 corpus runs; $1C0DA `3C66 0008`
 */
void Op2_AimDirAtPlayer(uint32_t *d0, uint32_t *d1, uint16_t *d2, uint16_t *d3, uint16_t *d4, uint32_t *a0,
                        const u16 **script, const Vm2Block *vm, Vm2Sprite *spr, const Vm2Obj *self)
{
    *d0 = (*d0 & 0xFFFF0000u) | (uint16_t)vm->tracked;
    if ((uint16_t)*d0 == 0) {
        dk::jump(at_Op2_SkipWord);
        return;
    }
    const PlayerRec *player = dk::handle<const PlayerRec>(vm->tracked);
    *a0 = ADDR68(player);
    *d2 = (uint16_t)self->x;
    *d3 = (uint16_t)self->y;
    *d4 = *(*script)++;
    Mot_DirToPlayer(d0, d1, d2, *d3, *d4, player);
    *d2 = (uint16_t)(*d2 * 4);
    spr->aim_dir = *d2;
    *a0 = op_next(script);
}
