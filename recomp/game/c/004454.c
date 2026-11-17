#include "game.h"

/* $004454-$00463F: drawing an a2 object's sprite. Op2_Draw runs the sprite block's draw list (the
 * inline sprite ops of the frame Op2_AnimFrame showed) as a second script with a1 = the block's
 * frame slots: it pushes the object script's next handler and a2, and the list's Op2_DrawEnd pops
 * the a2 back and returns (rts) into that handler. Each Op2_DrawSpr* op draws one piece: the
 * frame slot it owns keeps its art loaded (Spr_ChangeFrame), then the piece is queued
 * (Spr_AddSorted). */

/* NEXT: the handler the script's next word names, jumped to */
static uint32_t op_next(const u16 **script)
{
    uint32_t handler = dk::code((int16_t)(uint16_t)*(*script)++);
    dk::jump(handler);
    return handler;
}

/* the integer part of a 16.16 camera coordinate (the word at its address) */
static uint16_t camera_word(uint32_t c) { return (uint16_t)(c >> 16); }

/* the low word / byte of a data register, the rest kept */
static void set_low_word(uint32_t *r, uint16_t v) { *r = (*r & 0xFFFF0000u) | v; }
static void set_low_byte(uint32_t *r, uint8_t v) { *r = (*r & 0xFFFFFF00u) | v; }

#if DK_MODE_NATIVE
/* ---- Phase 6 (ruling R11, criterion E3): the box widened for the wide view ----------------------
 * When the box culls the object on x alone and the wide view's box (x [left - reach, right +
 * reach], ws_draw_reach) keeps it, the draw Op2_Draw would make is read here, without running it:
 * the draw list's sprite ops give each piece's frame, position, attribute and depth, handed to
 * ws_draw as culled nodes (Spr_BuildSAT's walk draws them into the margins only, ws_draw.h). No
 * Spr_ChangeFrame (no art, no DMA budget, the frame slots untouched), no Spr_Add*, no register or
 * memory written: the game goes on exactly as the 4:3 cull leaves it. A list with any other op is
 * left undrawn (ws_draw_unsupported). Op2_Draw's blink is followed (not drawn on the frames it
 * hides the sprite); its shake is not. */
static constexpr uint32_t SPR_RAM_FRAMES_AT = 0xFFFFA764u;   /* frames < 0 (Spr_ChangeFrame) */

static uint32_t frame_entry_of(uint16_t frame)
{
    return ((int16_t)frame >= 0 ? SPR_ROM_FRAMES : SPR_RAM_FRAMES_AT) + (uint32_t)(int32_t)(int16_t)frame;
}

/* the attribute's high byte changed as attr_changed does (without its d0) */
static uint16_t ws_attr(uint16_t ops, const Vm2Sprite *spr)
{
    return (uint16_t)((((spr->attr >> 8) & (ops >> 8)) ^ (ops & 0xFF)) << 8);
}

/* a byte offset pair mirrored by the block's flips, from (x, y) */
static void ws_off(uint16_t offs, const Vm2Sprite *spr, int16_t *x, int16_t *y)
{
    int16_t dx = (int8_t)(offs >> 8), dy = (int8_t)(offs & 0xFF);
    if (spr->attr & 0x0800)
        dx = (int16_t)-dx;
    if (spr->attr & 0x1000)
        dy = (int16_t)-dy;
    *x = (int16_t)(*x + dx);
    *y = (int16_t)(*y + dy);
}

