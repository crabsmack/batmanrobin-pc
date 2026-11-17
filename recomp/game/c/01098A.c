#include "game.h"

/* Weapon_KeepShotArt - keeps both players' current weapon-shot art resident in VRAM:
 * Weapon_KeepShotArtOne for player 1 and its slot, then (falling into it) for player 2.
 * In:  none
 * Out: a6 = g_Player2, a5 = its slot; then Weapon_KeepShotArtOne's
 * Evidence: lea g_Player1,a6 / lea g_ShotArtSlots,a5 / bsr.s Weapon_KeepShotArtOne / lea $40(a6),a6
 *           / lea $6(a5),a5 and the fall into Weapon_KeepShotArtOne; called by the frame start
 *           $9B86 every gameplay frame
 */
void Weapon_KeepShotArt(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *a0, uint32_t *a1,
                        uint32_t *a5, uint32_t *a6)
{
    PlayerRec *rec = (PlayerRec *)g_Player1;
    ShotArtSlot *slot = (ShotArtSlot *)g_ShotArtSlots;
    Weapon_KeepShotArtOne(d0, d1, d2, d3, a0, a1, slot, rec);
    *a6 = dk::addr(rec + 1);
    *a5 = dk::addr(slot + 1);
    dk::jump(at_Weapon_KeepShotArtOne);
}
