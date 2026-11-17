#include "game.h"

/* The child add family ($0058EE-$005959): a signed word added to the last spawned child's heading,
 * and 8.8 words added to its velocities and accelerations. a5 is the child (the object itself
 * until a spawn op ran). */

/* the next handler word (NEXT) */
static uint32_t next_op(const u16 **script)
{
    return dk::code((int16_t)(uint16_t)*(*script)++);
}

/* an 8.8 operand word as 16.16: ext.l / asl.l #8 */
static uint32_t word_8_8(const u16 **script)
{
    return (uint32_t)(int32_t)(int16_t)(uint16_t)*(*script)++ << 8;
}

/* `add.l src,dst` on a long field: the sum stored, and its condition codes (X = C) */
static void add_long(s32 &dst, uint32_t src, dk::Flags *ccr)
{
    uint32_t old = (uint32_t)(int32_t)dst;
    uint32_t sum = old + src;
    bool carry = sum < old;
    *ccr = dk::Flags{ carry, (sum >> 31) != 0, sum == 0, ((~(old ^ src)) & (old ^ sum) & 0x80000000u) != 0, carry };
    dst = (int32_t)sum;
}

/* Op4_ChildAddHeading - a4 op: adds a signed word to the integer word of the child's heading
 * ($400 a turn).
 * In:  script = word amount; child = a5
 * Out: d0 = it; child->heading; the add.w's condition codes; NEXT (handler = the next op)
 * Evidence: move.w (a4)+,d0 / add.w d0,$2A(a5) / NEXT; 84 a4-script uses, e.g. $37034-$37050 (one
 *           spawn, then turn it); 118 corpus runs
 */
void Op4_ChildAddHeading(uint16_t *d0, uint32_t *handler, const u16 **script, Vm4Obj *child, dk::Flags *ccr)
{
    *d0 = *(*script)++;
    uint16_t old = (uint16_t)child->heading;
    uint16_t sum = (uint16_t)(old + *d0);
    bool carry = old + *d0 > 0xFFFF;
    *ccr = dk::Flags{ carry, (sum & 0x8000) != 0, sum == 0, ((~(old ^ *d0)) & (old ^ sum) & 0x8000) != 0, carry };
    child->heading = (int16_t)sum;
    *handler = next_op(script);
    dk::jump(*handler);
}

/* Op4_ChildAddVelX - a4 op: adds a signed 8.8 word (px/frame) to the child's x velocity.
 * In:  script = word 8.8; child = a5
 * Out: d0 = it as 16.16; child->vel_x; NEXT
 * Evidence: ext.l / asl.l #8 / add.l d0,$3E(a5); script uses $26C66, $26C7A; 23 corpus runs
 */
void Op4_ChildAddVelX(uint32_t *d0, const u16 **script, Vm4Obj *child)
{
    *d0 = word_8_8(script);
    vm4_add_long(child->vel_x, *d0);
    dk::jump(next_op(script));
}

/* Op4_ChildAddVelY - a4 op: adds a signed 8.8 word to the child's y velocity.
 * In:  script = word 8.8; child = a5
 * Out: d0 = it as 16.16; child->vel_y; the add.l's condition codes; NEXT (handler = the next op)
 * Evidence: ext.l / asl.l #8 / add.l d0,$42(a5); script uses $1A4B0, $1E228 (after Op4_Spawn); 3 runs
 */
void Op4_ChildAddVelY(uint32_t *d0, uint32_t *handler, const u16 **script, Vm4Obj *child, dk::Flags *ccr)
{
    *d0 = word_8_8(script);
    add_long(child->vel_y, *d0, ccr);
    *handler = next_op(script);
    dk::jump(*handler);
}

/* Op4_ChildAddVelRadius - a4 op: adds a signed 8.8 word to the child's radius velocity, so it
 * flies out along its heading.
 * In:  script = word 8.8; child = a5
 * Out: d0 = it as 16.16; child->vel_radius; NEXT
 * Evidence: ext.l / asl.l #8 / add.l d0,$36(a5); script uses $25B76, $25B94, $25BB2; 30 corpus runs
 */
void Op4_ChildAddVelRadius(uint32_t *d0, const u16 **script, Vm4Obj *child)
{
    *d0 = word_8_8(script);
    vm4_add_long(child->vel_radius, *d0);
    dk::jump(next_op(script));
}

/* Op4_ChildAddVelHeading - a4 op: adds a signed 8.8 word to the child's turn speed (a spin around
 * the centre).
 * In:  script = word 8.8; child = a5
 * Out: d0 = it as 16.16; child->vel_heading; NEXT
 * Evidence: ext.l / asl.l #8 / add.l d0,$3A(a5); 40 a4-script uses, e.g. $5036E-$50390 in the weapon
 *           shot scripts; 117 corpus runs
 */
void Op4_ChildAddVelHeading(uint32_t *d0, const u16 **script, Vm4Obj *child)
{
    *d0 = word_8_8(script);
    vm4_add_long(child->vel_heading, *d0);
    dk::jump(next_op(script));
}

/* Op4_ChildAddAccelX - a4 op: adds a signed 8.8 word to the child's x acceleration.
 * In:  script = word 8.8; child = a5
 * Out: d0 = it as 16.16; child->acc_x; NEXT
 * Evidence: ext.l / asl.l #8 / add.l d0,$4E(a5); 20 corpus runs
 */
void Op4_ChildAddAccelX(uint32_t *d0, const u16 **script, Vm4Obj *child)
{
    *d0 = word_8_8(script);
    vm4_add_long(child->acc_x, *d0);
    dk::jump(next_op(script));
}

/* Op4_ChildAddAccelY - a4 op: adds a signed 8.8 word to the child's y acceleration.
 * In:  script = word 8.8; child = a5
 * Out: d0 = it as 16.16; child->acc_y; the add.l's condition codes; NEXT (handler = the next op)
 * Evidence: ext.l / asl.l #8 / add.l d0,$52(a5); script uses $2057E, $20592, $3B74E; 15 corpus runs
 */
void Op4_ChildAddAccelY(uint32_t *d0, uint32_t *handler, const u16 **script, Vm4Obj *child, dk::Flags *ccr)
{
    *d0 = word_8_8(script);
    add_long(child->acc_y, *d0, ccr);
    *handler = next_op(script);
    dk::jump(*handler);
}

/* Op4_ChildAddAccelRadius - a4 op: adds a signed 8.8 word to the child's radius acceleration.
 * In:  script = word 8.8; child = a5
 * Out: d0 = it as 16.16; child->acc_radius; the add.l's condition codes; NEXT (handler = the next op)
 * Evidence: ext.l / asl.l #8 / add.l d0,$46(a5); script use $1E488; 6 corpus runs
 */
void Op4_ChildAddAccelRadius(uint32_t *d0, uint32_t *handler, const u16 **script, Vm4Obj *child, dk::Flags *ccr)
{
    *d0 = word_8_8(script);
    add_long(child->acc_radius, *d0, ccr);
    *handler = next_op(script);
    dk::jump(*handler);
}