static void ws_draw_list(const Vm2Sprite *spr, const Vm2Obj *self)
{
    if (spr->invulnerable != 0 && g_FrameParity != 0)
        return;                                       /* Op2_Draw's blink: hidden this frame */
    struct Piece { int kind; uint16_t frame; int16_t x, y; uint16_t attr, key; } pc[16];
    unsigned n = 0;
    if (spr->draw.v == 0)
        return;                                       /* no frame shown yet */
    const u16 *op = spr->draw;
    for (unsigned steps = 0;; steps++) {
        uint16_t h = *op++;
        if (h == 0x44F2)                              /* Op2_DrawEnd */
            break;
        if (steps >= 16 || n >= 16) {
            ws_draw_unsupported();
            return;
        }
        Piece p = { SPR_HOST_SORTED, 0, self->x, self->y, spr->attr, spr->depth };
        switch (h) {
        case 0x4524: break;                                                   /* Op2_DrawSpr */
        case 0x4546: p.attr = ws_attr(*op++, spr); break;                     /* ...Attr */
        case 0x4570: p.key = (uint16_t)(p.key + *op++); break;                /* ...Depth */
        case 0x4594: p.attr = ws_attr(*op++, spr); p.key = (uint16_t)(p.key + *op++); break;
        case 0x45C0: ws_off(*op++, spr, &p.x, &p.y); break;                   /* ...Off */
        case 0x45FE: p.attr = ws_attr(*op++, spr); ws_off(*op++, spr, &p.x, &p.y); break;
        case 0x46CC: p.kind = SPR_HOST_FRONT; break;                          /* Op2_DrawSprFront */
        case 0x46EA: p.kind = SPR_HOST_FRONT; p.attr = ws_attr(*op++, spr); break;
        case 0x4710: p.kind = SPR_HOST_FRONT; ws_off(*op++, spr, &p.x, &p.y); break;
        case 0x478C: p.kind = SPR_HOST_BACK; break;                           /* Op2_DrawSprBack */
        case 0x47AA: p.kind = SPR_HOST_BACK; p.attr = ws_attr(*op++, spr); break;
        case 0x47D0: p.kind = SPR_HOST_BACK; ws_off(*op++, spr, &p.x, &p.y); break;
        case 0x480A: p.kind = SPR_HOST_BACK; p.attr = ws_attr(*op++, spr); ws_off(*op++, spr, &p.x, &p.y); break;
        default:                                      /* the AtDrawn forms and anything else */
            ws_draw_unsupported();
            return;
        }
        if (p.kind != SPR_HOST_SORTED)
            p.key = 0;
        p.frame = *op++;
        pc[n++] = p;
    }
    for (unsigned i = 0; i < n; i++)
        if (pc[i].frame != 0)
            ws_draw_node(pc[i].kind, frame_entry_of(pc[i].frame), pc[i].x, pc[i].y, pc[i].attr, pc[i].key,
                         ADDR68(self));
}

/* the culled op's box words (left, right, top, bottom): would the wide view's box draw it? */
static void ws_box(const u16 *box, const Vm2Sprite *spr, const Vm2Obj *self)
{
    if (!ws_draw_on())
        return;
    int16_t sx = (int16_t)(self->x - camera_word(g_CameraX)), sy = (int16_t)(self->y - camera_word(g_CameraY));
    int reach = ws_draw_reach();
    if (sx < (int16_t)box[0] - reach || sx > (int16_t)box[1] + reach)
        return;
    if (sy < (int16_t)box[2] || sy >= (int16_t)box[3])
        return;
    ws_draw_list(spr, self);
}
#define WS_BOX(box) ws_box((box), spr, self)
#else
#define WS_BOX(box) ((void)0)
#endif

/* Op2_DrawInScreenBox - a2 op: draws the sprite (Op2_Draw, which it falls into) only while its
 * screen position lies in x [left, right] and y [top, bottom).
 * In:  script = words left, right, top, bottom (screen pixels); self x / y; the camera
 * Out: d0 low word = the screen x (or y); drawn: Op2_Draw's; else handler = NEXT's after the words
 * Evidence: move.w $3E(a6),d0 / sub.w (g_CameraX).w,d0 / cmp.w (a2)+,d0 / blt / bgt, the same on
 *           y / blt.s Op2_Draw, the misses skipping the remaining words; 47 corpus runs;
 *           $13358 `4454 FFF6 014A FFF6 014E`
 */
