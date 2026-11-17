#include "game.h"

/* Boss_Hatter_RoadCmdTbl_Goto - entry 2 of Boss_Hatter_RoadCmdTbl: the goto command
 * In:  as Boss_Hatter_RoadCmdGoto
 * Out: as Boss_Hatter_RoadCmdGoto
 * Evidence: bra.s Boss_Hatter_RoadCmdGoto at $03DD36 ($03DD32 + 2 x 2); fuzz-113400-s2 (the
 *           schedule's F2E8 loop)
 */
void Boss_Hatter_RoadCmdTbl_Goto(uint32_t *d0, uint8_t *d1, uint32_t *d4, uint16_t *d5, uint32_t a0, uint32_t a4,
                                 uint32_t a6)
{
    dk::jump(at_Boss_Hatter_RoadCmdGoto);
}
