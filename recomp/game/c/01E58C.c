#include "game.h"

/* the police car's a2 script (ROM data after the claw code; batmanrobin.asm): SetCruiseX $42A, anim
   $42, a coroutine that seeks screen-x ranges */
static constexpr uint32_t Scr2_01E5C2 = 0x01E5C2u;

static constexpr int16_t POLICE_START_DX = -0x50;   /* starts this far left of the screen */
static constexpr int16_t POLICE_START_DY = 0x6C;    /* ... this far below the screen top */

/* Stage1_SpawnPoliceCar - parade-truck trigger callback: spawns the police car that drives along
 * behind the truck: starts $50 px left of the screen at y camera + $6C, depth 1, script Scr2_01E5C2
 * (cruises to screen-x targets)
 * In:  d4/d5 = the trigger position (unused but passed to Obj_SpawnScripted)
 * Out: d0 = 0 and the car set up, or d0 = -1 (Z clear) when the pool is short; a4/a5/a6 = the new
 *      object's blocks as Obj_SpawnScripted left them
 * Evidence: jsr Obj_SpawnScripted / bne / move.l #$1E5C2,$2A(a4) / g_CameraX - $50 -> $3E(a6) /
 *           g_CameraY + $6C -> $42(a6) / move.w #1,$3A(a5) / moveq #0,d0; trigger entry Tbl_0166EA
 *           +$EE, TAS f13561; shot f13860: a black-and-white police cruiser. d0 an output (tst(d0.l),
 *           measured as a clobber): the ccout Z the trigger callers read is the moveq's, which the
 *           C gives as d0's test (Obj_NewMultiFail's moveq #-1 on the short path)
 */
uint32_t Stage1_SpawnPoliceCar(uint16_t *d1, uint16_t d4, uint16_t d5, uint32_t *a0, uint32_t *a4, uint32_t *a5,
                               uint32_t *a6)
{
    uint32_t d0 = 0, d7 = 0, a3 = 0;   /* Obj_SpawnScripted sets them (d7 = 4 blocks, a3 = the motion block) */
    Obj_SpawnScripted(&d0, d1, d4, d5, &d7, a0, &a3, a4, a5, a6);
    if (d0 != 0)                       /* bne: Z clear exactly when the pool was short (d0 = -1) */
        return d0;
    PTR(Vm2Block) vm;
    vm.v = *a4;
    PTR(Vm2Obj) self;
    self.v = *a6;
    PTR(Vm2Sprite) spr;
    spr.v = *a5;
    vm->resume.v = Scr2_01E5C2;
    /* native: the start placed for the wide view's edge (the gameplay mode, R20) */
    uint16_t dx = ws_place_x((uint16_t)POLICE_START_DX);
    WS_NOTE_PLACE(WS_PLACE_POLICE_CAR, POLICE_START_DX, (int16_t)(uint16_t)(dx - (uint16_t)POLICE_START_DX), *a6,
                  0x01E58Cu);
    self->x = (int16_t)((int16_t)(g_CameraX >> 16) + (int16_t)dx);
    self->y = (int16_t)((int16_t)(g_CameraY >> 16) + POLICE_START_DY);
    spr->depth = 1;
    return 0;
}