void Op2_DrawInScreenBox(uint32_t *d0, uint32_t *a0, uint32_t *a1, const u16 **script, Vm2Sprite *spr, Vm2Obj *self)
{
    set_low_word(d0, (uint16_t)(self->x - camera_word(g_CameraX)));
    if ((int16_t)*d0 < (int16_t)*(*script)++) {
        WS_BOX(*script - 1);                          /* phase 6 R11: native only, reads only */
        *script += 3;
        *a0 = op_next(script);
        return;
    }
    if ((int16_t)*d0 > (int16_t)*(*script)++) {
        WS_BOX(*script - 2);
        *script += 2;
        *a0 = op_next(script);
        return;
    }
    set_low_word(d0, (uint16_t)(self->y - camera_word(g_CameraY)));
    if ((int16_t)*d0 < (int16_t)*(*script)++) {
        *script += 1;
        *a0 = op_next(script);
        return;
    }
    if ((int16_t)*d0 < (int16_t)*(*script)++) {
        Op2_Draw(d0, a0, a1, script, spr, self);
        return;
    }
    *a0 = op_next(script);
}

/* run the draw list: the object script's a2 pushed (Op2_DrawEnd pops it), a1 = the frame slots */
static void run_draw_list(uint32_t *a0, uint32_t *a1, const u16 **script, const Vm2Sprite *spr)
{
    dk::push_long(ADDR68(*script));
    *script = spr->draw;
    *a1 = ADDR68(spr);
    *a0 = op_next(script);
}

/* Op2_Draw - a2 op: draws the current sprite block by running its draw list. The blink timer
 * (+$16) hides it on every other frame while it counts down; the shake timer (+$17) moves the
 * object by a random -2..1 px for this draw (Op2_Draw_RestorePos puts it back).
 * In:  script = the next op; spr (draw list, timers); self x / y
 * Out: (sp) = the object script, then the return: the next op's handler (or Op2_Draw_RestorePos
 *      with the saved x / y words above it); a1 = the frame slots; jumped into the list. Blinked
 *      out: VM2_Next. Shaken: d0 = Rand_Next << 2, swapped
 * Evidence: tst.w $16(a5) / movea.w (a2)+,a0 / pea (a0) / pea (a2) / movea.l $2A(a5),a2 /
 *           lea (a5),a1 / NEXT; blink: subq.b #1,$16(a5) / tst.b (g_FrameParity).w /
 *           bne.w VM2_Next; shake: subq.b #1,$17(a5) / bsr.w Rand_Next / asl.l #2 / swap /
 *           x, y words pushed / add.w d0 to both / pea.l (Op2_Draw_RestorePos).l; 153 corpus runs
 */
void Op2_Draw(uint32_t *d0, uint32_t *a0, uint32_t *a1, const u16 **script, Vm2Sprite *spr, Vm2Obj *self)
{
    if (spr->invulnerable != 0 || spr->shake != 0) {         /* tst.w $16(a5) */
        if (spr->invulnerable != 0) {
            spr->invulnerable = (uint8_t)(spr->invulnerable - 1);
            if (g_FrameParity != 0) {
                dk::jump(at_VM2_Next);
                return;
            }
        }
        if (spr->shake != 0) {
            spr->shake = (uint8_t)(spr->shake - 1);
            uint32_t r = (uint32_t)Rand_Next() << 2;
            *d0 = (r << 16) | (r >> 16);
            dk::push_word((uint16_t)self->x);
            dk::push_word((uint16_t)self->y);
            self->x = (int16_t)(self->x + (uint16_t)*d0);
            self->y = (int16_t)(self->y + (uint16_t)*d0);
            dk::push_long(at_Op2_Draw_RestorePos);
            run_draw_list(a0, a1, script, spr);
            return;
        }
    }
    *a0 = (uint32_t)(int32_t)(int16_t)(uint16_t)*(*script)++;  /* movea.w (a2)+,a0 / pea (a0) */
    dk::push_long(*a0);
    run_draw_list(a0, a1, script, spr);
}

/* Op2_Draw_RestorePos - the continuation of a shaken Op2_Draw: restores the object's x / y words
 * saved before the jitter, then the script goes on. Entered by Op2_DrawEnd's rts.
 * In:  (sp) = the saved y word, then the x word; script = the object script
 * Out: self x / y; handler = NEXT's
 * Evidence: move.w (a7)+,$42(a6) / move.w (a7)+,$3E(a6) / NEXT; 77 corpus runs as a computed target
 */
