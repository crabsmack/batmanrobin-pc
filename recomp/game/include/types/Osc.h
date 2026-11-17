/* types/Osc.h - the oscillator block of the a2 script objects (docs/NOTES.md "a2 script
 * objects": Op2_AllocOsc, Op2_SetOscX/Y, Op2_StepOsc / Op2_AddOsc, Op2_StopOsc{,Y}): a pool block
 * whose handle sits in an a6 field the ops name; two axes swinging between -limit and +limit. */
#pragma once
#include "dk_mem.h"

typedef struct OscAxis {
    s32 pos;          /* +$00: position (16.16) */
    s32 limit;        /* +$04: the swing's turning point; its sign flips with the direction */
    s32 vel;          /* +$08: velocity (16.16 per run), the move the step ops hand on */
    s32 max_speed;    /* +$0C: the velocity's signed bound (0 = brake to rest: Op2_StopOsc) */
    s32 accel;        /* +$10: added to the velocity per run */
} OscAxis;
static_assert(sizeof(OscAxis) == 0x14, "OscAxis size");
static_assert(offsetof(OscAxis, max_speed) == 0x0C, "OscAxis.max_speed");

typedef struct Vm2Osc {
    OscAxis x;        /* +$00 */
    OscAxis y;        /* +$14 */
} Vm2Osc;
static_assert(offsetof(Vm2Osc, y) == 0x14, "Vm2Osc.y");
