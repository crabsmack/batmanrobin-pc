#include "game.h"

/* The road-end swirl's placement and the Mad Hatter's road schedule reader.
 *
 * The schedule is a list of 2-byte commands the boss keeps in its coroutine slot 2: byte 0's low
 * nibble picks the handler in Boss_Hatter_RoadCmdTbl (0 end, 1 wait, 2 goto, 3 gosub, 4 return,
 * 5 set loop, 6 loop, 7 random pick, 8-15 spawn kinds 0-7), its high nibble is a lane or the top
 * of a 12-bit offset, byte 1 an argument. Waits are counted in checker rows that pass the
 * players. */

enum {
    ROAD_ROW_TRAVEL = 0x7F,        /* the road's travel within one checker row ($80) */
    SWIRL_DX = 0xC4,               /* the swirl's road x */
};

static uint32_t swap(uint32_t v) { return (v << 16) | (v >> 16); }
static void set_low_word(uint32_t *r, uint16_t v) { *r = (*r & 0xFFFF0000u) | v; }

/* Stage3_PlaceRoadEndSwirl - puts the road-end swirl at the far end of the road: x = ($C4 + the
 * last row's x offset) x its scale / 4096 + the road objects' screen origin, y = the last row's
 * line + the origin's y + 1
 * In:  swirl = the swirl (a6); the farthest row Stage3_DrawRoad left at g_SceneVars+$A2
 * Out: swirl->x / y; d4/d5 = the same (d4's upper word = the product's low bits after the swap);
 *      a1 = the row's scale word
 * Evidence: lea g_SceneVars+$A2,a1 / move.w (a1)+,d5 / addq.l #2,a1 / move.w #$C4,d4 / add.w (a1)+ /
 *           muls.w (a1),d4 / asl.l #4 / swap / add.w g_SceneVars2+$3A/$3C / addq.w #1,d5;
 *           Op2_CallNativeRel at $03DBD2 in Scr2_03DBC6's frame loop; TAS f113312-f116145
 */
void Stage3_PlaceRoadEndSwirl(uint32_t *d4, uint16_t *d5, uint32_t *a1, Vm2Obj *swirl)
{
    const HatterRoadVars *road = hatter_road();
    const RoadRow *far = &hatter_road_scene()->rows[18];
    uint32_t x = swap((uint32_t)((int32_t)(int16_t)(SWIRL_DX + far->x_offset) * (int32_t)far->scale) << 4);
    set_low_word(&x, (uint16_t)(x + road->origin_x));
    *d4 = x;
    *d5 = (uint16_t)(far->line + road->origin_y + 1);
    *a1 = dk::addr(&far->scale);
    swirl->x = (int16_t)(uint16_t)x;
    swirl->y = (int16_t)*d5;
}

/* Boss_Hatter_StepRoadSchedule - per-frame step of the Hatter's road schedule: counts the road's
 * travel within a checker row; each time a row passes it counts down the schedule's row wait and,
 * once that runs out, runs commands until one waits
 * In:  a4 = the boss's VM block (coroutine slot 2 = the schedule pointer, 0 = none)
 * Out: hatter_road_scene()->travel; as Boss_Hatter_RoadSchedRowPassed when a row passed, else d0's
 *      low word = the travel
 * Evidence: move.w g_SceneVars+2,d0 / sub.w g_SceneVars2+$C,d0 / bcs / andi.w #$7F / move.w;
 *           Op2_CallNativeRelSaved $03DC94 at $03D0C4/$03D12C in the boss's frame loops; TAS
 *           f113312-f116145
 */
void Boss_Hatter_StepRoadSchedule(uint32_t *d0, uint8_t *d1, uint32_t *d4, uint16_t *d5, uint32_t *a0, uint32_t *a1,
                                  uint32_t a4, uint32_t a6)
{
    HatterRoadScene *scene = hatter_road_scene();
    uint16_t travel = scene->travel;
    uint16_t step = (uint16_t)hatter_road()->step;
    set_low_word(d0, (uint16_t)(travel - step));
    if (travel < step) {                     /* bcs: a checker row passed */
        /* a command handler Boss_Hatter_RoadCmdNext jumps to runs with the registers this
           function's slot leaves: a0 (the schedule pointer past the command) and a1 must be
           outputs, or it reads the entry's (verify, TAS: $40(a4) = $03DC94, the native's address) */
        Boss_Hatter_RoadSchedRowPassed(d0, d1, d4, d5, a0, a1, a4, a6);
        return;
    }
    set_low_word(d0, (uint16_t)(*d0 & ROAD_ROW_TRAVEL));
    scene->travel = (uint16_t)*d0;
}

/* Boss_Hatter_RoadSchedIdle - the schedule step's return: no row passed, no schedule, or the wait
 * still running
 * In:  -
 * Out: -
 * Evidence: rts at $03DCA6; branched to from Boss_Hatter_RoadSchedRowPassed (beq) and
 *           Boss_Hatter_RoadSchedCountWait (bpl)
 */
void Boss_Hatter_RoadSchedIdle(void)
{
}

