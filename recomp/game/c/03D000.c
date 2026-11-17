#include "game.h"

/* the Mad Hatter's a2 script (ROM data right after this file) */
static constexpr uint32_t Scr2_03D032 = 0x03D032u;

enum {
    LINE3_RED_END = 0xE,      /* the colour components' upper bounds (CRAM bit positions); */
    LINE3_GREEN_END = 0xE0,   /* each also bounces at 0 */
    LINE3_BLUE_END = 0xE00,
};

/* Stage3_Line3CycleStep - one step of the line-3 colour walker: blue moves by its step and
 * bounces between 0 and $E00; only when it reaches an end does green move (bouncing 0-$E0), and
 * only at green's ends red (0-$E)
 * In:  red/green/blue = the components (d0/d1/d2), red_step/green_step/blue_step (d3/d4/d5)
 * Out: the components advanced, a step negated at its bound (low words)
 * Evidence: add.w d5,d2 / beq / cmpi.w #$E00 / bne / neg.w d5 / add.w d4,d1 / ... / neg.w d3; bsr
 *           from Stage3_StepLine3Cycle 16 times per frame; TAS f97888-f101674 (1405 frames)
 */
void Stage3_Line3CycleStep(uint16_t *red, uint16_t *green, uint16_t *blue, uint16_t *red_step,
                           uint16_t *green_step, uint16_t *blue_step)
{
    *blue = (uint16_t)(*blue + *blue_step);
    if (*blue != 0 && *blue != LINE3_BLUE_END)
        return;
    *blue_step = (uint16_t)-*blue_step;
    *green = (uint16_t)(*green + *green_step);
    if (*green != 0 && *green != LINE3_GREEN_END)
        return;
    *green_step = (uint16_t)-*green_step;
    *red = (uint16_t)(*red + *red_step);
    if (*red != 0 && *red != LINE3_RED_END)
        return;
    *red_step = (uint16_t)-*red_step;
}

/* Boss_Hatter_Spawn - spawns the Mad Hatter boss (boss class): the giant upside-down top hat with
 * the Hatter on its brim; its script makes the road-steering child, sets score 5000 and energy
 * $1D00/$2D00 (1P/2P), shows the boss meter and steps the road schedule every frame
 * In:  d4/d5 = position
 * Out: script (a2) = Scr2_03D032, then Obj_SpawnBoss's results
 * Evidence: lea Scr2_03D032,a2 / jmp Obj_SpawnBoss; jsr from Stage3_InitHatterRoad; png f114600:
 *           the Hatter standing on the hat's brim. a2 an output (measured as a clobber):
 *           Obj_SpawnBoss, jumped to, takes the script in it
 */
void Boss_Hatter_Spawn(uint16_t x, uint16_t y, uint32_t *a3, uint32_t *a4, uint32_t a6, uint32_t *script)
{
    *script = Scr2_03D032;
    dk::jump(at_Obj_SpawnBoss);
}
