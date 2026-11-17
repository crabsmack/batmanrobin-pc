#include "game.h"

/* Boss_Hatter_RoadCmdRandom - road schedule command 7: picks one of byte 1 entries of the
 * word-offset table right after the command (the pick = Rand_Next's low word x byte 1, high word)
 * and continues at that entry + its offset
 * In:  a0 = past the command (the table), d1 = byte 1 (the entry count); a4 = the boss's VM block
 * Out: d0 = Rand_Next's; d1's low word = 2 x the pick, its upper word the product's low word (swap);
 *      a0 = the picked entry + its word; then Boss_Hatter_RoadCmdNext
 * Evidence: jsr Rand_Next / ext.w d1 / mulu.w d0,d1 / swap d1 / add.w d1,d1 / adda.w d1,a0 /
 *           adda.w (a0),a0 / bra.s Boss_Hatter_RoadCmdNext; reached by Boss_Hatter_RoadCmdTbl entry 7
 *           ($03DD40); the Hatter's road schedules use it (`0702 0004 0004` at $03F920); no corpus run
 *           reaches it (raw-stubs 2026-10-30); 03DD1E.syn checks it
 */
void Boss_Hatter_RoadCmdRandom(uint32_t *d0, uint32_t *d1, uint32_t *d4, uint16_t *d5, uint32_t *a0, uint32_t a4,
                               uint32_t a6)
{
    *d0 = (uint32_t)Rand_Next();
    uint16_t count = (uint16_t)(int16_t)(int8_t)(uint8_t)*d1;          /* ext.w d1 */
    uint32_t product = (uint32_t)count * (uint16_t)*d0;                 /* mulu.w d0,d1 */
    product = (product << 16) | (product >> 16);                        /* swap d1 */
    uint16_t offset = (uint16_t)((uint16_t)product * 2);                /* add.w d1,d1 */
    *d1 = (product & 0xFFFF0000u) | offset;
    *a0 += (uint32_t)(int32_t)(int16_t)offset;
    PTR(const u16) entry;
    entry.v = *a0;
    *a0 += (uint32_t)(int32_t)(int16_t)*entry;
    dk::jump(at_Boss_Hatter_RoadCmdNext);
}
