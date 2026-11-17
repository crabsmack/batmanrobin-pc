#include "game.h"

static constexpr uint32_t BEAMSHIP_NOSE_DX = 0x180000;   /* shots leave $18 px left of the ship (16.16) */

/* Boss_BeamShip_FireAtSpot - ship native: fires an a4 shot script from the ship's nose (x - $18)
 * along the heading from the ship to the beam spot (its saved position) plus the script's word
 * offset
 * In:  script = word heading offset, then the shot's a4 script (a long, Fx_RunScriptArg's); ship
 *      = the ship (a6); the rest its registers
 * Out: d0/d1 low words = the spot - the ship (x, y); d2 = 0; d3 = the heading in the high word
 *      (its low word = d3's old high word: swap); d4/d5 = the nose's 16.16 x/y; then as
 *      Fx_RunScriptArg (script past the arguments)
 * Evidence: move.w $46(a6),d0 / sub.w $3E(a6),d0 / move.w $4A(a6),d1 / sub.w $42(a6),d1 / jsr
 *           Mot_Atan2 / move.w d2,d3 / add.w (a2)+,d3 / swap d3 / moveq #0,d2 / move.l $3E(a6),d4 /
 *           subi.l #$180000,d4 / move.l $42(a6),d5 / jmp Fx_RunScriptArg; op $2CB2 in the attack
 *           scripts $2E2A8/$2E5A6 in fans of 3-6 (offsets -$3C, 0, $3C ..., script $2E9CE);
 *           fuzz-77400-s2 only
 */
void Boss_BeamShip_FireAtSpot(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5,
                              uint32_t d6, uint32_t d7, uint32_t a1, const s16 **script, uint32_t a3, uint32_t a4,
                              uint32_t a5, const Vm2Obj *ship)
{
    /* the spot's integer position: the high words of the saved 16.16 position */
    uint16_t dx = (uint16_t)(((uint32_t)ship->saved_x >> 16) - (uint16_t)ship->x);
    uint16_t dy = (uint16_t)(((uint32_t)ship->saved_y >> 16) - (uint16_t)ship->y);
    *d0 = (*d0 & 0xFFFF0000u) | dx;
    *d1 = (*d1 & 0xFFFF0000u) | dy;
    Mot_Atan2(d0, d1, d2);
    uint16_t heading = (uint16_t)((uint16_t)*d2 + (uint16_t)*(*script)++);
    uint32_t d3_hi = *d3 >> 16;                     /* move.w d2,d3 keeps d3's high word ... */
    *d3 = ((uint32_t)heading << 16) | d3_hi;        /* ... which swap moves down */
    *d2 = 0;
    *d4 = (((uint32_t)(uint16_t)ship->x << 16) | ship->x_frac) - BEAMSHIP_NOSE_DX;
    *d5 = ((uint32_t)(uint16_t)ship->y << 16) | ship->y_frac;
    dk::jump(at_Fx_RunScriptArg);
}

/* Boss_BeamShip_SetShipVertex - ship native, every frame: stores the ship's x/y as span vertex 0
 * (the beam's source; the callback subtracts $18), then falls into Boss_BeamShip_RunRadiusObj
 * In:  ship = the ship (a6), a4 its VM block; the rest its registers
 * Out: g_TriX0/g_TriY0 = the ship's x/y; then as Boss_BeamShip_RunRadiusObj
 * Evidence: move.w $3E(a6),g_TriX0 / move.w $42(a6),g_TriY0, falling into the next file; op $2CB2
 *           at $029310 and $02936E in the ship's main and death loops (Scr2_02928A); TAS
 *           f77185-f79028. d0-d7 are in/out as Boss_BeamShip_RunRadiusObj's
 */
void Boss_BeamShip_SetShipVertex(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6,
                                 uint32_t *d7, uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4, uint32_t a5,
                                 const Vm2Obj *ship)
{
    g_TriX0 = (u16)ship->x;
    g_TriY0 = (u16)ship->y;
    dk::jump(at_Boss_BeamShip_RunRadiusObj);   /* falls into the next function file */
}
