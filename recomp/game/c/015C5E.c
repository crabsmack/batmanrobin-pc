#include "game.h"

/* the 4 sign slots' records (16 bytes each; batmanrobin.asm: the asm points a4 16 bytes before it,
   at loc_015CE4, and steps it first) */
static constexpr uint32_t Tbl_015CF4 = 0x015CF4u;
static constexpr int SIGN_SLOTS = 4;

/* Stage1_VIntFacade - V-int object of the stage 1 street: steps the building fire, then redraws the
 * bank-sign panels whose wanted state differs from the drawn one
 * In:  vdp_ctrl/vdp_data = the VDP ports (a0/a1, from the V-int object list $8ADA); a2 for
 *      Stage1_VIntFireStep
 * Out: each changed slot's drawn state = wanted, its panel drawn; nothing while g_FrameBusy
 * Evidence: tst.b g_FrameBusy / bne / move.b g_Stage1FireState,d0 / ble / bsr Stage1_VIntFireStep;
 *           the loop: a4 += $10 / move.b (a5)+,d0 / cmp.b (a6)+,d0 / dbne d7 / beq out / move.b
 *           d0,-1(a6) / bsr Stage1_DrawSignPanel / subq.w #1,d6 / dbeq d7 (d6 = 4 draws, d7 = 4
 *           slots: the d6 limit is reached only with the last slot, so every slot is looked at);
 *           installed by the street scene $1FB3A through $8B7E; TAS f532-f6334 every frame, 10 runs
 */
void Stage1_VIntFacade(uint32_t vdp_ctrl, uint32_t vdp_data, uint32_t a2)
{
    if (g_FrameBusy)
        return;
    int8_t fire = (int8_t)g_Stage1FireState;
    if (fire > 0)
        Stage1_VIntFireStep((uint8_t)fire, vdp_ctrl, vdp_data, a2);
    Stage1StreetVars *vars = (Stage1StreetVars *)g_SceneVars;
    PTR(const Stage1SignSlot) table;
    table.v = Tbl_015CF4;
    const Stage1SignSlot *slots = table;
    for (int i = 0; i < SIGN_SLOTS; i++) {
        uint8_t wanted = vars->sign_wanted[i];
        if (wanted == vars->sign_drawn[i])
            continue;
        vars->sign_drawn[i] = wanted;
        Stage1_DrawSignPanel(wanted, vdp_ctrl, vdp_data, &slots[i]);
    }
}
