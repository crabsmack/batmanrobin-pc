#include "game.h"

/* $006776-$00694D: the a4 draw ops. The Op4_DrawKill* variants first delete an object that is off
 * screen (or any object during a screen clear, Op4_Kill); Op4_Draw steps the animation ticker and,
 * on its carry, runs the anim script, whose frame ops show a frame and draw; Op4_DrawFrame swaps
 * the frame's art through the VRAM art cache when the wanted frame's art differs from the shown
 * one (keeping the old frame when the VRAM or DMA budget is short) and puts the sprite on the
 * display list by depth (0 front, 1 back, else sorted). Every one ends the frame's run. */

/* the frame tables: frames >= 0 index Tbl_1DDB60 (ROM), negative ones the RAM table below
 * g_ArtRelRingWr; each entry starts with the word handle of its art record */
static constexpr uint32_t Tbl_1DDB60 = 0x1DDB60u;

static uint32_t sext16(uint32_t v) { return (uint32_t)(int32_t)(int16_t)(uint16_t)v; }
static void set_low_word(uint32_t *r, uint16_t v) { *r = (*r & 0xFFFF0000u) | v; }
static void set_low_byte(uint32_t *r, uint8_t v) { *r = (*r & 0xFFFFFF00u) | v; }

/* word / byte / long at a 68000 address held in a register */
static uint16_t rd16(uint32_t a) { PTR(const u16) p; p.v = a; return *p; }
static void wr16(uint32_t a, uint16_t v) { PTR(u16) p; p.v = a; *p = v; }
static uint8_t rd8(uint32_t a) { PTR(const u8) p; p.v = a; return *p.get(); }
static void wr8(uint32_t a, uint8_t v) { PTR(u8) p; p.v = a; *p.get() = v; }
static uint32_t rd32(uint32_t a) { PTR(const u32) p; p.v = a; return *p; }

/* the frame entry of frame f (a word offset, sign-extended) */
static uint32_t frame_entry(uint16_t f)
{
    uint32_t base = (int16_t)f < 0 ? ADDR68(&g_ArtRelRingWr) : Tbl_1DDB60;
    return base + sext16(f);
}

/* Op4_DrawFrame - a4 op / the draw tail: when the wanted frame's art differs from the shown one,
 * swaps it (the art cache's reference counts; a new art record is allocated and its DMA queued
 * when VRAM blocks, descriptors and the DMA budget allow, else the old frame stays shown); then
 * adds the sprite at d4 / d5 with the object's attributes to the display list by its depth.
 * In:  a6 = the object (frame, frame_shown, attr, depth); d4 / d5 = the x / y words
 * Out: frame_shown; the art cache's counts; d6 = attr, d7 = depth (- 1); the registers the swap
 *      leaves; ends the run (Spr_AddFront / Spr_AddBack / Spr_AddSorted's rts)
 * Evidence: the Spr_SwapFrameArt logic inline (Tbl_1DDB60 / the RAM table at g_ArtRelRingWr,
 *           ArtCache_Alloc, Spr_QueueArtDMA, g_ArtBlocksFree / g_ArtDescsFree, the release ring);
 *           move.w $24(a6),d7 / beq.w Spr_AddFront / subq.w #1 / beq.w Spr_AddBack / bra.w
 *           Spr_AddSorted; script use after Op4_CallNative `5438 0003 70DC 6834`; 32 corpus runs
 */
