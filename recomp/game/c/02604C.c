#include "game.h"

enum { BOSS2_HULL_Y_MIN = (int32_t)0xFE0C0000 };   /* -500.0 (16.16): the hull sinks no further up */

/* the shared tail of the SetTurretNCoroutine entries (loc_026062): the turret's coroutine slot 0
   = the script at the inline relative offset (from the offset word's own address) */
static void Boss2_SetTurretCoroutine(uint32_t *target, uint32_t *script, int16_t turret, dk::Flags *ccr)
{
    PTR(const s16) off;
    off.v = *script;
    *target = *script + (uint32_t)(int32_t)*off;   /* movea.l a2,a1 / adda.w (a2)+,a1 */
    *script += 2;
    const Vm2Obj *t = dk::handle<const Vm2Obj>(turret);
    dk::handle<Vm2Block>(t->vm)->coroutine[0].v = *target;   /* move.l a1,$38(a0) */
    *ccr = dk::Flags::test(*target, 4, ccr->x);
}

/* Boss_Stage2_SetTurret1Coroutine - boss native with an inline relative offset word: turret 1's
 * coroutine 0 becomes the script there, so the hull's attack script commands it (to fire)
 * In:  script = the offset word (a2); boss = the arena boss (a6, turret 1 = +$1E)
 * Out: a1 = the target script; the turret's a4 +$38 = it; script past the word; N/Z of the
 *      target (move.l), V/C clear
 * Evidence: movea.w $1E(a6),a0 / bra loc_026062: movea.l a2,a1 / adda.w (a2)+,a1 / movea.w
 *           $A(a0),a0 / move.l a1,$38(a0) / rts; dc.l after op $2CBC at $026FE4, $027034,
 *           $027084 ... (offsets to $0277C6); TAS f32290-f32770 (3 frames). The prototypes of
 *           the four entries gain &ccr for the measured ccout(NVC) (registers unchanged)
 */
void Boss_Stage2_SetTurret1Coroutine(uint32_t *target, uint32_t *script, const Stage2BossObj *boss, dk::Flags *ccr)
{
    Boss2_SetTurretCoroutine(target, script, boss->turret[0], ccr);
}

/* Boss_Stage2_SetTurret2Coroutine - Boss_Stage2_SetTurret1Coroutine for turret 2 (a6 +$20)
 * In:  script = the offset word (a2); boss = the arena boss (a6)
 * Out: a1 = the target script, turret 2's coroutine 0 = it; script past the word; N/Z, V/C clear
 * Evidence: movea.w $20(a6),a0 / bra loc_026062; dc.l after op $2CBC at $026FF8, $027048 ...
 */
void Boss_Stage2_SetTurret2Coroutine(uint32_t *target, uint32_t *script, const Stage2BossObj *boss, dk::Flags *ccr)
{
    Boss2_SetTurretCoroutine(target, script, boss->turret[1], ccr);
}

/* Boss_Stage2_SetTurret3Coroutine - Boss_Stage2_SetTurret1Coroutine for turret 3 (a6 +$22)
 * In:  script = the offset word (a2); boss = the arena boss (a6)
 * Out: a1 = the target script, turret 3's coroutine 0 = it; script past the word; N/Z, V/C clear
 * Evidence: movea.w $22(a6),a0 / bra loc_026062; dc.l after op $2CBC at $02700C, $02705C ...
 */
void Boss_Stage2_SetTurret3Coroutine(uint32_t *target, uint32_t *script, const Stage2BossObj *boss, dk::Flags *ccr)
{
    Boss2_SetTurretCoroutine(target, script, boss->turret[2], ccr);
}

/* Boss_Stage2_SetTurret4Coroutine - Boss_Stage2_SetTurret1Coroutine for turret 4 (a6 +$24),
 * falling into the shared tail
 * In:  script = the offset word (a2); boss = the arena boss (a6)
 * Out: a1 = the target script, turret 4's coroutine 0 = it; script past the word; N/Z, V/C clear
 * Evidence: movea.w $24(a6),a0, then loc_026062; dc.l after op $2CBC at $027020, $027070 ...
 */
void Boss_Stage2_SetTurret4Coroutine(uint32_t *target, uint32_t *script, const Stage2BossObj *boss, dk::Flags *ccr)
{
    Boss2_SetTurretCoroutine(target, script, boss->turret[3], ccr);
}

