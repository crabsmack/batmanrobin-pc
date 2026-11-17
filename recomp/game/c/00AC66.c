#include "game.h"

/* The line-tint flashes and their per-frame steps ($00AC66-$00AD13): a flash sets g_TintLevel
 * (a brightness offset toward white) and its decay speed; a scene then calls a Tint_StepLine
 * every frame, which steps the level back to 0 and applies it to one palette line's r/g/b. */

/* one flash start: the level decays to 0 by 2 every delay + 1 frames; the ccr of the last
   move.b (#delay) */
static void tint_flash(uint8_t level, uint8_t delay)
{
    g_TintTarget = 0;
    g_TintLevel = level;
    g_TintDelay = delay;
    g_TintDelayCount = delay;
}

/* Tint_FlashC1 - starts a line-tint flash: +$C decaying by 2 every frame.
 * In:  none
 * Out: g_TintTarget = 0, g_TintLevel = $C, g_TintDelay = g_TintDelayCount = 0; ccr of move.b #0
 * Evidence: clr.b g_TintTarget / move.b #$C,g_TintLevel / move.b #0,g_TintDelay / move.b
 *           #0,g_TintDelayCount; pointers in boss/stage scripts ($018144, $0181A0 ...: a2
 *           natives); 20 corpus runs, TAS 1004 frames
 */
void Tint_FlashC1(dk::Flags *ccr)
{
    tint_flash(0x0C, 0);
    *ccr = dk::Flags::test(0, 1, ccr->x);
}

/* Tint_FlashC3 - starts a line-tint flash: +$C decaying by 2 every 3rd frame.
 * In:  none
 * Out: g_TintTarget = 0, g_TintLevel = $C, g_TintDelay = g_TintDelayCount = 2
 * Evidence: the same four stores; jsr from the native $029608 after Fade_FlashWhite7; not run in
 *           the corpus
 */
void Tint_FlashC3(void)
{
    tint_flash(0x0C, 2);
}

/* Tint_FlashE2 - starts a line-tint flash: +$E decaying by 2 every 2nd frame.
 * In:  none
 * Out: g_TintTarget = 0, g_TintLevel = $E, g_TintDelay = g_TintDelayCount = 1; ccr of move.b #1
 * Evidence: the same four stores; pointers at $048E14 and $048E50; not run in the corpus
 */
void Tint_FlashE2(dk::Flags *ccr)
{
    tint_flash(0x0E, 1);
    *ccr = dk::Flags::test(1, 1, ccr->x);
}

/* Tint_FlashA2 - starts a line-tint flash: +$A decaying by 2 every 2nd frame.
 * In:  none
 * Out: g_TintTarget = 0, g_TintLevel = $A, g_TintDelay = g_TintDelayCount = 1; ccr of move.b #1
 * Evidence: the same four stores; pointers at $02D084, $048B76, $048DC6, $048DF6, $048E32; not
 *           run in the corpus
 */
void Tint_FlashA2(dk::Flags *ccr)
{
    tint_flash(0x0A, 1);
    *ccr = dk::Flags::test(1, 1, ccr->x);
}

/* Tint_FlashE6 - starts a line-tint flash: +$E decaying by 2 every 6th frame.
 * In:  none
 * Out: g_TintTarget = 0, g_TintLevel = $E, g_TintDelay = g_TintDelayCount = 5; ccr of move.b #5
 * Evidence: the same four stores; the pointer at $048B82; not run in the corpus
 */
void Tint_FlashE6(dk::Flags *ccr)
{
    tint_flash(0x0E, 5);
    *ccr = dk::Flags::test(5, 1, ccr->x);
}

/* Tint_StepLine0 - steps the line tint and applies it to palette line 0 (the same offset for red,
 * green and blue).
 * In:  the g_Tint* state
 * Out: g_Pal0TintR/G/B = the new g_TintLevel; d1 = Tint_Step's step (the result); d0 clobbered
 * Evidence: bsr Tint_Step / 3x move.b d0,g_Pal0Tint*; caller PlayerSelect_ScrollHalves when
 *           g_P1IsRobin is clear
 */
uint32_t Tint_StepLine0(void)
{
    uint8_t level = 0;
    uint32_t step = 0;
    Tint_Step(&level, &step);
    g_Pal0TintR = level;
    g_Pal0TintG = level;
    g_Pal0TintB = level;
    return step;
}

/* Tint_StepLine1 - steps the line tint and applies it to palette line 1.
 * In:  the g_Tint* state
 * Out: level (d0.b) = g_TintLevel, g_Pal1TintR/G/B = it; step (d1) as Tint_Step; ccr of the
 *      last move.b of the level
 * Evidence: bsr Tint_Step / 3x move.b d0,g_Pal1Tint*; caller PlayerSelect_ScrollHalves (when
 *           g_P1IsRobin), pointers at $02962A, $03C752
 */
void Tint_StepLine1(uint8_t *level, uint32_t *step, dk::Flags *ccr)
{
    Tint_Step(level, step);
    g_Pal1TintR = *level;
    g_Pal1TintG = *level;
    g_Pal1TintB = *level;
    *ccr = dk::Flags::test(*level, 1, ccr->x);
}
