#include "game.h"

/* the toy figure's rotation frames: plain, and the outlined ("charged") copy (ROM data) */
static constexpr uint32_t Tbl_036DA8 = 0x036DA8u;
static constexpr uint32_t Tbl_ToyFigureChargedFrames = 0x036E98u;
/* the road steerer's a2 script (ROM data after this file's code) */
static constexpr uint32_t Scr2_03DA8C = 0x03DA8Cu;

enum {
    TOY_KILL_Y = 0xF0,             /* a thrown toy is deleted once drawn this low */
    TOY_ATTR_KEEP = 0xE0,          /* the attribute bits the frame table keeps (priority, palette) */
    TOY_OUTLINE_PHASE = 1 << 1,    /* g_FrameCounter bit: outlined frames on alternate 2-frame phases */
    STEER_IDLE_OP = 0x2D28,        /* an idle coroutine slot (the a2 VM's stop-coroutine resume) */
};

/* Boss_Hatter_ThrownToyFrame - a4 native in the toy figures the Hatter throws: deletes the toy
 * once below y $F0 (dropping the native's return, then Op4_Kill), else sets its frame and
 * attributes from the rotation table at the road's spin step: plain, or outlined on alternate
 * 2-frame phases
 * In:  y = its drawn y (d5); toy = the toy (a6)
 * Out: kill: Op4_Kill's. Else toy->frame / attr (the step's mask AND $E0, XOR its flip byte),
 *      d0's low byte = the attribute byte, d1 = the flip byte, a0 = past the step; the
 *      condition codes of the move.b to the attributes
 * Evidence: cmpi.w #$F0,d5 / blt / addq.l #4,a7 / jmp Op4_Kill; lea Tbl_036DA8 / btst #1,
 *           g_FrameCounter+3 / Tbl_ToyFigureChargedFrames / adda.w g_SceneVars2+$3E / move.w
 *           (a0)+,$1E(a6) / move.b #$E0,d0 / and.b (a0)+ / move.b (a0)+,d1 / eor.b / move.b
 *           d0,$22(a6); Op4_CallNative at $03DA1C in Scr4_03D9BC; TAS f113792-f114052
 */
void Boss_Hatter_ThrownToyFrame(uint16_t *d0, uint8_t *d1, uint16_t *d2, uint16_t y, uint32_t *a0, uint32_t *a1,
                                ThrownToy *toy, dk::Flags *ccr)
{
    if ((int16_t)y >= TOY_KILL_Y) {
        dk::drop_return();
        dk::jump(at_Op4_Kill);
        return;
    }
    uint32_t table = (g_FrameCounter & TOY_OUTLINE_PHASE) ? Tbl_ToyFigureChargedFrames : Tbl_036DA8;
    PTR(const ToyFrameStep) step;
    step.v = table + (uint32_t)(int32_t)(int16_t)hatter_road()->spin;
    toy->frame = step->frame;
    uint8_t attr = (uint8_t)((TOY_ATTR_KEEP & step->attr_mask) ^ step->flip);
    *d1 = step->flip;
    *d0 = (uint16_t)((*d0 & 0xFF00u) | attr);
    toy->attr = attr;
    *a0 = step.v + sizeof(ToyFrameStep);
    *ccr = dk::Flags::test(attr, 1, ccr->x);
}

/* Boss_Hatter_SpawnRoadSteer - spawns the Hatter's road-steering child Scr2_03DA8C: an invisible
 * mover whose world x/y become the road's bend and pitch; stops its coroutine slots 0/1 and clears
 * its script variables 0-2 (the road speed target and rate)
 * In:  d4/d5 = position; a4 = the boss's VM block (Op2_SpawnChildOrWait)
 * Out: a6 = the child, d7 = Obj_SpawnBossOrQuit's; Z set (moveq #0 and the clears); a full pool
 *      quits the caller (Obj_SpawnBossOrQuit returns past this)
 * Evidence: lea Scr2_03DA8C,a2 / jsr Obj_SpawnBossOrQuit / move.l #$2D28,$38(a4) / $3C(a4) /
 *           moveq #0,d0 / move.l d0,$22(a4) / move.l d0,$24(a4); Op2_SpawnChildOrWait $12 at
 *           $03D032; TAS f113311
 */
void Boss_Hatter_SpawnRoadSteer(uint16_t d4, uint16_t d5, uint32_t *d7, uint32_t a4, uint32_t *a6, dk::Flags *ccr)
{
    uint32_t d0 = 0, list = 0, motion = 0, sprite = 0;
    uint16_t d1 = 0;
    Obj_SpawnBossOrQuit(&d0, &d1, d4, d5, d7, &list, Scr2_03DA8C, &motion, &a4, &sprite, a6);
    PTR(Vm2Block) vm;
    vm.v = a4;                                     /* the child's VM block */
    vm->coroutine[0].v = STEER_IDLE_OP;
    vm->coroutine[1].v = STEER_IDLE_OP;
    vm->var[0] = 0;
    vm->var[1] = 0;
    vm->var[2] = 0;
    *ccr = dk::Flags::test(0, 4, ccr->x);
}