void Op2_Draw_RestorePos(uint32_t *handler, const u16 **script, Vm2Obj *self)
{
    uint32_t saved = dk::pop_long();       /* the y word (lower address), then the x word */
    self->y = (int16_t)(uint16_t)(saved >> 16);
    self->x = (int16_t)(uint16_t)saved;
    *handler = op_next(script);
}

/* Op2_DrawEnd - draw-list op: ends a draw list: the object script's a2 back, the frame count =
 * the slots used this time, and the frames of the slots no longer used released and cleared.
 * In:  a1 = the next frame slot; (sp) = the object script, then the return Op2_Draw pushed; spr
 * Out: a2 = the object script; d0 = the slots used; d7 / d2 the release loop's; spr->frame_count;
 *      ccr = the last sub.w / subq.w / clr.w's; rts into the pushed return
 * Evidence: movea.l (a7)+,a2 / move.w a1,d0 / sub.w a5,d0 / asr.w #1,d0 / move.w $14(a5),d7 /
 *           move.w d0,$14(a5) / sub.w d0,d7 / bgt / rts; then Spr_ReleaseFrame + clr.w (a1)+ per
 *           slot (subq.w #2,d7 / bpl / dbra); 153 runs; 1837 script uses (`... 4524 2A80 44F2`)
 */
void Op2_DrawEnd(uint16_t *d0, uint16_t *d2, uint16_t *d7, uint32_t *a0, uint32_t *a1, uint32_t *a2, Vm2Sprite *spr,
                 dk::Flags *ccr)
{
    *a2 = dk::pop_long();
    *d0 = (uint16_t)((int16_t)(uint16_t)(*a1 - ADDR68(spr)) >> 1);
    uint16_t before = spr->frame_count;
    spr->frame_count = *d0;
    *d7 = (uint16_t)(before - *d0);
    *ccr = dk::Flags::compare(before, *d0, 2);
    ccr->x = ccr->c;
    if (!(!ccr->z && ccr->n == ccr->v))    /* bgt */
        return;
    PTR(u16) slot;
    slot.v = *a1;
    *d2 = *slot;
    Spr_ReleaseFrame(d0, d2, a0);
    slot.v = *a1;
    *slot = 0;
    *a1 += 2;
    uint16_t left = *d7;
    *d7 = (uint16_t)(left - 2);
    *ccr = dk::Flags::compare(left, 2, 2);
    ccr->x = ccr->c;
    if ((int16_t)*d7 < 0)                  /* bpl not taken */
        return;
    do {
        slot.v = *a1;
        *d2 = *slot;
        Spr_ReleaseFrame(d0, d2, a0);
        slot.v = *a1;
        *slot = 0;
        *a1 += 2;
        *ccr = dk::Flags::test(0, 2, ccr->x);   /* clr.w (a1)+ */
        *d7 = (uint16_t)(*d7 - 1);
    } while (*d7 != 0xFFFF);
}

/* one draw-list piece: the frame word into the slot a1 names (Spr_ChangeFrame keeps its art
 * loaded), the slot advanced, the piece queued at (x, y) with attr / depth (Spr_AddSorted), NEXT */
static uint32_t draw_piece(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint16_t x, uint16_t y,
                           uint16_t attr, uint16_t depth, uint32_t *a0, uint32_t *a1, const u16 **script)
{
    set_low_word(d1, *(*script)++);
    PTR(u16) slot;
    slot.v = *a1;
    set_low_word(d2, *slot);
    Spr_ChangeFrame(d0, d1, d2, d3, a0, a1);
    slot.v = *a1;
    *slot = (uint16_t)*d1;
    *a1 += 2;
    uint16_t w = (uint16_t)*d0;
    dk::Flags spr_ccr{};   /* its &ccr (Spr_* hand their condition codes back) */
    Spr_AddSorted(&w, d1, d2, x, y, attr, depth, *a0, *a1, ADDR68(*script), &spr_ccr);
    set_low_word(d0, w);
    return op_next(script);
}

/* the attribute's high byte changed: (byte & mask) ^ xor, in the high byte (lsl.w #8); d0's low
 * byte = xor (move.b (a2)+,d0) */
