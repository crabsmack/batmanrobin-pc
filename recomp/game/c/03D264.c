#include "game.h"

/* the boss's coroutine-0 script after the native's jsr $7FE.w (VM2_Enter): yield, loop */
static constexpr uint32_t Scr2_03D2A6 = 0x03D2A6u;

enum {
    HATTER_DROP_GRAVITY = 0x1000,     /* 1/16 px per frame per frame (16.16) */
    HATTER_DROP_FLOOR = 0x400000,     /* the offset bounces off 64 px */
};

/* two script variables as one 16.16 long (the asm's move.l $22(a4) / $26(a4)) */
static uint32_t var_long(const Vm2Block *vm, int i)
{
    return (uint32_t)(uint16_t)vm->var[i] << 16 | (uint16_t)vm->var[i + 1];
}
static void set_var_long(Vm2Block *vm, int i, uint32_t v)
{
    vm->var[i] = (uint16_t)(v >> 16);
    vm->var[i + 1] = (uint16_t)v;
}

/* Boss_Hatter_StepBounce - a2 native fragment run every frame after the Hatter's defeat
 * (coroutine 0): the hat's drop offset falls under gravity and bounces off 64 px with 3/4 of its
 * speed; the hat is drawn that far below its position
 * In:  vm = the boss's VM block (a4: vars 0/1 = speed, 2/3 = offset, 16.16); boss = the boss (a6)
 * Out: the speed and offset stored back; boss->draw_x = x, draw_y = y + the offset's integer part;
 *      then the script at Scr2_03D2A6
 * Evidence: move.l $22(a4),d0 / move.l $26(a4),d1 / addi.l #$1000 / add.l d0,d1 / cmpi.l #$400000
 *           / ble / asr.l #2 / sub.l / neg.l / swap d1 / add.w $42(a6) / move.w d1,$50(a6) /
 *           move.w $3E(a6),$4E(a6) / jsr ($7FE).w; Op2_StartCoroutine0Here at $03D260; TAS
 *           f115654-f116145 (492 frames)
 */
void Boss_Hatter_StepBounce(uint32_t d2, uint32_t d3, uint32_t d4, uint32_t d5, uint32_t d6, uint32_t d7,
                            uint32_t a1, uint32_t a3, Vm2Block *vm, uint32_t a5, Vm2Obj *boss)
{
    uint32_t speed = var_long(vm, 0) + HATTER_DROP_GRAVITY;
    uint32_t offset = var_long(vm, 2) + speed;
    if ((int32_t)offset > HATTER_DROP_FLOOR) {   /* the bounce: back up at 3/4 of the speed */
        speed = 0u - (speed - (uint32_t)((int32_t)speed >> 2));
        offset = HATTER_DROP_FLOOR;
    }
    set_var_long(vm, 0, speed);
    set_var_long(vm, 2, offset);
    boss->draw_y = (int16_t)(uint16_t)((offset >> 16) + (uint16_t)boss->y);
    boss->draw_x = boss->x;
    dk::jump_script(at_VM2_Enter, Scr2_03D2A6);
}
