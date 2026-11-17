#include "game.h"

/* The Mad Hatter's drop natives (a2 natives, op $2CBC): bomb rabbits from the hat and toys thrown
 * in arcs. */

enum {
    HATTER_HOP_RABBIT_DY = 0x0A,     /* the hopping rabbit starts just below the hat's position */
    HATTER_DROP_RABBIT_DY = 0x46,    /* the dropping rabbit starts below the hat (70 px) */
};
static constexpr uint32_t HATTER_LAUNCH_DX = 0xA0000u;   /* toys leave 10 px right of the hat (16.16) */

/* Boss_Hatter_DropHopRabbit - spawns a hopping bomb rabbit (Enemy_BombRabbit_SpawnHop) at the
 * hat's x, y + 10
 * In:  boss = the Hatter (a6: x/y); d0/d1/d7/a4 pass through to the spawner
 * Out: x/y (d4/d5 low words) = the spawn position; d0/d1/d7 and the condition codes = the
 *      spawner's; a2-a6 kept (movem)
 * Evidence: move.w $3E(a6),d4 / move.w $42(a6),d5 / addi.w #$A,d5 / movem.l a2-a6,-(a7) / jsr
 *           Enemy_BombRabbit_SpawnHop / movem.l (a7)+,a2-a6 / rts; dc.l $0003D4B8 in the attack
 *           script Scr2_03E39A; TAS f114032-f114152
 */
void Boss_Hatter_DropHopRabbit(uint32_t *d0, uint16_t *d1, uint16_t *x, uint16_t *y, uint32_t *d7, uint32_t a4,
                               const Vm2Obj *boss)
{
    *x = (uint16_t)boss->x;
    *y = (uint16_t)(boss->y + HATTER_HOP_RABBIT_DY);
    uint32_t a2 = 0, a3 = 0, saved_a4 = a4, a6 = dk::addr(boss);   /* movem.l a2-a6: the spawner's are dropped */
    Enemy_BombRabbit_SpawnHop(d0, d1, *x, *y, d7, &a2, &a3, &saved_a4, &a6);
}

/* Boss_Hatter_DropRabbit - spawns a bomb rabbit that drops in (Enemy_BombRabbit_SpawnDropIn) at
 * the hat's x, y + 70 (below the hat)
 * In:  boss = the Hatter (a6: x/y); d0/d1/d7/a4 pass through to the spawner
 * Out: x/y (d4/d5 low words) = the spawn position; d0/d1/d7 and the condition codes = the
 *      spawner's; a2-a6 kept (movem)
 * Evidence: move.w $3E(a6),d4 / move.w $42(a6),d5 / addi.w #$46,d5 / movem.l a2-a6 / jsr
 *           Enemy_BombRabbit_SpawnDropIn / movem.l (a7)+,a2-a6 / rts; dc.l $0003D4D2 in the
 *           coroutine-3 script Scr2_03E034 (every $3C0 ticks); fuzz-115200 and the TAS
 */
void Boss_Hatter_DropRabbit(uint32_t *d0, uint16_t *d1, uint16_t *x, uint16_t *y, uint32_t *d7, uint32_t a4,
                            const Vm2Obj *boss)
{
    *x = (uint16_t)boss->x;
    *y = (uint16_t)(boss->y + HATTER_DROP_RABBIT_DY);
    uint32_t a2 = 0, a3 = 0, saved_a4 = a4, a6 = dk::addr(boss);
    Enemy_BombRabbit_SpawnDropIn(d0, d1, *x, *y, d7, &a2, &a3, &saved_a4, &a6);
}

/* Boss_Hatter_LaunchFx - runs the a4 spawn script at the inline offset from 10 px right of the
 * hat (velocity d2/d3 = 0): the hat throwing a spinning toy figure in an arc
 * In:  script = the native's offset word (a2); boss = the Hatter (a6: x/y 16.16)
 * Out: d2/d3 = 0, d4/d5 = the start position (16.16), a0 = the effect script (the word's address +
 *      the word), script past the word; then Fx_RunScript's
 * Evidence: moveq #0,d2 / moveq #0,d3 / move.l $3E(a6),d4 / move.l $42(a6),d5 / addi.l #$A0000,d4 /
 *           movea.l a2,a0 / adda.w (a2)+,a0 / jmp Fx_RunScript; dc.l $0003D4EC in Scr2_03E39A
 *           ($03E47C, $03E48A, $03E4FE ...); fuzz-113400, level-skip. a0 an output (measured as a
 *           clobber): Fx_RunScript, jumped to, takes the script in it
 */
void Boss_Hatter_LaunchFx(uint32_t d0, uint32_t d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5,
                          uint32_t d6, uint32_t d7, uint32_t *a0, uint32_t a1, const u16 **script, uint32_t a3,
                          uint32_t a4, uint32_t a5, const Vm2Obj *boss)
{
    *d2 = 0;
    *d3 = 0;
    *d4 = (((uint32_t)(uint16_t)boss->x << 16) | boss->x_frac) + HATTER_LAUNCH_DX;
    *d5 = ((uint32_t)(uint16_t)boss->y << 16) | boss->y_frac;
    const u16 *word = *script;
    *a0 = dk::addr(word) + (uint32_t)(int32_t)(int16_t)*word;
    *script = word + 1;
    dk::jump(at_Fx_RunScript);
}