/* Boss_Stage2_MoveHullRunTurrets - boss native, every frame: the hull's y moves by -3/4 of the
 * floor's roll speed (no higher than -500), the hull picture follows the boss's x/y, then the four
 * unlinked turrets' scripts run inline
 * In:  boss = the arena boss (a6); g_Stage2Vars2->roll_speed; every register (the turret
 *      scripts run with them)
 * Out: boss->y moved; picture x/y integer words = the boss's; d0-d7 and a0-a5 as the last turret
 *      script leaves them (d0/d1 the outputs callers read); g_VM2SavedSP kept
 * Evidence: move.l g_SceneVars2+$12,d0 / move.l d0,d1 / asr.l #2,d0 / sub.l d1,d0 / add.l
 *           $42(a6),d0 / cmpi.l #$FE0C0000 / bgt / clamp / move.l d0,$42(a6) / move.w $3E(a6) and
 *           $42(a6) to g_SceneVars2+$16/$1A; g_VM2SavedSP, movem.l a1-a6 and the handles of
 *           turrets 4/3/2 pushed, VM2_RunObject x4 popping each; op $2CB2 at $025C9C (phase 1) and
 *           $025CEA (phase 2); TAS f32140-f35416. The prototype makes d2-d7 in/out (measured
 *           inputs: the contract ends at the turrets' scripts): with them inputs, verify's first
 *           call had d2 generated $263D0 (a turret script's) and C $900 (restored)
 */
void Boss_Stage2_MoveHullRunTurrets(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5,
                                    uint32_t *d6, uint32_t *d7, uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4,
                                    uint32_t a5, Stage2BossObj *boss)
{
    Stage2Vars2 *v2 = g_Stage2Vars2;
    int32_t roll = v2->roll_speed;
    int32_t y = (roll >> 2) - roll + boss->y;   /* asr.l #2 / sub.l / add.l */
    if (y <= BOSS2_HULL_Y_MIN)                  /* bgt keeps it: signed */
        y = BOSS2_HULL_Y_MIN;
    *d0 = (uint32_t)y;
    *d1 = (uint32_t)roll;
    boss->y = y;
    /* move.w: the integer words of the picture position (the fractions stay) */
    v2->picture_x = (int32_t)(((uint32_t)(uint16_t)boss->x << 16) | ((uint32_t)v2->picture_x & 0xFFFFu));
    v2->picture_y = (int32_t)(((uint32_t)y & 0xFFFF0000u) | ((uint32_t)v2->picture_y & 0xFFFFu));

    /* the stack image the asm builds: g_VM2SavedSP, movem.l a1-a6, the handles of turrets 4/3/2 */
    dk::push_long(g_VM2SavedSP);
    dk::push_long(dk::addr(boss));
    dk::push_long(a5);
    dk::push_long(a4);
    dk::push_long(a3);
    dk::push_long(a2);
    dk::push_long(a1);
    dk::push_word((uint16_t)boss->turret[3]);
    dk::push_word((uint16_t)boss->turret[2]);
    dk::push_word((uint16_t)boss->turret[1]);

    /* each turret's script runs with the registers the previous one left; VM2_RunObject loads
       a0 and a2-a5 from the object itself (the values going in are not read) */
    uint32_t r_a0 = 0, r_a1 = a1;
    const u16 *script = 0;
    void *motion = 0, *sprite = 0;
    Vm2Block *vm = 0;

    int16_t turret = boss->turret[0];   /* movea.w $1E(a6),a6 */
    for (int i = 0; i < 4; i++) {
        if (i > 0) {
            /* movea.w (a7)+,a6: pop the next handle (a long out, the word under it put back) */
            uint32_t two = dk::pop_long();
            dk::push_word((uint16_t)two);
            turret = (int16_t)(uint16_t)(two >> 16);
        }
        VM2_RunObject(d0, d1, d2, d3, d4, d5, d6, d7, &r_a0, &r_a1, &script, &motion, &vm, &sprite,
                      dk::handle<Vm2Obj>(turret));
    }

    for (int i = 0; i < 6; i++)
        (void)dk::pop_long();   /* movem.l (a7)+,a1-a6: the boss's registers, which C kept */
    g_VM2SavedSP = dk::pop_long();
}

/* Boss_Stage2_PlaceTurret - turret / ladder script native: places the object at its hull offset
 * (script variables 1/2) tilted with the arena floor
 * In:  vm = the object's a4 block (var 1/2 = offset x/y in the hull picture), self = the object
 * Out: self->x/y integer words = the tilted point; d0/d1 = the offset; d2-d5 as
 *      Boss_Stage2_TiltPoint leaves them
 * Evidence: move.w $24(a4),d0 / move.w $26(a4),d1 / bsr Boss_Stage2_TiltPoint / move.w d4,$3E(a6) /
 *           move.w d5,$42(a6); op $2CB2 at $026200 and $026230 (turret loops), bsr from
 *           Enemy_LadderThug_PlaceOnHull; RAM f32220: turret var1 $B7 + picture x -85 = x 98
 */
void Boss_Stage2_PlaceTurret(uint16_t *d0, uint16_t *d1, uint16_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5,
                             const Vm2Block *vm, Vm2Obj *self)
{
    *d0 = vm->var[1];
    *d1 = vm->var[2];
    Boss_Stage2_TiltPoint(*d0, *d1, d2, d3, d4, d5);
    self->x = (int16_t)(uint16_t)*d4;
    self->y = (int16_t)(uint16_t)*d5;
}
