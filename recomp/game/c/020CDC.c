#include "game.h"

/* the bombing director's a2 script (ROM, right after this file's code) */
static constexpr uint32_t Scr2_020CF6 = 0x020CF6u;

/* Stage1_SpawnBankBombing - street trigger callback: ends all waves and spawns the bank bombing
 * director Scr2_020CF6, which keeps the trigger entry's extra long (the wave script to start
 * afterwards) at +$38
 * In:  a1 = the trigger entry after its callback long; d4/d5 = x/y
 * Out: a6 = the director, its next_wave = the entry's long, a1 past it; all waves ended; d0 = 0, Z
 *      set. When the pool is full Obj_SpawnEnemyOrQuit returns to this callback's caller itself
 * Evidence: lea Scr2_020CF6,a2 / jsr Obj_SpawnEnemyOrQuit / move.l (a1)+,$38(a6) / jsr Wave_EndAll /
 *           moveq #0,d0; dc.l $00020CDC at $016146 in Tbl_015F5A; TAS f1729
 */
uint32_t Stage1_SpawnBankBombing(uint16_t *d1, uint16_t d4, uint16_t d5, uint32_t *a0, uint32_t *a1, uint32_t *a4,
                                 uint32_t *a5, uint32_t *a6)
{
    uint32_t d0 = 0, d7 = 0, a3 = 0;  /* clobbered */
    Obj_SpawnEnemyOrQuit(&d0, d1, d4, d5, &d7, a0, Scr2_020CF6, &a3, a4, a5, a6);
    PTR(BankBombingObj) at;
    at.v = *a6;
    PTR(const u32) entry;
    entry.v = *a1;
    at->next_wave = *entry;
    *a1 += 4;
    Wave_EndAll();
    return 0;
}
