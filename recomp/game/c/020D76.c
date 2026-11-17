#include "game.h"

/* the a4 effect scripts of the bombing (ROM) */
static constexpr uint32_t Scr4_020E5C = 0x020E5Cu;    /* the row of explosions along the upper floor */
static constexpr uint32_t Scr4_020F28 = 0x020F28u;    /* the big blast at the window */
static constexpr uint16_t BOMBING_FIRE_X = 0xA0;      /* screen x of the window that catches fire */
static constexpr uint16_t BOMBING_LEFT_SIGN_X = 0x4D;
static constexpr uint16_t BOMBING_RIGHT_SIGN_X = 0xF4;

/* the camera's integer x (the high word of the 16.16 g_CameraX) */
static uint16_t camera_x(void) { return (uint16_t)(g_CameraX >> 16); }

/* Stage1_BombingStartWave - last step of the bank bombing: starts the wave director whose script the
 * trigger entry gave (the director's next_wave)
 * In:  self = the bombing director (a6); d4/d5 = the wave's position
 * Out: script (a5) = next_wave, then as Wave_Start (d0 = 0 and a6 = the wave director)
 * Evidence: movea.l $38(a6),a5 / jmp Wave_Start; Op2_CallNativeRelSaved at $020D72 in Scr2_020CF6,
 *           then Op2_Kill; TAS f2118
 */
uint32_t Stage1_BombingStartWave(uint16_t d4, uint16_t d5, const BankBombingObj *self, uint32_t *script)
{
    *script = self->next_wave;
    dk::jump(at_Wave_Start);
    return 0;                         /* Wave_Start sets d0 */
}

/* Stage1_BombingExplosionRow - bombing native: spawns the a4 effect script Scr4_020E5C at the screen
 * centre: the row of explosions that sweeps the bank's upper floor
 * In:  a6 = the bombing director
 * Out: script (a4) = Scr4_020E5C, then as Fx_SpawnAtScreenCentre
 * Evidence: lea Scr4_020E5C,a4 / jmp Fx_SpawnAtScreenCentre; the first native of Scr2_020CF6
 *           ($020D14); shots f1760/f1790: explosions running along the upper windows
 */
void Stage1_BombingExplosionRow(uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *a0,
                                uint32_t *script, uint32_t *a6)
{
    *script = Scr4_020E5C;
    dk::jump(at_Fx_SpawnAtScreenCentre);
}

/* Stage1_BombingBigBlast - bombing native: spawns the a4 effect script Scr4_020F28 at the screen
 * centre: the large blast at the window where the fire then starts
 * In:  a6 = the bombing director
 * Out: script (a4) = Scr4_020F28, then as Fx_SpawnAtScreenCentre
 * Evidence: lea Scr4_020F28,a4 / jmp Fx_SpawnAtScreenCentre; $020D5A in Scr2_020CF6, right after
 *           Pal_FlashWhite and Shake_StartBig; shot f2116
 */
void Stage1_BombingBigBlast(uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *a0,
                            uint32_t *script, uint32_t *a6)
{
    *script = Scr4_020F28;
    dk::jump(at_Fx_SpawnAtScreenCentre);
}

/* Stage1_BombingStartFire - bombing native: starts the building fire at the upper window nearest
 * the screen centre (camera x + $A0)
 * In:  -
 * Out: d4 low word = g_CameraX + $A0, then as Stage1_StartFireAt
 * Evidence: move.w g_CameraX,d4 / addi.w #$A0,d4 / jmp Stage1_StartFireAt; $020D5E in Scr2_020CF6;
 *           the fire burns from f2116 on (png f002400)
 */
void Stage1_BombingStartFire(uint32_t *d0, uint32_t *d1, uint16_t *d2, uint16_t *d4)
{
    *d4 = (uint16_t)(camera_x() + BOMBING_FIRE_X);
    dk::jump(at_Stage1_StartFireAt);
}

/* Stage1_BombingBlastLeftSign - bombing native: blasts the bank sign under screen x $4D (the left one)
 * In:  a6 for Stage1_BlastSignAt
 * Out: d4 low word = g_CameraX + $4D, then as Stage1_BlastSignAt
 * Evidence: move.w g_CameraX,d4 / addi.w #$4D,d4 / bra.w Stage1_BlastSignAt; $020D24 in Scr2_020CF6
 *           after a white flash; png f002400 shows both signs blasted
 */
void Stage1_BombingBlastLeftSign(uint32_t *d0, uint32_t *d1, uint16_t *d4, uint16_t *d5, uint16_t *d6, uint32_t *d7,
                                 uint32_t a6)
{
    *d4 = (uint16_t)(camera_x() + BOMBING_LEFT_SIGN_X);
    dk::jump(at_Stage1_BlastSignAt);
}

/* Stage1_BombingBlastRightSign - bombing native: blasts the bank sign under screen x $F4 (the right one)
 * In:  a6 for Stage1_BlastSignAt
 * Out: d4 low word = g_CameraX + $F4, then as Stage1_BlastSignAt
 * Evidence: move.w g_CameraX,d4 / addi.w #$F4,d4 / bra.w Stage1_BlastSignAt; $020D34 in Scr2_020CF6,
 *           100 frames after the left sign
 */
void Stage1_BombingBlastRightSign(uint32_t *d0, uint32_t *d1, uint16_t *d4, uint16_t *d5, uint16_t *d6, uint32_t *d7,
                                  uint32_t a6)
{
    *d4 = (uint16_t)(camera_x() + BOMBING_RIGHT_SIGN_X);
    dk::jump(at_Stage1_BlastSignAt);
}
