#include "game.h"

/* ToyFigure_FxSpinOrKill, ToyChain_SpawnInline and ToyChain_KillChild ($0370DC-$0371E5). */

/* the toy figure's rotation frames (ROM data) */
static constexpr uint32_t Tbl_036DA8 = 0x036DA8u;

enum {
    TOY_STEPS = 60,                /* rotation steps of a full turn */
    TOY_FX_SPIN = 5,               /* steps an effect figure turns per frame (30 degrees) */
    TOY_FX_SCREEN_W = 0x170,       /* deleted once x - g_OnScreenLeft is past this (unsigned) */
    TOY_FX_BOTTOM = 0xE6,          /* or its y this low */
    TOY_FX_ATTR_KEEP = 0x60,       /* the attribute bits it keeps (the palette) */
    CHAIN_BLOCKS = 5,              /* the chain object's pool blocks: a6/a3/a4/a5 + the chain block */
    CHAIN_OWNER_ID = 0x0B,         /* a boss-class owner id */
    CHAIN_TOUCH_PATTERN = 0xAAAA,  /* the links tested on alternate frames (ToyChain_LinkTouchAttack) */
    CHAIN_HEADING_FRAC = 0x8000,   /* the base heading's fraction: half a unit */
};

static void set_low_word(uint32_t *r, uint16_t v) { *r = (*r & 0xFFFF0000u) | v; }

/* ToyFigure_FxSpinOrKill - a4-script native of a toy figure flung off as an effect: deletes it
 * (drops the native's return, then Op4_Kill) once it leaves the screen (x - g_OnScreenLeft >= $170
 * unsigned, or y >= $E6), else turns it 5 steps (30 degrees) a frame through Tbl_036DA8
 * In:  x/y = its drawn position (d4/d5, from the motion op before it); fig = the a4 object (a6:
 *      +$1C the rotation step, a byte)
 * Out: kill: Op4_Kill's. Else fig +$1C = the step, frame = the step's frame, the attribute byte =
 *      (byte & $60 & mask) ^ flip; d0 = that byte (the whole long), d1's low byte = the flip, a0 =
 *      past the step; the condition codes of the move.b to the attributes
 * Evidence: move.w d4,d0 / sub.w g_OnScreenLeft,d0 / cmpi.w #$170 / bcc / cmpi.w #$E6,d5 / blt /
 *           addq.l #4,a7 / jmp Op4_Kill; moveq #0,d0 / move.b $1C(a6),d0 / addq.b #5 / the mod-60
 *           loop / lea Tbl_036DA8 / move.w (a0)+,$1E(a6) / andi.b #$60 / and.b (a0)+ / eor.b /
 *           move.b d0,$22(a6); Op4_CallNative (dc.l $0370DC) at $037012; TAS f79698-f112962 (6940
 *           frames). The prototype gives the condition codes as &ccr (measured ccout NZVC)
 */
void ToyFigure_FxSpinOrKill(uint32_t *d0, uint8_t *d1, uint16_t *d2, uint16_t x, uint16_t y, uint32_t *a0,
                            uint32_t *a1, Vm4Obj *fig, dk::Flags *ccr)
{
    uint16_t dx = (uint16_t)(x - g_OnScreenLeft + WS_WIN_L());   /* native: the window widened to the view (R4, R20) */
    set_low_word(d0, dx);
    if (dx >= (uint16_t)(TOY_FX_SCREEN_W + (WS_WIN_L() + WS_WIN_R())) || (int16_t)y >= TOY_FX_BOTTOM) {
        dk::drop_return();
        dk::jump(at_Op4_Kill);
        return;
    }
    uint8_t step = (uint8_t)(fig->anim_tick + TOY_FX_SPIN);   /* +$1C: the rotation step here */
    while (step >= TOY_STEPS)
        step = (uint8_t)(step - TOY_STEPS);
    fig->anim_tick = step;
    PTR(const ToyFrameStep) entry;
    entry.v = Tbl_036DA8 + (uint32_t)step * 4u;
    fig->frame = entry->frame;
    uint8_t attr = (uint8_t)(((uint16_t)fig->attr >> 8) & TOY_FX_ATTR_KEEP & entry->attr_mask);
    *d1 = entry->flip;
    attr ^= entry->flip;
    fig->attr = (uint16_t)((uint16_t)attr << 8 | ((uint16_t)fig->attr & 0xFFu));   /* the high byte */
    *d0 = attr;   /* moveq #0 cleared it; step * 4 < $100, so only the byte remains */
    *a0 = entry.v + sizeof(ToyFrameStep);
    *ccr = dk::Flags::test(attr, 1, ccr->x);
}

/* ToyChain_SpawnInline - inline-argument spawn of a toy-figure chain, called by jsr with dc.w link
 * count, dc.l parameter table, dc.l a2 script after it: makes an unlinked 5-block object (the
 * chain block at +$E) and fills the chain block from the table, then returns to the caller's
 * caller (past the arguments: the caller is a stub whose bytes after the jsr are only them)
 * In:  the return address = the inline arguments
 * Out: d0 = 0 (tst: Z set) with a0 = the chain block, a1 = past the table, a2 = past the
 *      arguments; or Obj_SpawnUnlinked's nonzero d0 on a full pool. a3-a6 / d1 / d7 =
 *      Obj_SpawnUnlinked's; d4 = d5 = 0
 * Evidence: movea.l (a7)+,a2 / moveq #0,d4 / moveq #0,d5 / lea VM2_RunObject,a0 / moveq #5,d7 / jsr
 *           Obj_SpawnUnlinked / bne / movea.w $E(a6),a0 / jsr ObjPool_Clear / the table copy /
 *           moveq #0,d0; 46 jsr sites $0398BA-$03B0EE. The prototype adds tst(d0.l): the moveq's Z
 *           (ccout NZVC), and on a full pool Obj_SpawnUnlinked's moveq #-1
 */