void Op4_DrawFrame(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5,
                   uint32_t *d6, uint32_t *d7, uint32_t *a0, uint32_t *a1, uint32_t *a2, uint32_t *a3,
                   uint32_t *a4, uint32_t a6)
{
    PTR(Vm4Obj) self;
    self.v = a6;
    set_low_word(d6, self->frame);
    set_low_word(d7, self->frame_shown);
    *a0 = frame_entry((uint16_t)*d6);
    *a1 = frame_entry((uint16_t)*d7);
    set_low_word(d3, rd16(*a1));
    bool keep_old = false;
    if ((uint16_t)*d3 != rd16(*a0)) {
        /* a different art record: take a reference on the wanted one ($689E) */
        *a2 = sext16(rd16(*a0));
        set_low_word(d0, rd16(*a2));
        bool referenced = true;
        if ((uint16_t)*d0 != 0) {
            /* its art is loaded: one more user ($68F2): the count byte goes down (-1 = unused) */
            *a2 = sext16(*d0);
            uint8_t count = rd8(*a2);
            uint8_t left = (uint8_t)(count - 1);
            wr8(*a2, left);
            *a2 += 1;
            if (count == 0) {
                /* it was unused, waiting in the release ring: its blocks and descriptor taken back */
                set_low_byte(d0, rd8(*a2));
                g_ArtBlocksFree = (uint8_t)(g_ArtBlocksFree - (uint8_t)*d0);
                g_ArtDescsFree = (uint8_t)(g_ArtDescsFree - 1);
            } else if (!(left & 0x80)) {
                wr8(*a2, (uint8_t)(rd8(*a2) + 1));
            }
        } else if ((uint16_t)*d6 != 0) {
            /* not loaded: allocate and upload it if VRAM, descriptors and DMA allow */
            *a4 = rd32(*a0 + 6);
            *d0 = sext16(rd16(*a4));
            *d1 = sext16(rd16(*a4 + 2));
            set_low_word(d0, (uint16_t)(*d0 + *d1));
            bool fits = (int16_t)(uint16_t)*d0 <= (int16_t)g_DMABudgetLeft &&
                        (int16_t)(uint16_t)*d1 <= (int16_t)g_DMAQueueFree;
            if (fits) {
                uint8_t blocks = g_ArtBlocksFree;
                g_ArtBlocksFree = (uint8_t)(blocks - (uint8_t)*d1);
                if (blocks < (uint8_t)*d1) {
                    fits = false;                                  /* bcs: no VRAM */
                } else {
                    uint8_t descs = g_ArtDescsFree;
                    g_ArtDescsFree = (uint8_t)(descs - 1);
                    if (descs == 0) {
                        g_ArtDescsFree = 0;                        /* bcs: no descriptor */
                        fits = false;
                    }
                }
                if (!fits)
                    g_ArtBlocksFree = (uint8_t)(g_ArtBlocksFree + (uint8_t)*d1);
            }
            if (!fits) {
                /* keep showing the old frame */
                *a0 = *a1;
                self->frame_shown = (uint16_t)*d7;
                keep_old = true;
                referenced = false;
            } else {
                uint32_t wanted = *a0;   /* move.l a0,usp */
                /* movem.w d3-d6,-(a7) ... movem.w (a7)+,d3-d6 around the two calls */
                dk::push_word((uint16_t)*d6);
                dk::push_word((uint16_t)*d5);
                dk::push_word((uint16_t)*d4);
                dk::push_word((uint16_t)*d3);
                *a0 = *a2;
                uint16_t w1 = (uint16_t)*d1;
                ArtCache_Alloc(&w1, d2, d3, d4, d5, d6, d7, *a0, a1, a3);
                set_low_word(d1, w1);
                Spr_QueueArtDMA(&w1, d2, d3, d7, a1, a3, a4);
                set_low_word(d1, w1);
                uint32_t hi = dk::pop_long();
                uint32_t lo = dk::pop_long();
                *d3 = sext16(hi >> 16);
                *d4 = sext16(hi);
                *d5 = sext16(lo >> 16);
                *d6 = sext16(lo);
                *a0 = wanted;            /* move.l usp,a0 */
            }
        }
        if (referenced) {
            /* drop the reference on the old art ($6908): its count byte goes up; at 0 (unused) its
               blocks and descriptor are released and it joins the release ring */
            *a2 = sext16(*d3);
            set_low_word(d0, rd16(*a2));
            if ((uint16_t)*d0 != 0) {
                *a2 = sext16(*d0);
                uint8_t count = (uint8_t)(rd8(*a2) + 1);
                wr8(*a2, count);
                *a2 += 1;
                if (count == 0) {
                    g_ArtDescsFree = (uint8_t)(g_ArtDescsFree + 1);
                    set_low_byte(d2, rd8(*a2));
                    *a2 += 1;
                    g_ArtBlocksFree = (uint8_t)(g_ArtBlocksFree + (uint8_t)*d2);
                    if (rd16(*a2) == 0) {
                        *a3 = sext16(g_ArtRelRingWr);
                        g_ArtRelRingWrLo = (uint8_t)(g_ArtRelRingWrLo + 2);
                        wr16(*a2, (uint16_t)*a3);
                        wr16(*a3, (uint16_t)*d0);
                    }
                } else if (!(count & 0x80)) {
                    wr8(*a2 - 1, 0);
                }
            }
        }
    }
    if (!keep_old)
        self->frame_shown = (uint16_t)*d6;
    set_low_word(d6, self->attr);
    set_low_word(d7, self->depth);
    if ((uint16_t)*d7 == 0) {
        dk::jump(at_Spr_AddFront);
        return;
    }
    set_low_word(d7, (uint16_t)(*d7 - 1));
    if ((uint16_t)*d7 == 0) {
        dk::jump(at_Spr_AddBack);
        return;
    }
    dk::jump(at_Spr_AddSorted);
}

