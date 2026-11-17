#include "game.h"

/* the claw's a2 script and its palette (ROM data; batmanrobin.asm) */
static constexpr uint32_t Scr2_01E778 = 0x01E778u;
static constexpr uint32_t Tbl_01E748 = 0x01E748u;

static constexpr uint16_t CLAW_BLOCKS = 8;            /* body, chain and the three fingers' blocks */
static constexpr uint16_t CLAW_FINGER_ANGLE0 = 0x2F8;  /* the fingers' frame starts turned this far */

/* Boss_Claw_Spawn - warehouse trigger callback: spawns Harley's giant crane claw (8 pool blocks:
 * body, chain and three finger blocks) running Scr2_01E778, with its palette in line 3
 * In:  d4/d5 = x/y (trigger entry Tbl_016632 +$A2)
 * Out: d0 = 0 (Z set), or Obj_Spawn's -1 (Z clear) when the pool is short; a0/a1/a4/a5/a6 as
 *      Pal_LoadLine3 and Obj_Spawn left them
 * Evidence: lea VM2_RunObject,a0 / moveq #8,d7 / jsr Obj_Spawn / bne / move.l #$1E778,$2A(a4) /
 *           movea.w $E(a6),a0 / jsr ObjPool_Clear / lea Tbl_01E748,a0 / jsr Pal_LoadLine3 /
 *           move.w #$2F8,g_Rot3DInner / clr.b g_SceneVars / moveq #0,d0; TAS f10511; shots
 *           f11460/f12420. d0 an output (tst(d0.l), measured as a clobber): the ccout Z the
 *           trigger callers read is the final moveq's, which the C gives as d0's test
 */
uint32_t Boss_Claw_Spawn(uint16_t *d1, uint16_t d4, uint16_t d5, uint32_t *a0, uint32_t *a1, uint32_t *a4,
                         uint32_t *a5, uint32_t *a6)
{
    uint32_t d0 = 0, a3 = 0;
    uint16_t d7 = CLAW_BLOCKS;
    *a0 = at_VM2_RunObject;             /* the object's handler */
    Obj_Spawn(&d0, d1, d4, d5, &d7, a0, &a3, a4, a5, a6);
    if (d0 != 0)                        /* bne: Z clear exactly when the pool was short (d0 = -1) */
        return d0;
    PTR(Vm2Block) vm;
    vm.v = *a4;
    vm->resume.v = Scr2_01E778;
    PTR(ClawObj) claw;
    claw.v = *a6;
    *a0 = (uint32_t)(int32_t)(int16_t)claw->chain;   /* movea.w: the chain block, sign-extended */
    ObjPool_Clear(&d0, a0);
    *a0 = Tbl_01E748;
    dk::Flags pal{};                    /* its condition codes: the moveq #0,d0 below replaces them */
    Pal_LoadLine3(a0, a1, &pal);
    ((Rot3D *)g_Rot3DInner)->angle_a = CLAW_FINGER_ANGLE0;
    g_SceneVars[0] = 0;                 /* Harley's final form not beaten yet (Boss_Claw_BranchHarleyBeaten) */
    return 0;
}
