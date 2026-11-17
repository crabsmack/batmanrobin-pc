#include "game.h"

static constexpr uint32_t Tbl_1E7EE4 = 0x1E7EE4u;   /* the three music modules (SndModule) */
static constexpr uint32_t Tbl_1E6028 = 0x1E6028u;   /* per song: SndSongRef (song 0 first) */

/* Snd_PlaySong - starts song d0: looks up its (song in module, module offset) pair and starts it
 * in that module through Snd_StartSong
 * In:  song (d0) low word = the song (1-$13; Sound_RestartMusic never passes 0)
 * Out: module (a0) = the module's Tbl_1E7EE4 entry, d0 low word = the song in the module, then
 *      Snd_StartSong (a tail jump)
 * Evidence: lea Tbl_1E7EE4,a0 / add.w d0,d0 / add.w d0,d0 / adda.w Tbl_1E602A(pc,d0.w),a0 /
 *           move.w Tbl_1E6028(pc,d0.w),d0 / jmp Snd_StartSong(pc); e.g. song 3 = song 5 of
 *           module 1; only caller Sound_RestartMusic $A1DE; 146 corpus runs. The prototype hands
 *           a0 on (measured as clobbered: the caller doesn't read it, Snd_StartSong does)
 */
void Snd_PlaySong(uint32_t *song, uint32_t *d1, uint32_t *d7, uint32_t *module, uint32_t a1)
{
    uint16_t offset = (uint16_t)(*song << 2);   /* add.w d0,d0 twice */
    *song = (*song & 0xFFFF0000u) | offset;
    const SndSongRef *ref = snd_rom<SndSongRef>(Tbl_1E6028 + (uint32_t)(int32_t)(int16_t)offset);
    *module = Tbl_1E7EE4 + (uint32_t)(int32_t)(int16_t)(uint16_t)ref->module_offset;
    *song = (*song & 0xFFFF0000u) | (uint16_t)ref->song;
    dk::jump(at_Snd_StartSong);
}
