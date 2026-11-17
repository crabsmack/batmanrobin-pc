/* types/VM5.h - the a5 wave director (docs/NOTES.md "Waves", "a5 wave scripts"): a one-block
 * object on the main list (Wave_Start), handler VM5_RunObject, which runs its a5 script while the
 * population is under the cap and its timer has run out; and the kill group block the script's
 * spawns join (Op5_StartGroup). Fields the C uses are named; the rest stay unk_XX. */
#pragma once
#include "dk_mem.h"
#include "dk_flow.h"

typedef struct Vm5Wave {
    u32 handler;                /* +$00: VM5_RunObject (the object list node) */
    s16 prev;                   /* +$04 */
    s16 next;                   /* +$06 */
    u16 unk_08;                 /* +$08: 0 (no extra blocks) */
    u16 serial;                 /* +$0A: g_WaveSerial at start; the wave ends when g_WaveGen passes it */
    u16 spawned;                /* +$0C: spawns so far (VM5_SpawnEntry) */
    u16 quota_1p;               /* +$0E: spawns before the after-quota script, one player */
    u16 quota_2p;               /* +$10: two players */
    u16 cap_1p;                 /* +$12: population cap the wave waits under, one player */
    u16 cap_2p;                 /* +$14: two players */
    PTR(const u16) after_quota; /* +$16: the script the quota switches to */
    u16 loop_a;                 /* +$1A: loop counters (Op5_SetLoopA* / Op5_LoopA, ...B) */
    u16 loop_b;                 /* +$1C */
    PTR(const u16) pc;          /* +$1E: where VM5_RunObject resumes the script */
    s16 start_x;                /* +$22: Wave_Start's d4 / d5 */
    s16 start_y;                /* +$24 */
    s16 origin_x;               /* +$26: added to every spawn position (Op5_SetOrigin*) */
    s16 origin_y;               /* +$28 */
    s32 interval;               /* +$2A: the timer's reload after each spawn op (Op5_SetInterval) */
    s32 timer;                  /* +$2E: counts down by g_WaveTimerStep; the script runs at <= 0 */
    s16 group;                  /* +$32: handle of the open kill group block, 0 = none */
    u16 rstack;                 /* +$34: return-stack pointer (handle; Op5_Gosub / Op5_Return) */
    u8  rstack_area[0x5A - 0x36];   /* +$36: the return stack */
} Vm5Wave;
static_assert(offsetof(Vm5Wave, serial) == 0x0A, "Vm5Wave.serial");
static_assert(offsetof(Vm5Wave, spawned) == 0x0C, "Vm5Wave.spawned");
static_assert(offsetof(Vm5Wave, quota_1p) == 0x0E, "Vm5Wave.quota_1p");
static_assert(offsetof(Vm5Wave, cap_1p) == 0x12, "Vm5Wave.cap_1p");
static_assert(offsetof(Vm5Wave, after_quota) == 0x16, "Vm5Wave.after_quota");
static_assert(offsetof(Vm5Wave, loop_a) == 0x1A, "Vm5Wave.loop_a");
static_assert(offsetof(Vm5Wave, pc) == 0x1E, "Vm5Wave.pc");
static_assert(offsetof(Vm5Wave, start_x) == 0x22, "Vm5Wave.start_x");
static_assert(offsetof(Vm5Wave, origin_x) == 0x26, "Vm5Wave.origin_x");
static_assert(offsetof(Vm5Wave, interval) == 0x2A, "Vm5Wave.interval");
static_assert(offsetof(Vm5Wave, timer) == 0x2E, "Vm5Wave.timer");
static_assert(offsetof(Vm5Wave, group) == 0x32, "Vm5Wave.group");
static_assert(offsetof(Vm5Wave, rstack) == 0x34, "Vm5Wave.rstack");
static_assert(sizeof(Vm5Wave) == 0x5A, "Vm5Wave size");

/* A kill group (Op5_StartGroup: one pool block): its members must all die within the time limit
 * for the reward spawn action to run at the last kill. */
typedef struct Vm5Group {
    u16 members;                /* +$00: the member target (closed at the spawned count by VM5_CloseGroup) */
    u16 kills_left;             /* +$02: counted down per counted kill; the reward fires at 0 */
    u16 reward;                 /* +$04: the reward spawn action (Tbl_012EF0) */
    u16 spawned;                /* +$06: members spawned */
    u16 dead;                   /* +$08: members gone (killed or removed); the block is freed at members */
    u32 deadline;               /* +$0A: g_FrameCounter limit for the reward, 0 = none */
} Vm5Group;
static_assert(offsetof(Vm5Group, spawned) == 0x06, "Vm5Group.spawned");
static_assert(offsetof(Vm5Group, deadline) == 0x0A, "Vm5Group.deadline");

/* the a5 VM's NEXT inline: the handler the next script word names (a0), jumped to */
static inline void vm5_next(uint32_t *a0, uint32_t *a5)
{
    PTR(const u16) script;
    script.v = *a5;
    *a0 = dk::code((int16_t)(uint16_t)*script++);
    *a5 = script.raw();
    dk::jump(*a0);
}

/* the next word of an a5 script */
static inline uint16_t vm5_word(uint32_t *a5)
{
    PTR(const u16) script;
    script.v = *a5;
    uint16_t w = *script;
    *a5 += 2;
    return w;
}
