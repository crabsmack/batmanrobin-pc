#include "game.h"

/* the beam-spot coroutines (a2 scripts after this code; batmanrobin.asm) */
static constexpr uint32_t Scr2_0294A8 = 0x0294A8u;   /* the spot leaves the ship and homes on the player */
static constexpr uint32_t Scr2_0294EC = 0x0294ECu;   /* var 0 = 0 (the disc shrinks), the spot comes back */

static constexpr int BEAM_COLOUR = 15;   /* palette line 1 colour 15: the beam and disc */

/* Boss_BeamShip_BeamOn - ship native: switches the beam on: script var 0 = the wanted disc radius,
 * the beam colour, and coroutine 1 = Scr2_0294A8 (the beam spot homes on the targeted player)
 * In:  script = words radius, colour (a2); vm = the ship's VM block (a4)
 * Out: script past the words; var 0 = radius; palette line 1 colour 15 = colour (g_Pal1Reload
 *      set); coroutine 1 = Scr2_0294A8
 * Evidence: move.w (a2)+,$22(a4) / move.w (a2)+,g_PalSource1+$1E / st.b g_Pal1Reload / move.l
 *           #$294A8,$3C(a4); op $2CB2 in the attack scripts $2DFBA, $2E084, $2E144, $2E2B6 ...
 *           (e.g. radius $64, colour $A66); TAS f77305-f78655
 */
void Boss_BeamShip_BeamOn(const u16 **script, Vm2Block *vm)
{
    vm->var[0] = *(*script)++;
    ((u16 *)g_PalSource1)[BEAM_COLOUR] = *(*script)++;
    g_Pal1Reload = 0xFF;
    vm->coroutine[1].v = Scr2_0294A8;
}

/* Boss_BeamShip_BeamOff - ship native: coroutine 1 = Scr2_0294EC, which zeroes var 0 (the disc
 * shrinks) and brings the beam spot back to the ship
 * In:  vm = the ship's VM block (a4)
 * Out: coroutine 1 = Scr2_0294EC
 * Evidence: move.l #$294EC,$3C(a4) / rts; op $2CB2 at $02934C (death) and in the attack scripts
 *           $2E120, $2E1A2, $2E258, $2E5A6 ...; TAS f77305-f78655
 */
void Boss_BeamShip_BeamOff(Vm2Block *vm)
{
    vm->coroutine[1].v = Scr2_0294EC;
}