/* Op4_Draw - a4 op: steps the animation ticker (accumulator += rate) and on its carry runs the
 * anim script (whose frame op draws); otherwise draws the current frame (Op4_DrawFrame). Ends the
 * frame either way.
 * In:  script (no operands); a6 = the object; d4 / d5 = the x / y words
 * Out: self->anim_tick; d0's low byte = the rate; a4 = the anim script (NEXT), or Op4_DrawFrame's
 * Evidence: move.b $1D(a6),d0 / add.b d0,$1C(a6) / bcc.s Op4_DrawFrame / movea.l $C(a6),a4 / NEXT;
 *           117 script uses, e.g. $108E2 (`634E 67EE`); 151 corpus runs
 */
void Op4_Draw(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6,
              uint32_t *d7, uint32_t *a0, uint32_t *a1, uint32_t *a2, uint32_t *a3, uint32_t *a4, uint32_t a6)
{
    PTR(Vm4Obj) self;
    self.v = a6;
    set_low_byte(d0, self->anim_rate);
    unsigned sum = (unsigned)self->anim_tick + (uint8_t)*d0;
    self->anim_tick = (uint8_t)sum;
    if (sum < 0x100) {   /* no carry */
        Op4_DrawFrame(d0, d1, d2, d3, d4, d5, d6, d7, a0, a1, a2, a3, a4, a6);
        return;
    }
    *a4 = self->anim.raw();
    *a0 = dk::code((int16_t)rd16(*a4));
    *a4 += 2;
    dk::jump(*a0);
}

/* the kill tail: Op4_Kill deletes the object (box, frame art, block) and ends the run */
static void kill(void)
{
    dk::jump(at_Op4_Kill);
}

/* Op4_DrawKillSidesBelow - a4 op: kills the object when d4 / d5 is off the sides or below the
 * on-screen window (above the top is kept); else draws (Op4_Draw).
 * In:  d4 / d5 = the drawn x / y words; script; a6 = the object
 * Out: d0's low word = the window offset; Op4_Kill or Op4_Draw's
 * Evidence: move.w d4,d0 / sub.w (g_OnScreenLeft).w,d0 / cmpi.w #$170,d0 / bcc.w Op4_Kill /
 *           sub.w (g_OnScreenTop).w / cmpi.w #$110 / blt.s Op4_Draw / bra.w Op4_Kill; an entry after
 *           the screen-clear test (computed-jump target in 43 corpus runs); 19 script uses
 */