/* Boss_Hatter_RoadSchedRowPassed - a checker row has passed: wraps the travel to the row, then,
 * with a schedule in the boss's slot 2, counts down its row wait
 * In:  d0's low word = travel - step (borrowed); a4 = the boss's VM block
 * Out: travel; d0 = the schedule pointer (0: none, done); a0 = it; then
 *      Boss_Hatter_RoadSchedCountWait's
 * Evidence: andi.w #$7F,d0 / move.w d0,g_SceneVars+2 / move.l $40(a4),d0 / beq / movea.l d0,a0;
 *           the bcs target in Boss_Hatter_StepRoadSchedule; TAS f114753-f114833
 */
void Boss_Hatter_RoadSchedRowPassed(uint32_t *d0, uint8_t *d1, uint32_t *d4, uint16_t *d5, uint32_t *a0, uint32_t *a1,
                                    uint32_t a4, uint32_t a6)
{
    set_low_word(d0, (uint16_t)(*d0 & ROAD_ROW_TRAVEL));
    hatter_road_scene()->travel = (uint16_t)*d0;
    PTR(const Vm2Block) boss_vm;
    boss_vm.v = a4;
    *d0 = boss_vm->coroutine[2].v;
    if (*d0 == 0)
        return;
    *a0 = *d0;
    dk::Flags ccr{};
    Boss_Hatter_RoadSchedCountWait(d0, d1, d4, d5, a0, a1, a4, a6, &ccr);
}

/* Boss_Hatter_RoadSchedCountWait - counts down the schedule's row wait and, once it runs out
 * (below 0), reads the next command
 * In:  a0 = the schedule pointer; a4 = the boss's VM block
 * Out: hatter_road_scene()->wait; waiting: the condition codes of its subq.b; else
 *      Boss_Hatter_RoadCmdNext's
 * Evidence: subq.b #1,(g_SceneVars+$10).w / bpl rts; the fall from Boss_Hatter_RoadSchedRowPassed and
 *           the bra.w from Boss_Hatter_RoadCmdSpawnInLane; TAS f114753-f114833
 */
void Boss_Hatter_RoadSchedCountWait(uint32_t *d0, uint8_t *d1, uint32_t *d4, uint16_t *d5, uint32_t *a0, uint32_t *a1,
                                    uint32_t a4, uint32_t a6, dk::Flags *ccr)
{
    HatterRoadScene *scene = hatter_road_scene();
    uint8_t wait = scene->wait;
    scene->wait = (uint8_t)(wait - 1);
    *ccr = dk::Flags::compare(wait, 1, 1, wait == 0);
    if ((int8_t)scene->wait >= 0)
        return;
    Boss_Hatter_RoadCmdNext(d0, d1, d4, d5, a0, a1, a4, a6, ccr);   /* its condition codes come back in ccr */
}

/* Boss_Hatter_RoadCmdNext - reads the schedule's next 2-byte command and jumps to its handler in
 * Boss_Hatter_RoadCmdTbl by the low nibble of byte 0
 * In:  a0 = the command; a4 = the boss's VM block
 * Out: a0 past the command; d4's low byte = byte 0, d1 = byte 1, d0's low word = the nibble x 2
 *      (its upper word kept); then the handler's
 * Evidence: move.b (a0)+,d0 / move.b (a0)+,d1 / move.b d0,d4 / andi.w #$F,d0 / add.w d0,d0 /
 *           jmp Boss_Hatter_RoadCmdTbl(pc,d0.w); bra.s from Boss_Hatter_RoadCmdGoto; the raw
 *           commands 4-7 branch here too
 */
void Boss_Hatter_RoadCmdNext(uint32_t *d0, uint8_t *d1, uint32_t *d4, uint16_t *d5, uint32_t *a0, uint32_t *a1,
                             uint32_t a4, uint32_t a6, dk::Flags *ccr)
{
    PTR(const u8) command;
    command.v = *a0;
    uint8_t op = command[0];
    *d1 = command[1];
    *a0 += 2;
    *d4 = (*d4 & 0xFFFFFF00u) | op;
    set_low_word(d0, (uint16_t)((op & 0xF) * 2));
    dk::jump(at_Boss_Hatter_RoadCmdTbl + (uint16_t)*d0);
}

/* Boss_Hatter_RoadCmdEnd - road schedule command 0: ends the schedule (clears the gosub depth, the
 * row wait and the boss's slot 2)
 * In:  boss_vm = the boss's VM block (a4)
 * Out: depth, wait (the whole word) and slot 2 = 0; the condition codes of the clr.l (Z)
 * Evidence: clr.w g_SceneVars+4 / clr.w g_SceneVars+$10 / clr.l $40(a4) / rts; the bra.s of
 *           Boss_Hatter_RoadCmdTbl entry 0; 3 corpus runs
 */
void Boss_Hatter_RoadCmdEnd(Vm2Block *boss_vm, dk::Flags *ccr)
{
    HatterRoadScene *scene = hatter_road_scene();
    scene->depth = 0;
    scene->wait = 0;
    scene->unk_11 = 0;
    boss_vm->coroutine[2].v = 0;
    *ccr = dk::Flags::test(0, 4, ccr->x);
}
