#include "game.h"

/* the spawned object's a2 script (ROM data right after this spawner) */
static constexpr uint32_t Scr2_039F62 = 0x039F62u;

/* Enemy_GiftBox_SpawnStay1 - spawner of a gift box (weight $A) that stays put, opens into a single
 * toy figure (ToyChain_MakeBox1) and hops in place every $B40 ticks
 * In:  d4/d5 = spawn position (trigger/spawn entry)
 * Out: Obj_SpawnEnemy's result
 * Evidence: lea Scr2_039F62 / jmp Obj_SpawnEnemy; script `0A32 000A`, `244A 002A 0003A2E6`, `17B8
 *           0000`, timer $B40; entries Tbl_033F30+$1B0/+$1BE/+$1CC/+$1DA; TAS f96521-f96777 (4
 *           spawns, section $3200); a2 an output (measured as a clobber): Obj_SpawnEnemy, jumped
 *           to, takes the script in it
 */
void Enemy_GiftBox_SpawnStay1(uint16_t *d1, uint16_t x, uint16_t y, uint32_t *a0, uint32_t *script,
                              uint32_t *a4, uint32_t *a5, uint32_t *a6)
{
    *script = Scr2_039F62;
    dk::jump(at_Obj_SpawnEnemy);
}
