#include "game.h"

/* Op2_PlaySound / Op2_PlayMusic / Op2_BranchPopulationNz ($00433E-$004369). */

/* NEXT: the handler the script's next word names, jumped to */
static uint32_t op_next(const u16 **script)
{
    uint32_t handler = dk::code((int16_t)(uint16_t)*(*script)++);
    dk::jump(handler);
    return handler;
}

/* Op2_PlaySound - a2 op: plays a sound effect through the driver request Snd_PlaySfx (0 = none).
 * In:  script = word sound id
 * Out: sound (d0) low word = the id, then Snd_PlaySfx's d0; NEXT
 * Evidence: move.w (a2)+,d0 / jsr (Snd_PlaySfx).l / NEXT (Snd_PlaySfx: subq #1 / bmi rts; lsl #3
 *           into Tbl_1E50D6, a priority check); 133 corpus runs; $13436 `433E 0041`
 */
void Op2_PlaySound(uint32_t *sound, const u16 **script)
{
    *sound = (*sound & 0xFFFF0000u) | *(*script)++;
    Snd_PlaySfx(sound);
    op_next(script);
}

/* Op2_PlayMusic - a2 op: switches the music to a song (Sound_PlayMusic), a1-a6 saved on the stack
 * around the call.
 * In:  script = word song; a1-a6 (pushed: movem.l a1-a6,-(a7))
 * Out: song (d0) low word = the song, then Sound_PlayMusic's d0 / d1 / d7; handler = NEXT's
 * Evidence: move.w (a2)+,d0 / movem.l a1-a6,-(a7) / jsr (Sound_PlayMusic).l /
 *           movem.l (a7)+,a1-a6 / NEXT; 3 corpus runs; $18080 `434A 0001`
 */
void Op2_PlayMusic(uint32_t *song, uint32_t *d1, uint32_t *d7, uint32_t *handler, uint32_t a1, const u16 **script,
                   uint32_t a3, uint32_t a4, uint32_t a5, uint32_t a6)
{
    *song = (*song & 0xFFFF0000u) | *(*script)++;
    uint32_t a2 = ADDR68(*script);
    /* movem.l a1-a6,-(a7): a1 ends at the lowest address */
    dk::push_long(a6);
    dk::push_long(a5);
    dk::push_long(a4);
    dk::push_long(a3);
    dk::push_long(a2);
    dk::push_long(a1);
    Sound_PlayMusic(song, d1, d7, a1, a2, a3, a4, a5, a6);
    for (int i = 0; i < 6; i++)
        dk::pop_long();
    *handler = op_next(script);
}

/* Op2_BranchPopulationNz - a2 op: branches (Op2_Goto) while any weighted script object is alive
 * (g_ObjPopulation != 0), else NEXT after the offset.
 * In:  script = word offset; g_ObjPopulation
 * Out: the branch, else handler = NEXT's
 * Evidence: tst.w (g_ObjPopulation).w / bne.w Op2_Goto / addq.l #2,a2 / NEXT; 5 corpus runs;
 *           $1E96A `435E FFCE`
 */
void Op2_BranchPopulationNz(uint32_t *handler, const u16 **script)
{
    if (g_ObjPopulation != 0) {
        dk::jump(at_Op2_Goto);
        return;
    }
    (*script)++;
    *handler = op_next(script);
}
