/* types/BgTask.h - the background task's saved context (g_BgTaskContext, $FFE066): what
 * BgTask_Start and the V-int save and BgTask_Resume restores (docs/NOTES.md "Background task and
 * preemption"). Registers in movem order: d0-d2/d6-d7/a0-a4. */
#pragma once
#include "dk_mem.h"

typedef struct BgTaskContext {
    u32 d0;                /* +$00: its low word is the SR the resume's RTE restores */
    u32 d1, d2, d6, d7;
    u32 a0, a1, a2, a3;
    u32 a4;                /* +$24: the PC the resume's RTE continues at */
} BgTaskContext;
static_assert(sizeof(BgTaskContext) == 0x28, "BgTaskContext");
