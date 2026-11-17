#include "game.h"

/* the shot frame per weapon type (rows of 8 levels; the flying section's 3 rows from +$30) */
static constexpr uint32_t Tbl_010A1E = 0x010A1Eu;

/* Weapon_KeepShotArtOne - for one player, holds the art of the shot frame for its weapon type and
 * level (the flying set while g_PlayMode bit 1), so shots spawn without an art load; a player out
 * of play holds none. After a change the previous frame is released 20 frames later (a second
 * change inside those 20 releases the one before it at once).
 * In:  slot = the player's slot (a5); rec = the player record (a6)
 * Out: d1 = the wanted frame; d0 = g_PlayMode & 2 (in play); d2 as the last Spr_ call left it;
 *      the slot
 * Evidence: moveq #0,d1 / btst #0,$25(a6) / move.w $28(a6),d1 / lsl.w #3 / add.w $2A(a6) / add.w
 *           d1,d1 / andi.w #2 of g_PlayMode / addi.w #$30 / move.w Tbl_010A1E(pc,d1.w),d1 / cmp.w
 *           (a5),d1 / moveq #0,d2 / jsr Spr_ChangeFrame / the slot updates, #$14 / jmp
 *           Spr_ReleaseFrame (twice); TAS RAM: $238C on foot, $2594 type 2 level 6, $2684 flying
 */
void Weapon_KeepShotArtOne(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *a0, uint32_t *a1,
                           ShotArtSlot *slot, const PlayerRec *rec)
{
    *d1 = 0;
    if (rec->flags & PLAYER_IN_PLAY) {
        uint16_t index = (uint16_t)((((uint16_t)rec->weapon_type << 3) + (uint16_t)rec->weapon_level) * 2);
        uint16_t flying = (uint16_t)(g_PlayMode & 2);
        *d0 = (*d0 & 0xFFFF0000u) | flying;
        if (flying)
            index = (uint16_t)(index + 0x30);
        PTR(const u16) tbl;
        tbl.v = Tbl_010A1E + (uint32_t)(int32_t)(int16_t)index;
        *d1 = *tbl;
    }
    if ((uint16_t)*d1 != slot->frame) {
        *d2 = 0;
        Spr_ChangeFrame(d0, d1, d2, d3, a0, a1);
        uint16_t held = slot->frame;
        *d2 = (*d2 & 0xFFFF0000u) | held;
        if (held != (uint16_t)*d1) {
            slot->frame = (uint16_t)*d1;
            if (held == 0)
                return;
            if (slot->release_timer) {
                /* a change while the last one's release is pending: release that one now */
                slot->release_timer = 0x14;
                *d0 = (*d0 & 0xFFFF0000u) | held;
                *d2 = (*d2 & 0xFFFF0000u) | (uint16_t)slot->old_frame;
                slot->old_frame = held;
                dk::jump(at_Spr_ReleaseFrame);
                return;
            }
            slot->release_timer = 0x14;
            slot->old_frame = held;
            return;
        }
    }
    if (!slot->release_timer)
        return;
    if (--slot->release_timer)
        return;
    *d2 = (*d2 & 0xFFFF0000u) | (uint16_t)slot->old_frame;
    dk::jump(at_Spr_ReleaseFrame);
}
