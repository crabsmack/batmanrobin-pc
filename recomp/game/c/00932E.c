#include "game.h"

/* A wave director: a one-block a5-VM object (VM5_RunObject; docs/NOTES.md "a5 wave scripts"). The
 * fields Wave_Start sets. */
struct WaveObj_932E {
    u32 handler;        /* +$00: VM5_RunObject (Obj_New) */
    s16 prev, next;     /* +$04/$06: the object list links (Obj_New) */
    u16 unk_08;         /* +$08: cleared */
    u16 serial;         /* +$0A: the wave serial (VM5_RunObject ends the wave when g_WaveGen is above it) */
    u16 spawn_count;    /* +$0C: enemies spawned so far */
    u16 quota[2];       /* +$0E/$10: the spawn quota with one / two players (Op5_SetQuota) */
    u16 cap[2];         /* +$12/$14: the population cap with one / two players (Op5_SetPopulationCap) */
    u32 after_quota;    /* +$16: where the script goes once the quota is reached */
    u16 loop[2];        /* +$1A/$1C: loop counters A and B */
    u32 script;         /* +$1E: the a5 script (resumed here) */
    u16 x, y;           /* +$22/$24: the start position */
    u16 origin_x;       /* +$26: the spawn origin (Op5_SetOrigin*) */
    u16 origin_y;       /* +$28 */
    u32 interval;       /* +$2A: the spawn interval, reloaded into the timer after every spawn op */
    u32 timer;          /* +$2E: g_WaveTimerStep comes off it each frame; at <= 0 the script runs */
    u16 kill_group;     /* +$32 */
    u16 stack_ptr;      /* +$34: the return stack's pointer (Op5_Gosub / Op5_Return) */
    u8  stack[0x24];    /* +$36: the return stack */
};
static_assert(offsetof(WaveObj_932E, serial) == 0x0A, "WaveObj_932E.serial");
static_assert(offsetof(WaveObj_932E, after_quota) == 0x16, "WaveObj_932E.after_quota");
static_assert(offsetof(WaveObj_932E, script) == 0x1E, "WaveObj_932E.script");
static_assert(offsetof(WaveObj_932E, interval) == 0x2A, "WaveObj_932E.interval");
static_assert(offsetof(WaveObj_932E, stack_ptr) == 0x34, "WaveObj_932E.stack_ptr");
static_assert(offsetof(WaveObj_932E, stack) == 0x36, "WaveObj_932E.stack");

/* Scr5_00939E: an a5 script that is just the end op ($6A72), right after this function: the
   after-quota pc until the script sets its own */
static constexpr uint32_t Scr5_00939E = 0x00939Eu;

enum {
    WAVE_NO_LIMIT = 0x7FFF,   /* quota and caps until the script sets them */
};

/* a 68000 address held in a register, as a pointer (PTR(T): the 68000 value kept) */
template <class T> static T *ptr(uint32_t a)
{
    PTR(T) p;
    p.v = a;
    return p;
}

/* Wave_Start - spawns a wave director running a5 script a5 at d4/d5 and counts it in g_WaveCount.
 * In:  a5 = the a5-VM script; d4/d5 = the position
 * Out: d0 = 0 and a6 = the object (serial = ++g_WaveSerial, no quota or cap, the return stack
 *      empty); the pool short: Obj_New's d0 = -1. Flags: tst(d0.l) (moveq #0, or Obj_New's moveq #-1)
 * Evidence: lea VM5_RunObject,a0 / bsr.w Obj_New / bne.s rts / addq.w #1,g_WaveSerial / move.w
 *           g_WaveSerial,$A(a6) / move.l a5,$1E(a6) / move.w d4,$22(a6) / move.w d5,$24(a6) / clr.w
 *           $26/$28 / move.w #$7FFF,$E..$14 / move.l #$939E,$16(a6) / clr.w $32/$8/$C / clr.l $2A/$2E /
 *           $34 = a6 + $36 / addq.w #1,g_WaveCount / moveq #0,d0; Wave_StartScriptArg, Wave_StartFromEntry
 *           (falling in). Prototype: tst(d0.l) added (the flags, its ccout)
 */
void Wave_Start(uint32_t *d0, uint16_t d4, uint16_t d5, uint32_t *a0, uint32_t *a1, uint32_t a5, uint32_t *a6)
{
    *a0 = at_VM5_RunObject;
    Obj_New(d0, a0, a1, a6);
    if (*d0 != 0)
        return;   /* the pool is short */
    WaveObj_932E *wave = ptr<WaveObj_932E>(*a6);
    g_WaveSerial++;
    wave->serial = g_WaveSerial;
    wave->script = a5;
    wave->x = d4;
    wave->y = d5;
    wave->origin_x = 0;
    wave->origin_y = 0;
    for (int i = 0; i < 2; i++) {
        wave->quota[i] = WAVE_NO_LIMIT;
        wave->cap[i] = WAVE_NO_LIMIT;
    }
    wave->after_quota = Scr5_00939E;
    wave->kill_group = 0;
    wave->unk_08 = 0;
    wave->spawn_count = 0;
    wave->interval = 0;
    wave->timer = 0;
    wave->stack_ptr = (uint16_t)dk::handle_of(wave->stack);   /* move.w a6,d0 / addi.w #$36,d0 */
    g_WaveCount++;
    *d0 = 0;
}
