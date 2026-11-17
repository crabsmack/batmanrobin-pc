#include "game.h"

enum {
    SHAKE_RUMBLE_EVERY = 3,     /* frame counter & 3 == 0: one kick every 4th frame */
};

/* a random whole-pixel launch speed: (Rand_Below(range) + 3) << 16 (Rand_Below's result is
   zero-extended, so the swap leaves a 0 fraction) */
static uint32_t random_launch(uint32_t *d0, uint16_t range)
{
    *d0 = (*d0 & 0xFFFF0000u) | range;   /* move.w #range,d0 */
    Rand_Below(d0);
    *d0 = (*d0 & 0xFFFF0000u) | (uint16_t)(*d0 + 3);
    *d0 = *d0 << 16 | *d0 >> 16;         /* swap */
    return *d0;
}

/* Shake_StartBig - starts the big screen shake: 12 px up, pulled back 3 px per frame.
 * In:  none
 * Out: g_ShakeVel = 12.0, g_ShakeGravity = 3.0; then Shake_Arm
 * Evidence: move.l #$C0000,g_ShakeVel / move.l #$30000,g_ShakeGravity / bra.s Shake_Arm (the
 *           previous file); callers $2417E and 16 script uses ($20D54, $26B52, $3C774...)
 */
void Shake_StartBig(void)
{
    g_ShakeVel = 0xC0000u;
    g_ShakeGravity = 0x30000u;
    dk::jump(at_Shake_Arm);
}

/* Shake_StartRandom - native callback: starts a 3-5 px shake (random), pulled back 2 px per frame,
 * over a running one too.
 * In:  none
 * Out: d0 = g_ShakeVel = 3.0-5.0, g_ShakeGravity = 2.0; then Shake_Arm
 * Evidence: move.w #3,d0 / bsr.w Rand_Below / addq.w #3,d0 / swap d0 / move.l d0,g_ShakeVel /
 *           move.l #$20000,g_ShakeGravity / bra.s Shake_Arm; a script's native call names $9604
 *           itself (the corpus's call-site log: 5,835 calls)
 */
void Shake_StartRandom(uint32_t *d0)
{
    g_ShakeVel = random_launch(d0, 3);
    g_ShakeGravity = 0x20000u;
    dk::jump(at_Shake_Arm);
}

/* Shake_StartRandomIfIdle - native callback: Shake_StartRandom unless a shake runs.
 * In:  none
 * Out: a shake running: the tst.l flags (Z clear), nothing changed; else Shake_StartRandom's
 * Evidence: tst.l g_ShakeY / bne.s Shake_Done (the previous file), falling into Shake_StartRandom;
 *           script uses $3B6A8, $3B860, $3CC52. Prototype: &ccr added (the early exit's flags)
 */
void Shake_StartRandomIfIdle(uint32_t *d0, dk::Flags *ccr)
{
    *ccr = dk::Flags::test(g_ShakeY, 4, ccr->x);
    if (g_ShakeY != 0) {
        dk::jump(at_Shake_Done);
        return;
    }
    Shake_StartRandom(d0);
}

/* Shake_Rumble - every 4th frame kicks the shake 3-7 px up (random, pull 2); then steps it
 * (Shake_Update, which it falls into).
 * In:  none
 * Out: off frames: nothing; kick frames: g_ShakeVel = 3.0-7.0, g_ShakeGravity = 2.0, then
 *      Shake_Update's outputs
 * Evidence: move.b $E031 (g_FrameCounter's low byte),d0 / andi.b #3,d0 / beq / rts / move.w #5,d0 /
 *           bsr.w Rand_Below / addq.w #3,d0 / swap d0 / ... falling into Shake_Update (the next
 *           file); caller $2417E
 */
void Shake_Rumble(uint32_t *d1, uint32_t *d2)
{
    (void)d1, (void)d2;
    if ((uint8_t)g_FrameCounter & SHAKE_RUMBLE_EVERY)
        return;
    uint32_t d0 = (uint8_t)g_FrameCounter & SHAKE_RUMBLE_EVERY;   /* d0 is a clobber: its high bytes don't matter */
    g_ShakeVel = random_launch(&d0, 5);
    g_ShakeGravity = 0x20000u;
    dk::jump(at_Shake_Update);
}