void ToyChain_SpawnInline(uint32_t *d0, uint16_t *d1, uint32_t *d4, uint32_t *d5, uint32_t *d7, uint32_t *a0,
                          uint32_t *a1, const u16 **args, uint32_t *a3, uint32_t *a4, uint32_t *a5, uint32_t *a6)
{
    *args = dk::return_data<u16>();
    dk::drop_return();   /* the rts goes to the caller's caller */
    *d4 = 0;
    *d5 = 0;
    *a0 = at_VM2_RunObject;   /* the object's handler */
    *d7 = CHAIN_BLOCKS;
    uint16_t blocks = (uint16_t)*d7;
    Obj_SpawnUnlinked(d0, d1, 0, 0, &blocks, a0, a3, a4, a5, a6);
    *d7 = (*d7 & 0xFFFF0000u) | blocks;
    if (*d0 != 0)
        return;
    PTR(Vm2Obj) self;
    self.v = *a6;
    PTR(Vm2Block) vm;
    vm.v = *a4;
    PTR(Vm2Sprite) spr;
    spr.v = *a5;
    ToyChainBlock *chain = toy_chain_block(self);
    *a0 = dk::addr(chain);
    ObjPool_Clear(d0, a0);
    self->owner_id = CHAIN_OWNER_ID;
    uint16_t links = *(*args)++;
    set_low_word(d0, links);
    PTR(const u16) table;
    table.v = (uint32_t)(*args)[0] << 16 | (*args)[1];
    *args += 2;
    vm->resume.v = (uint32_t)(*args)[0] << 16 | (*args)[1];
    *args += 2;
    spr->var[0] = (int16_t)CHAIN_TOUCH_PATTERN;
    chain->grow_left = links;
    chain->links_max = links;
    vm->var[1] = 0;                 /* +$24: the link count */
    const u16 *p = table;
    vm->var[0] = *p++;              /* +$22: the mode bits */
    /* move.w into the longs' high words: the integer parts */
    chain->sway_base = (int32_t)((uint32_t)*p++ << 16 | ((uint32_t)chain->sway_base & 0xFFFFu));
    set_low_word(d0, *p++);
    *d0 = (*d0 << 16) | CHAIN_HEADING_FRAC;   /* swap / move.w #$8000 */
    chain->heading = (int32_t)*d0;
    chain->sway_scale = (int16_t)*p++;
    chain->extension = (int32_t)((uint32_t)*p++ << 16 | ((uint32_t)chain->extension & 0xFFFFu));
    *d0 = (uint32_t)((int32_t)(int16_t)*p++ << 8);
    chain->grow_speed = (int32_t)*d0;
    *d0 = (uint32_t)((int32_t)(int16_t)*p++ << 8);
    chain->regrow_speed = (int32_t)*d0;
    uint16_t energy = *p++;
    set_low_word(d0, energy);
    chain->link_energy = energy;
    self->energy = (int16_t)energy;
    chain->link_damage = *p++;
    chain->link_knockback = *p++;
    chain->link_hit = (uint32_t)p[0] << 16 | p[1];
    p += 2;
    chain->blink_reload = *p++;
    chain->blink_pattern = *p++;
    *a1 = dk::addr(p);
    *d0 = 0;
}

/* ToyChain_KillChild - Op2_CallNativeOnChild callback that destroys a chain child: clears the
 * owner's field, removes every link (frames, boxes, blocks) and destroys the chain object
 * In:  d0 = the owner's child field offset; a0 = the chain object; a6 = the owner
 * Out: Obj_DestroyLoaded's (it destroys a6 = the chain object and pops the a1-a6 pushed here)
 * Evidence: clr.w (a6,d0.w) / movem.l a1-a6,-(a7) / movea.l a0,a6 / movea.w 8/$A/$C(a6),a3/a4/a5 /
 *           bsr ToyChain_RemoveAllLinks / jmp Obj_DestroyLoaded; dc.l $0371C6 after
 *           Op2_CallNativeOnChild in Scr2_037B12+$4C and Scr2_037BC4+$A4/$2F0/$2F8. The movem's
 *           restore is Obj_DestroyLoaded's: the same stack image is pushed
 */
void ToyChain_KillChild(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5,
                        uint32_t *d6, uint32_t *d7, uint32_t *a0, uint32_t *a1, uint32_t *a2, uint32_t *a3,
                        uint32_t *a4, uint32_t *a5, uint32_t *a6)
{
    PTR(u16) field;
    field.v = *a6 + (uint32_t)(int32_t)(int16_t)(uint16_t)*d0;
    *field = 0;
    /* movem.l a1-a6,-(a7): a6 at the highest address */
    dk::push_long(*a6);
    dk::push_long(*a5);
    dk::push_long(*a4);
    dk::push_long(*a3);
    dk::push_long(*a2);
    dk::push_long(*a1);
    *a6 = *a0;
    PTR(Vm2Obj) chain;
    chain.v = *a6;
    *a3 = (uint32_t)(int32_t)chain->motion;
    *a4 = (uint32_t)(int32_t)chain->vm;
    *a5 = (uint32_t)(int32_t)chain->sprite;
    uint16_t frame = (uint16_t)*d2;
    PTR(Vm2Block) vm;
    vm.v = *a4;
    ToyChain_RemoveAllLinks(d0, &frame, a1, vm, chain);
    *d2 = (*d2 & 0xFFFF0000u) | frame;
    dk::jump(at_Obj_DestroyLoaded);
}
