#include "game.h"

/* $006952-$006A59: the player-shot ops of the a4 VM (a frame ring drawn straight onto the front
 * sprite list, a velocity from the heading, the step with its enemy-box test on alternate frames)
 * and the a5 VM's core: VM5_RunObject, the wave director's handler, and VM5_Next. */

/* Tbl_059B22: the sine table, a signed word per even heading (heading & $3FE as a byte offset);
   the cosine a quarter turn on (+$100 bytes) */
static constexpr uint32_t Tbl_059B22 = 0x059B22u;

static void next4(const u16 **script)
{
    dk::jump(dk::code((int16_t)(uint16_t)*(*script)++));
}

/* a node of a shot's frame ring: the next node, and the sprite data drawn */
typedef struct ShotFrameNode {
    PTR(const ShotFrameNode) next;
    u32 sprite;
} ShotFrameNode;

/* Op4_SetShotFrames - a4 op (shots): sets the object's frame ring (a cyclic list of {next node,
 * sprite data} longs, kept in the anim-script field) and its resume pc to the next op.
 * In:  script = long frame-ring address; self = a6
 * Out: self->anim = the ring; self->resume; NEXT
 * Evidence: move.l (a4)+,$C(a6) / move.l a4,$8(a6) / NEXT; player shot scripts $50524 (`6952 0005
 *           0612 6990`); Op4_ShotStep walks the ring; 148 corpus runs
 */
void Op4_SetShotFrames(const u16 **script, Vm4Obj *self)
{
    self->anim.v = (uint32_t)(uint16_t)(*script)[0] << 16 | (uint16_t)(*script)[1];
    *script += 2;
    self->resume = *script;
    next4(script);
}

/* Op4_SetVelFromHeading - a4 op: sets the object's x / y velocity to speed * (cos, sin)(heading)
 * (8.8 speed, the table's 1.0 = $100 after the asr 8) and steps x / y back by it once, so the
 * first move lands on the start point.
 * In:  script = word speed (8.8 px/frame); self = a6
 * Out: d1 = the y velocity; self->vel_x / vel_y; self->x / y; NEXT
 * Evidence: lea Tbl_059B22,a0 / andi.w #$3FE / move.w (a4)+,d0 / muls.w $100(a0),d0 / muls.w (a0),d1 /
 *           asr.l #8 / move.l to $3E / $42 / sub.l from $2E / $32; shot scripts $50520 (`695E 0E00`:
 *           14 px/frame); 148 corpus runs
 */
void Op4_SetVelFromHeading(uint32_t *d1, const u16 **script, Vm4Obj *self)
{
    PTR(const s16) e;
    e.v = Tbl_059B22 + (uint16_t)(self->heading & 0x3FE);
    int16_t speed = (int16_t)(uint16_t)*(*script)++;
    int32_t vx = ((int32_t)speed * e.get()[0x80]) >> 8;
    int32_t vy = ((int32_t)speed * e.get()[0]) >> 8;
    *d1 = (uint32_t)vy;
    self->vel_x = vx;
    self->vel_y = vy;
    vm4_add_fix(self->x, self->x_frac, 0u - (uint32_t)vx);
    vm4_add_fix(self->y, self->y_frac, 0u - (uint32_t)vy);
    next4(script);
}

/* Op4_ShotStep - a4 op (shots): moves x / y by the velocity, then as Op4_ShotStepAt at the new
 * position.
 * In:  script = word half-width, word half-height, word hit branch offset; self = a6
 * Out: d0 = the y velocity (its low byte then g_FrameParity); d4 / d5 = x / y words; then
 *      Op4_ShotStepAt's
 * Evidence: move.l $3E(a6),d0 / add.l d0,$2E(a6) / move.l $42(a6),d0 / add.l d0,$32(a6) /
 *           move.w $2E(a6),d4 / move.w $32(a6),d5, falling into Op4_ShotStepAt; player shot scripts
 *           $5052A (`6990 0006 0006 009E`), $5055E; 148 corpus runs
 */
