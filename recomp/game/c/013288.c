#include "game.h"

/* Drop_RunFxScript - shared tail of the Drop_ actions: runs a4 script as a parentless effect at
 * the position, radius and heading 0.
 * In:  x/y = pixels (d4/d5 low words); a4 = the script (passed on)
 * Out: radius = heading = 0 (d2/d3), x/y swapped into 16.16 (integer part in the high word), then
 *      Fx_RunScriptNoParent
 * Evidence: moveq #0,d2 / moveq #0,d3 / swap d4 / swap d5 / jmp Fx_RunScriptNoParent; branched to
 *           by the 12 Drop_HealthA / Drop_HealthB entries; d2/d3 are Fx_RunScript's radius/heading (NOTES a4
 *           effect spawns)
 */
void Drop_RunFxScript(uint32_t *radius, uint32_t *heading, uint32_t *x, uint32_t *y, uint32_t *a0, uint32_t *a4,
                      uint32_t *a6)
{
    *radius = 0;
    *heading = 0;
    *x = (*x << 16) | (*x >> 16);
    *y = (*y << 16) | (*y >> 16);
    dk::jump(at_Fx_RunScriptNoParent);
}

/* Drop_CollectHealth - a4 native of the health pickups: the player who touched one gets 16 health,
 * capped at $100 (full). A word over $7FFF after the add counts as negative in the signed compare
 * and is kept (the original's ble). Not a player's box: Item_BoxPlayer returns past this.
 * In:  box = the touching collision box (a2)
 * Out: the player's health; the box owner (a1, Item_BoxPlayer's); the flags of the cap's cmpi.w
 *      (or of the move.w #$100 when it capped)
 * Evidence: movea.l a2,a1 / bsr Item_BoxPlayer / addi.w #$10,$4(a0) / cmpi.w #$100 / ble / move.w
 *           #$100; op $5438 pointers at $01324C (B pickup) and $013282 (A pickup) after sound $12;
 *           37 corpus runs
 */
const void *Drop_CollectHealth(const HitBox *box, dk::Flags *ccr)
{
    const void *who = box;
    PlayerRec *p = 0;
    dk::Flags f = *ccr;
    Item_BoxPlayer(&p, &who, &f);
    uint16_t before = p->health;
    p->health = (uint16_t)(before + 0x10);
    bool x = (uint32_t)before + 0x10 > 0xFFFF;   /* addi's carry (cmpi keeps X) */
    *ccr = dk::Flags::compare(p->health, 0x100, 2, x);
    if (!(ccr->z || ccr->n != ccr->v)) {         /* bgt: over full */
        p->health = 0x100;
        *ccr = dk::Flags::test(0x100, 2, x);
    }
    return who;
}