static uint16_t attr_changed(uint32_t *d0, const u16 **script, const Vm2Sprite *spr)
{
    uint16_t ops = *(*script)++;            /* byte mask, byte xor */
    set_low_byte(d0, (uint8_t)ops);
    return (uint16_t)((((spr->attr >> 8) & (ops >> 8)) ^ (ops & 0xFF)) << 8);
}

/* the object's position plus a byte offset pair, mirrored by the H / V flip bits (attr bits 11 /
 * 12: btst #3 / #4 of the high byte) */
static void offset_pos(uint16_t *x, uint16_t *y, const u16 **script, const Vm2Sprite *spr, const Vm2Obj *self)
{
    uint16_t offs = *(*script)++;           /* byte dx, byte dy */
    int16_t dx = (int8_t)(offs >> 8), dy = (int8_t)(offs & 0xFF);
    if (spr->attr & 0x0800)
        dx = (int16_t)-dx;
    if (spr->attr & 0x1000)
        dy = (int16_t)-dy;
    *x = (uint16_t)(dx + self->x);
    *y = (uint16_t)(dy + self->y);
}

/* Op2_DrawSpr - draw-list op: one sprite frame at the object's position with the block's
 * attribute and depth.
 * In:  script = word frame; a1 = the frame slot; spr, self
 * Out: d4 / d5 = x / y, d6 = attr, d7 = depth, d1 = the frame; the slot set, a1 + 2; the
 *      Spr_ChangeFrame / Spr_AddSorted registers; NEXT
 * Evidence: move.w $38(a5),d6 / move.w $3A(a5),d7 / move.w $3E(a6),d4 / move.w $42(a6),d5 /
 *           move.w (a2)+,d1 / move.w (a1),d2 / bsr.w Spr_ChangeFrame / move.w d1,(a1)+ /
 *           bsr.w Spr_AddSorted / NEXT; 152 runs; 1672 uses, e.g. $13462 `4524 2A80 44F2`
 */
void Op2_DrawSpr(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint16_t *d4, uint16_t *d5, uint16_t *d6,
                 uint16_t *d7, uint32_t *a1, const u16 **script, const Vm2Sprite *spr, const Vm2Obj *self)
{
    *d6 = spr->attr;
    *d7 = spr->depth;
    *d4 = (uint16_t)self->x;
    *d5 = (uint16_t)self->y;
    uint32_t a0 = 0;
    draw_piece(d0, d1, d2, d3, *d4, *d5, *d6, *d7, &a0, a1, script);
}

/* Op2_DrawSprAttr - draw-list op: Op2_DrawSpr with the attribute's high byte (priority, palette,
 * flips) changed: (attr & mask) ^ xor.
 * In:  script = byte mask, byte xor, word frame; a1 = the frame slot
 * Out: as Op2_DrawSpr, d6 = the changed attribute (low byte 0), d0's low byte = xor first
 * Evidence: move.b $38(a5),d6 / and.b (a2)+,d6 / move.b (a2)+,d0 / eor.b d0,d6 / lsl.w #8,d6, then
 *           Op2_DrawSpr's tail; 103 runs; $19B0A `4546 18C0 FC90`. Prototype: a2 in / out (it was
 *           `a2 ... clobbers(a2)`: its NEXT hands the advanced list to the next draw op)
 */
void Op2_DrawSprAttr(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint16_t *d4, uint16_t *d5, uint16_t *d6,
                     uint16_t *d7, uint32_t *a1, const u16 **script, const Vm2Sprite *spr, const Vm2Obj *self)
{
    *d7 = spr->depth;
    *d4 = (uint16_t)self->x;
    *d5 = (uint16_t)self->y;
    *d6 = attr_changed(d0, script, spr);
    uint32_t a0 = 0;
    draw_piece(d0, d1, d2, d3, *d4, *d5, *d6, *d7, &a0, a1, script);
}