void Op4_ShotStep(uint32_t *d0, uint16_t *d2, uint16_t *d3, uint16_t *d4, uint16_t *d5, uint16_t *d6, uint16_t *d7,
                  uint32_t *a0, uint32_t *a1, uint32_t *a2, const u16 **script, Vm4Obj *self, dk::Flags *ccr)
{
    *d0 = (uint32_t)(int32_t)self->vel_x;
    vm4_add_fix(self->x, self->x_frac, *d0);
    *d0 = (uint32_t)(int32_t)self->vel_y;
    vm4_add_fix(self->y, self->y_frac, *d0);
    *d4 = (uint16_t)self->x;
    *d5 = (uint16_t)self->y;
    Op4_ShotStepAt(d0, d2, d3, d4, d5, d6, d7, a0, a1, a2, script, self, ccr);
}

/* Op4_ShotStepAt - a4 op (shots): at d4 / d5: on the object's frame phase branches when an enemy
 * box overlaps +-w / +-h; on the other frames deletes the object when it is off camera ($140 x $E0,
 * Obj_UnlinkFree); then puts the next frame of its ring straight onto the front sprite list (no
 * art cache: the entry is skipped when the list is full) and ends the frame.
 * In:  script = word half-width, word half-height, word hit branch offset; d4 / d5 = x / y words;
 *      self = a6 (hit_phase, the ring, attr)
 * Out: hit: the branch (a2 = the box); else the ring advanced, the sprite entry; d2 / d3 = the
 *      point, d4 = attr, d0 = the entry's address (low word); the condition codes of the last
 *      move (attr's tst, or the clr.b of a full list); rts (the end of the run)
 * Evidence: g_FrameParity vs $18(a6) / Hitbox_FindEnemyBox / beq / adda.w (a4),a4; sub.w g_CameraX /
 *           cmpi.w #$140 / bcc.w Obj_UnlinkFree; movea.l $C(a6),a0 / move.l (a0)+,$C(a6) /
 *           subq.b #1,(g_SprNodesFree).w / addi.w #$12,(g_SprFrontEnd).w / move.l a0,-(a1) /
 *           movem.w d0/d2-d4,-(a1); an entry inside Op4_ShotStep (computed-jump target in 117 runs)
 */
void Op4_ShotStepAt(uint32_t *d0, uint16_t *d2, uint16_t *d3, uint16_t *d4, uint16_t *d5, uint16_t *d6,
                    uint16_t *d7, uint32_t *a0, uint32_t *a1, uint32_t *a2, const u16 **script, Vm4Obj *self,
                    dk::Flags *ccr)
{
    *d2 = *d4;
    *d3 = *d5;
    *d0 = (*d0 & 0xFFFFFF00u) | g_FrameParity;
    if ((uint8_t)*d0 == self->hit_phase) {
        *d6 = *(*script)++;
        *d7 = *(*script)++;
        *d4 = (uint16_t)(*d4 - *d6);
        *d6 = (uint16_t)(*d6 + *d2);
        *d5 = (uint16_t)(*d5 - *d7);
        *d7 = (uint16_t)(*d7 + *d3);
        Hitbox_FindEnemyBox(d0, *d4, *d5, *d6, *d7, a2);
        PTR(const HitBox) box;
        box.v = *a2;
        if (box->next != 0) {   /* found (the tail sentinel's next link is 0) */
            *ccr = dk::Flags{ false, false, false, false, false };   /* Hitbox_FindEnemyBox's Z clear */
            *script = (const u16 *)((const u8 *)*script + (int16_t)(uint16_t)**script);
            *a0 = dk::code((int16_t)(uint16_t)*(*script)++);
            dk::jump(*a0);
            return;
        }
    } else {
        *d4 = (uint16_t)(*d4 - (uint16_t)(g_CameraX >> 16) + WS_WIN_L());   /* native: the window widened to the view (R4, R20) */
        if (*d4 >= (uint16_t)(0x140 + (WS_WIN_L() + WS_WIN_R()))) {
            dk::jump(at_Obj_UnlinkFree);
            return;
        }
        *d5 = (uint16_t)(*d5 - (uint16_t)(g_CameraY >> 16));
        if (*d5 >= 0xE0) {
            dk::jump(at_Obj_UnlinkFree);
            return;
        }
    }
    PTR(const ShotFrameNode) node;
    node.v = self->anim.raw();
    self->anim.v = node->next.raw();
    *a0 = node->sprite;
    g_SprNodesFree = (uint8_t)(g_SprNodesFree - 1);
    if ((int8_t)g_SprNodesFree < 0) {
        g_SprNodesFree = 0;
        *ccr = dk::Flags{ false, false, true, false, false };   /* clr.b (X is never read) */
        return;
    }
    *a1 = (uint32_t)(int32_t)(int16_t)g_SprFrontEnd;
    g_SprFrontEnd = (uint16_t)(g_SprFrontEnd + 0x12);
    *d0 = (*d0 & 0xFFFF0000u) | (uint16_t)*a1;
    *d4 = self->attr;
    /* move.l a0,-(a1) / movem.w d0/d2-d4,-(a1): the entry's last 12 bytes, written downward */
    *a1 -= 4;
    PTR(u32) data;
    data.v = *a1;
    *data = *a0;
    *a1 -= 8;
    PTR(u16) words;
    words.v = *a1;
    words.get()[0] = (uint16_t)*d0;
    words.get()[1] = *d2;
    words.get()[2] = *d3;
    words.get()[3] = *d4;
    *ccr = dk::Flags::test(*d4, 2);
}