void Op4_DrawKillSidesBelow(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5,
                            uint32_t *d6, uint32_t *d7, uint32_t *a0, uint32_t *a1, uint32_t *a2, uint32_t *a3,
                            uint32_t *a4, uint32_t a6)
{
    set_low_word(d0, (uint16_t)(*d4 - g_OnScreenLeft + WS_WIN_L()));   /* native: the window widened to the view (R4, R20) */
    if ((uint16_t)*d0 >= (uint16_t)(0x170 + (WS_WIN_L() + WS_WIN_R()))) {
        kill();
        return;
    }
    set_low_word(d0, (uint16_t)(*d5 - g_OnScreenTop));
    if ((int16_t)(uint16_t)*d0 < 0x110) {
        Op4_Draw(d0, d1, d2, d3, d4, d5, d6, d7, a0, a1, a2, a3, a4, a6);
        return;
    }
    kill();
}

/* Op4_DrawKillSidesBelowOrClear - a4 op: Op4_DrawKillSidesBelow, and the object is also deleted
 * during a screen clear.
 * In:  as Op4_DrawKillSidesBelow; g_ScreenClearTimer
 * Out: Op4_Kill, or Op4_DrawKillSidesBelow's
 * Evidence: tst.w (g_ScreenClearTimer).w / bne.w Op4_Kill, falling into Op4_DrawKillSidesBelow;
 *           script uses $1ABAA, $1F116; 6 corpus runs
 */
void Op4_DrawKillSidesBelowOrClear(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4,
                                   uint32_t *d5, uint32_t *d6, uint32_t *d7, uint32_t *a0, uint32_t *a1,
                                   uint32_t *a2, uint32_t *a3, uint32_t *a4, uint32_t a6)
{
    if (g_ScreenClearTimer != 0) {
        kill();
        return;
    }
    Op4_DrawKillSidesBelow(d0, d1, d2, d3, d4, d5, d6, d7, a0, a1, a2, a3, a4, a6);
}

/* Op4_DrawKillOutside - a4 op: kills the object when its drawn point is outside a
 * camera-relative rectangle (signed screen px); else draws.
 * In:  script = words x lo, x hi, y lo, y hi; d4 / d5 = the drawn x / y words; a6 = the object
 * Out: d0's low word = the screen y (or x); Op4_Kill or Op4_Draw's
 * Evidence: sub.w (g_CameraX).w,d0 / cmp.w (a4)+,d0 / blt.w Op4_Kill / cmp.w (a4)+,d0 / bgt.w ... /
 *           ble.s Op4_Draw; an entry after the screen-clear test (computed-jump target in 17 corpus
 *           runs); script uses $1FF9C (`67A4 FFB0 03E8 0000 00E0`), $306B4, $428A2
 */
void Op4_DrawKillOutside(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5,
                         uint32_t *d6, uint32_t *d7, uint32_t *a0, uint32_t *a1, uint32_t *a2, uint32_t *a3,
                         uint32_t *a4, uint32_t a6)
{
    set_low_word(d0, (uint16_t)(*d4 - (uint16_t)(g_CameraX >> 16)));
    int16_t v = (int16_t)(uint16_t)*d0;
    int16_t lo = (int16_t)rd16(*a4);
    *a4 += 2;
    if (v < lo) {
        kill();
        return;
    }
    int16_t hi = (int16_t)rd16(*a4);
    *a4 += 2;
    if (v > hi) {
        kill();
        return;
    }
    set_low_word(d0, (uint16_t)(*d5 - (uint16_t)(g_CameraY >> 16)));
    v = (int16_t)(uint16_t)*d0;
    lo = (int16_t)rd16(*a4);
    *a4 += 2;
    if (v < lo) {
        kill();
        return;
    }
    hi = (int16_t)rd16(*a4);
    *a4 += 2;
    if (v <= hi) {
        Op4_Draw(d0, d1, d2, d3, d4, d5, d6, d7, a0, a1, a2, a3, a4, a6);
        return;
    }
    kill();
}

/* Op4_DrawKillOutsideOrClear - a4 op: Op4_DrawKillOutside, and the object is also deleted during
 * a screen clear.
 * In:  as Op4_DrawKillOutside; g_ScreenClearTimer
 * Out: Op4_Kill, or Op4_DrawKillOutside's
 * Evidence: tst.w (g_ScreenClearTimer).w / bne.w Op4_Kill, falling into Op4_DrawKillOutside; script
 *           uses $15072 (`679C 0000 0190 FF9C 01A8`), $150B0; 26 corpus runs
 */