/* Op2_DrawSprDepth - draw-list op: Op2_DrawSpr with a depth offset added to the block's depth
 * (this piece sorts in front of or behind the rest).
 * In:  script = word depth offset, word frame; a1 = the frame slot
 * Out: as Op2_DrawSpr, d7 = depth + offset; handler = NEXT's
 * Evidence: move.w $3A(a5),d7 / add.w (a2)+,d7, then Op2_DrawSpr's tail; 87 runs; $1B4FE `4570 1000 2FB2`
 */
void Op2_DrawSprDepth(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint16_t *d4, uint16_t *d5,
                      uint16_t *d6, uint16_t *d7, uint32_t *a0, uint32_t *a1, const u16 **script,
                      const Vm2Sprite *spr, const Vm2Obj *self)
{
    *d6 = spr->attr;
    *d4 = (uint16_t)self->x;
    *d5 = (uint16_t)self->y;
    *d7 = (uint16_t)(spr->depth + *(*script)++);
    *a0 = draw_piece(d0, d1, d2, d3, *d4, *d5, *d6, *d7, a0, a1, script);
}

/* Op2_DrawSprAttrDepth - draw-list op: Op2_DrawSpr with both the attribute change and a depth
 * offset.
 * In:  script = byte mask, byte xor, word depth offset, word frame; a1 = the frame slot
 * Out: as Op2_DrawSprAttr and Op2_DrawSprDepth; handler = NEXT's
 * Evidence: the Attr and Depth code combined; 2 runs; $19B18 `4594 18C0 FFCE FC9A`
 */
void Op2_DrawSprAttrDepth(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint16_t *d4, uint16_t *d5,
                          uint16_t *d6, uint16_t *d7, uint32_t *a0, uint32_t *a1, const u16 **script,
                          const Vm2Sprite *spr, const Vm2Obj *self)
{
    *d4 = (uint16_t)self->x;
    *d5 = (uint16_t)self->y;
    *d6 = attr_changed(d0, script, spr);
    *d7 = (uint16_t)(spr->depth + *(*script)++);
    *a0 = draw_piece(d0, d1, d2, d3, *d4, *d5, *d6, *d7, a0, a1, script);
}

/* Op2_DrawSprOff - draw-list op: Op2_DrawSpr at the object's position plus a signed byte offset,
 * mirrored by the attribute's H / V flip bits.
 * In:  script = byte dx, byte dy, word frame; a1 = the frame slot
 * Out: as Op2_DrawSpr, d4 / d5 = the offset position
 * Evidence: move.b (a2)+,d4 / move.b (a2)+,d5 / ext.w / btst #3 / #4,$38(a5) + neg.w /
 *           add.w $3E(a6),d4 / add.w $42(a6),d5; 109 runs; 322 uses, e.g. $19D1A `45C0 0100 FC7C`
 */
void Op2_DrawSprOff(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint16_t *d4, uint16_t *d5, uint16_t *d6,
                    uint16_t *d7, uint32_t *a1, const u16 **script, const Vm2Sprite *spr, const Vm2Obj *self)
{
    *d6 = spr->attr;
    *d7 = spr->depth;
    offset_pos(d4, d5, script, spr, self);
    uint32_t a0 = 0;
    draw_piece(d0, d1, d2, d3, *d4, *d5, *d6, *d7, &a0, a1, script);
}

/* Op2_DrawSprAttrOff - draw-list op: Op2_DrawSpr with the attribute change and a mirrored byte
 * offset.
 * In:  script = byte mask, byte xor, byte dx, byte dy, word frame; a1 = the frame slot
 * Out: as Op2_DrawSprAttr and Op2_DrawSprOff
 * Evidence: the Attr then Off code, Spr_AddSorted; 75 runs; $1366A `45FE F008 0100 2B16`
 */
void Op2_DrawSprAttrOff(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint16_t *d4, uint16_t *d5,
                        uint16_t *d6, uint16_t *d7, uint32_t *a1, const u16 **script, const Vm2Sprite *spr,
                        const Vm2Obj *self)
{
    *d7 = spr->depth;
    *d6 = attr_changed(d0, script, spr);
    offset_pos(d4, d5, script, spr, self);
    uint32_t a0 = 0;
    draw_piece(d0, d1, d2, d3, *d4, *d5, *d6, *d7, &a0, a1, script);
}
