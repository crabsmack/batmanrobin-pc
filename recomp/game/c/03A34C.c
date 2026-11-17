#include "game.h"

/* the inline arguments after the jsr: dc.w links, dc.l link table, dc.l chain script */
static constexpr uint32_t ToyChainArgs_03A350 = 0x03A350u;

/* ToyChain_MakeBox10Wide - inline chain maker for Enemy_GiftBox_SpawnRoamWide10: 10 figures (table
 * $03A512: growth $2C0, energy $10) that sway wide (oscillator X limit $1E00, speed $100, accel
 * $78)
 * In:  a6 = the gift box (field $2A); inline dc.w $A, dc.l $03A512, dc.l $03A35A
 * Out: d0 = 0 and the chain object, or nonzero (pool full)
 * Evidence: jsr ToyChain_SpawnInline + `000A 0003A512 0003A35A`; Op2_SetOscX `0008 1E00 0100 0100
 *           0078`; Op2_SpawnChild 002A at $039CDE; TAS f108754-f112230 (section $3400). The maker
 *           after it ($03A372, used only by the never-run box $03A264) is still raw;
 *           ToyChain_SpawnInline pops the return address as its argument pointer and returns to
 *           this function's caller
 */
void ToyChain_MakeBox10Wide(uint16_t *d1, uint32_t *d4, uint32_t *d5, uint32_t *d7, uint32_t *a1, uint32_t a4,
                            uint32_t a5, uint32_t *a6)
{
    dk::jump_script(at_ToyChain_SpawnInline, ToyChainArgs_03A350);
}