void Op4_DrawKillOutsideOrClear(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4,
                                uint32_t *d5, uint32_t *d6, uint32_t *d7, uint32_t *a0, uint32_t *a1,
                                uint32_t *a2, uint32_t *a3, uint32_t *a4, uint32_t a6)
{
    if (g_ScreenClearTimer != 0) {
        kill();
        return;
    }
    Op4_DrawKillOutside(d0, d1, d2, d3, d4, d5, d6, d7, a0, a1, a2, a3, a4, a6);
}

/* Op4_DrawKillOffScreen - a4 op: kills the object when d4 / d5 is outside the on-screen window
 * (x - g_OnScreenLeft < $170, y - g_OnScreenTop < $110, unsigned); else draws (falling into
 * Op4_Draw).
 * In:  d4 / d5 = the drawn x / y words; a6 = the object
 * Out: d0's low word = the window offset; Op4_Kill or Op4_Draw's
 * Evidence: two sub.w / cmpi.w / bcc.w Op4_Kill tests, falling into Op4_Draw; an entry after the
 *           screen-clear test (computed-jump target in 151 corpus runs); 44 script uses, e.g. $13244,
 *           the player shots $505A0
 */
void Op4_DrawKillOffScreen(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5,
                           uint32_t *d6, uint32_t *d7, uint32_t *a0, uint32_t *a1, uint32_t *a2, uint32_t *a3,
                           uint32_t *a4, uint32_t a6)
{
    set_low_word(d0, (uint16_t)(*d4 - g_OnScreenLeft + WS_WIN_L()));   /* native: the window widened to the view (R4, R20) */
    if ((uint16_t)*d0 >= (uint16_t)(0x170 + (WS_WIN_L() + WS_WIN_R()))) {
        kill();
        return;
    }
    set_low_word(d0, (uint16_t)(*d5 - g_OnScreenTop));
    if ((uint16_t)*d0 >= 0x110) {
        kill();
        return;
    }
    Op4_Draw(d0, d1, d2, d3, d4, d5, d6, d7, a0, a1, a2, a3, a4, a6);
}

/* Op4_DrawKillOffScreenOrClear - a4 op: Op4_DrawKillOffScreen, and the object is also deleted
 * during a screen clear.
 * In:  as Op4_DrawKillOffScreen; g_ScreenClearTimer
 * Out: Op4_Kill, or Op4_DrawKillOffScreen's
 * Evidence: tst.w (g_ScreenClearTimer).w / bne.w Op4_Kill, falling into Op4_DrawKillOffScreen;
 *           script use $1515E; 63 corpus runs
 */
void Op4_DrawKillOffScreenOrClear(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4,
                                  uint32_t *d5, uint32_t *d6, uint32_t *d7, uint32_t *a0, uint32_t *a1,
                                  uint32_t *a2, uint32_t *a3, uint32_t *a4, uint32_t a6)
{
    if (g_ScreenClearTimer != 0) {
        kill();
        return;
    }
    Op4_DrawKillOffScreen(d0, d1, d2, d3, d4, d5, d6, d7, a0, a1, a2, a3, a4, a6);
}

/* the attribute edit: the high byte of the attributes (priority, palette, flips) ANDed with the
 * first operand byte, then XORed with the second; d0's low byte = the second */
static uint8_t edit_attr(uint8_t *d0, const u16 **script, Vm4Obj *obj)
{
    const u8 *bytes = (const u8 *)*script;
    *script += 1;
    uint8_t hi = (uint8_t)(obj->attr >> 8);
    *d0 = bytes[0];
    hi &= *d0;
    *d0 = bytes[1];
    hi ^= *d0;
    obj->attr = (uint16_t)(hi << 8 | (obj->attr & 0xFF));
    return hi;
}

