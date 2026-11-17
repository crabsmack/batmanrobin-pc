#include "game.h"

/* Boss_Hatter_RoadCmdGoto - road schedule command 2 (and the tail of the raw gosub 3 and loop 6):
 * moves the schedule pointer by a 12-bit offset (byte 0's high nibble, sign-extended, then byte
 * 1) from the command's start, then reads the command there
 * In:  a0 = past the command, d4's low byte = byte 0, d1 = byte 1; a4 = the boss's VM block
 * Out: d4's low word = the offset, a0 = the new command; then Boss_Hatter_RoadCmdNext's
 * Evidence: ext.w d4 / asl.w #4,d4 / move.b d1,d4 / lea -2(a0,d4.w),a0 / bra.s
 *           Boss_Hatter_RoadCmdNext; F2E8 in the schedule at $03F5BE jumps back $18 bytes;
 *           fuzz-113400-s2. a0 an output (measured: an input): the command reader the bra
 *           reaches takes the new pointer in it
 */
void Boss_Hatter_RoadCmdGoto(uint32_t *d0, uint8_t *d1, uint32_t *d4, uint16_t *d5, uint32_t *a0, uint32_t a4,
                             uint32_t a6)
{
    uint16_t offset = (uint16_t)((int16_t)(int8_t)(uint8_t)*d4 << 4);   /* ext.w / asl.w #4 */
    offset = (uint16_t)((offset & 0xFF00u) | *d1);                      /* move.b d1,d4 */
    *d4 = (*d4 & 0xFFFF0000u) | offset;
    *a0 = *a0 - 2 + (uint32_t)(int32_t)(int16_t)offset;
    dk::jump(at_Boss_Hatter_RoadCmdNext);
}