/* VM5_RunObject - the wave director's object handler (the a5 VM): ends the wave when a newer wave
 * generation started; while g_ObjPopulation is below its cap (the two-player cap when both
 * players are in play) counts its timer down by g_WaveTimerStep and, when it runs out, saves the
 * stack pointer (the ops yield by restoring it) and resumes the script.
 * In:  a6 = the wave object
 * Out: rts (waiting: d0 = the cap or the timer left), VM5_EndWave, or the script (a5) via VM5_Next
 * Evidence: move.w (g_WaveGen).w,d0 / cmp.w $A(a6),d0 / bgt.w VM5_EndWave / btst #0 of g_P1Flags and
 *           g_P2Flags / cmp.w (g_ObjPopulation).w,d0 / ble rts / sub.l (g_WaveTimerStep).w,d0 / ble /
 *           move.l a7,(g_VM5SavedSP).w / movea.l $1E(a6),a5; installed by Wave_Start; 117 corpus runs
 */
void VM5_RunObject(uint32_t *d0, uint32_t *a0, uint32_t *a1, uint32_t *a5, Vm5Wave *self)
{
    (void)a1;
    *d0 = (*d0 & 0xFFFF0000u) | g_WaveGen;
    if ((int16_t)g_WaveGen > (int16_t)self->serial) {
        dk::jump(at_VM5_EndWave);
        return;
    }
    uint16_t cap = self->cap_1p;
    if ((g_P1Flags & 1) && (g_P2Flags & 1))
        cap = self->cap_2p;
    *d0 = (*d0 & 0xFFFF0000u) | cap;
    if ((int16_t)cap <= (int16_t)g_ObjPopulation)
        return;
    int32_t left = (int32_t)((uint32_t)(int32_t)self->timer - (uint32_t)g_WaveTimerStep);
    *d0 = (uint32_t)left;
    /* ble after sub.l: Z, or N != V (the exact difference <= 0) */
    if ((int64_t)(int32_t)self->timer - (int64_t)(int32_t)g_WaveTimerStep > 0) {
        self->timer = left;
        return;
    }
    g_VM5SavedSP = dk::stack_pointer();
    *a5 = self->pc.raw();
    VM5_Next(a0, a5);
}

/* VM5_Next - the a5 VM's NEXT: fetches the next op word from the script and jumps to that handler
 * (also the not-waiting path of the Op5_WaitPopulation ops).
 * In:  a5 = the script
 * Out: a0 = the handler; a5 past the word
 * Evidence: movea.w (a5)+,a0 / jmp (a0); the bge targets in Op5_WaitPopulation and
 *           Op5_WaitPopulation0-6
 */
void VM5_Next(uint32_t *a0, uint32_t *a5)
{
    PTR(const u16) script;
    script.v = *a5;
    *a0 = dk::code((int16_t)(uint16_t)*script);
    *a5 += 2;
    dk::jump(*a0);
}