/* Op4_SetAttr - a4 op: edits the object's sprite attribute byte: AND a mask, then XOR a value.
 * In:  script = byte and-mask, byte xor-value; self = a6
 * Out: d0's low byte = the xor value; self->attr's high byte; NEXT
 * Evidence: move.b (a4)+,d0 / and.b d0,$22(a6) / move.b (a4)+,d0 / eor.b d0,$22(a6) / NEXT; 123
 *           script uses, e.g. $14612 (`6800 9800`), $14634; 150 corpus runs
 */
void Op4_SetAttr(uint8_t *d0, const u16 **script, Vm4Obj *self)
{
    edit_attr(d0, script, self);
    dk::jump(dk::code((int16_t)(uint16_t)*(*script)++));
}

/* Op4_ChildSetAttr - a4 op: edits the last spawned child's attribute byte: AND mask, then XOR
 * value.
 * In:  script = byte and-mask, byte xor-value; child = a5
 * Out: d0's low byte = the xor value; child->attr's high byte; the eor.b's condition codes; NEXT
 * Evidence: move.b (a4)+,d0 / and.b d0,$22(a5) / move.b (a4)+,d0 / eor.b d0,$22(a5) / NEXT; script
 *           uses $26C5E (`6810 F008`), $49586 (`6810 7880`); 3 corpus runs
 */
void Op4_ChildSetAttr(uint8_t *d0, uint32_t *handler, const u16 **script, Vm4Obj *child, dk::Flags *ccr)
{
    *ccr = dk::Flags::test(edit_attr(d0, script, child), 1);
    *handler = dk::code((int16_t)(uint16_t)*(*script)++);
    dk::jump(*handler);
}

/* Op4_AnimFrame - anim-script op: shows the next animation frame: the wanted frame = the word,
 * the anim pc = the op after it; then draws (Op4_DrawFrame) and ends the frame until the ticker's
 * next carry.
 * In:  a4 = the anim script: word frame; a6 = the object; d4 / d5
 * Out: self->frame / anim; then Op4_DrawFrame's
 * Evidence: move.w (a4)+,$1E(a6) / move.l a4,$C(a6), falling into Op4_DrawFrame; an entry inside
 *           Op4_AnimFrameAttr (computed-jump target in 151 corpus runs); 639 uses in anim scripts
 */
void Op4_AnimFrame(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6,
                   uint32_t *d7, uint32_t *a0, uint32_t *a1, uint32_t *a2, uint32_t *a3, uint32_t *a4, uint32_t a6)
{
    PTR(Vm4Obj) self;
    self.v = a6;
    self->frame = rd16(*a4);
    *a4 += 2;
    self->anim.v = *a4;
    Op4_DrawFrame(d0, d1, d2, d3, d4, d5, d6, d7, a0, a1, a2, a3, a4, a6);
}

/* Op4_AnimFrameAttr - anim-script op: edits the attribute byte (AND mask, XOR value), then as
 * Op4_AnimFrame shows a frame, saves the anim pc and draws.
 * In:  a4 = the anim script: byte and-mask, byte xor-value, word frame; a6 = the object
 * Out: d0's low byte = the xor value; then Op4_AnimFrame's
 * Evidence: move.b (a4)+,d0 / and.b d0,$22(a6) / move.b (a4)+,d0 / eor.b d0,$22(a6), falling into
 *           Op4_AnimFrame; the first op of the anim scripts, e.g. $108E4 (`6820 1880 22E2 682C ...`);
 *           135 corpus runs
 */
void Op4_AnimFrameAttr(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5,
                       uint32_t *d6, uint32_t *d7, uint32_t *a0, uint32_t *a1, uint32_t *a2, uint32_t *a3,
                       uint32_t *a4, uint32_t a6)
{
    PTR(Vm4Obj) self;
    self.v = a6;
    PTR(const u16) script;
    script.v = *a4;
    const u16 *p = script;
    uint8_t b = (uint8_t)*d0;
    edit_attr(&b, &p, self.get());
    set_low_byte(d0, b);
    *a4 = dk::addr(p);
    Op4_AnimFrame(d0, d1, d2, d3, d4, d5, d6, d7, a0, a1, a2, a3, a4, a6);
}
