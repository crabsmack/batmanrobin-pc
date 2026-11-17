#include "game.h"

/* Op4_Spawn / Op4_SpawnAddVel and the motion-triple ops ($005606-$0056CF): spawning a child a4
 * object, and adding (value, velocity, acceleration) longs to one axis of the object or of the
 * last spawned child. */

/* NEXT: the handler the script's next word names, jumped to */
static void op_next(uint32_t *handler, const u16 **script)
{
    *handler = dk::code((int16_t)(uint16_t)*(*script)++);
    dk::jump(*handler);
}

/* the handler word at a script address (a register holding it), and the address past it */
static uint32_t fetch_at(uint32_t *a4)
{
    PTR(const u16) script;
    script.v = *a4;
    uint32_t handler = dk::code((int16_t)(uint16_t)*script++);
    *a4 = script.raw();
    return handler;
}

/* the next long operand */
static uint32_t read_long(const u16 **script)
{
    uint32_t v = (uint32_t)(uint16_t)(*script)[0] << 16 | (uint16_t)(*script)[1];
    *script += 2;
    return v;
}

/* `add.l src,dst`: the sum, and its condition codes (X = C) */
static uint32_t add_long(uint32_t dst, uint32_t src, dk::Flags *ccr)
{
    uint32_t sum = dst + src;
    bool carry = sum < dst;
    *ccr = dk::Flags{ carry, (sum >> 31) != 0, sum == 0, ((~(dst ^ src)) & (dst ^ sum) & 0x80000000u) != 0, carry };
    return sum;
}

/* Op4_Spawn - a4 op: spawns a child a4 object (VM4_SpawnChild: a pool block on the object list,
 * handler VM4_RunObject, owner and attributes inherited, radius/heading/x/y from d2-d5 plus an
 * optional parameter table) and continues with a5 = the child.
 * In:  a4 = script: word failure offset (0 = end the run), word child-script offset, word table
 *      offset (0 = start at d2-d5); d2-d5; a6 = the spawner
 * Out: a5 = the child; a4 past the operands; NEXT (a full pool: VM4_SpawnChild takes the failure
 *      branch or ends the run instead, never returning here)
 * Evidence: bsr VM4_SpawnChild / NEXT; the most used a4 op (229 a4-script uses, e.g. the shot
 *           scripts $108CE-$10964: `5606 0000 0006 0000 53F4`); 142 corpus runs
 */
void Op4_Spawn(uint32_t *d0, uint32_t *d1, uint32_t d2, uint32_t d3, uint32_t d4, uint32_t d5, uint32_t d6,
               uint32_t d7, uint32_t *a0, uint32_t *a1, uint32_t a2, uint32_t *a3, uint32_t *a4, uint32_t *a5,
               uint32_t a6)
{
    (void)d6, (void)d7, (void)a2;
    dk::Flags spawn_ccr{};   /* VM4_SpawnChild's condition codes (not read here) */
    VM4_SpawnChild(d0, d1, d2, d3, d4, d5, a0, a1, a3, a4, a5, a6, &spawn_ccr);
    *a0 = fetch_at(a4);
    dk::jump(*a0);
}

/* Op4_SpawnAddVel - a4 op: as Op4_Spawn, then adds this object's velocities (radius, heading, x,
 * y) to the child's, so the child carries the spawner's motion (on g_SpawnCtx: the parent's
 * velocity Fx_RunScript stored there).
 * In:  as Op4_Spawn
 * Out: as Op4_Spawn; d0 = this object's y velocity; the condition codes of the last add
 * Evidence: bsr VM4_SpawnChild / move.l $36(a6),d0 / add.l d0,$36(a5) ... $42; script uses $1A45E,
 *           $1A552, $1ABDC; 43 corpus runs
 */
void Op4_SpawnAddVel(uint32_t *d0, uint32_t *d1, uint32_t d2, uint32_t d3, uint32_t d4, uint32_t d5, uint32_t d6,
                     uint32_t d7, uint32_t *a0, uint32_t *a1, uint32_t a2, uint32_t *a3, uint32_t *a4, uint32_t *a5,
                     uint32_t a6, dk::Flags *ccr)
{
    (void)d6, (void)d7, (void)a2;
    dk::Flags spawn_ccr{};   /* VM4_SpawnChild's condition codes (not read here) */
    VM4_SpawnChild(d0, d1, d2, d3, d4, d5, a0, a1, a3, a4, a5, a6, &spawn_ccr);
    PTR(const Vm4Obj) self;
    self.v = a6;
    PTR(Vm4Obj) child;
    child.v = *a5;
    child->vel_radius = (int32_t)add_long((uint32_t)(int32_t)child->vel_radius, (uint32_t)(int32_t)self->vel_radius, ccr);
    child->vel_heading = (int32_t)add_long((uint32_t)(int32_t)child->vel_heading, (uint32_t)(int32_t)self->vel_heading, ccr);
    child->vel_x = (int32_t)add_long((uint32_t)(int32_t)child->vel_x, (uint32_t)(int32_t)self->vel_x, ccr);
    *d0 = (uint32_t)(int32_t)self->vel_y;
    child->vel_y = (int32_t)add_long((uint32_t)(int32_t)child->vel_y, *d0, ccr);
    *a0 = fetch_at(a4);
    dk::jump(*a0);
}

/* Op4_AddMotionX - a4 op: adds three longs to the object's x, x velocity and x acceleration.
 * In:  script = longs: value, velocity, acceleration (16.16)
 * Out: d0 = the acceleration long; self->x, vel_x, acc_x; NEXT
 * Evidence: three move.l (a4)+,d0 / add.l d0 to $2E/$3E/$4E(a6); the motion op $6286 integrates
 *           accel -> velocity -> position; script uses $46BD0, $46BF2; 2 corpus runs
 */
void Op4_AddMotionX(uint32_t *d0, const u16 **script, Vm4Obj *self)
{
    vm4_add_fix(self->x, self->x_frac, read_long(script));
    vm4_add_long(self->vel_x, read_long(script));
    *d0 = read_long(script);
    vm4_add_long(self->acc_x, *d0);
    uint32_t handler;
    op_next(&handler, script);
}

/* Op4_AddMotionY - a4 op: adds three longs to the object's y, y velocity and y acceleration.
 * In:  script = longs: value, velocity, acceleration (16.16)
 * Out: d0 = the acceleration long; self->y, vel_y, acc_y; NEXT
 * Evidence: three add.l (a4)+ to $32/$42/$52(a6); script use $46D00; 4 corpus runs
 */
void Op4_AddMotionY(uint32_t *d0, const u16 **script, Vm4Obj *self)
{
    vm4_add_fix(self->y, self->y_frac, read_long(script));
    vm4_add_long(self->vel_y, read_long(script));
    *d0 = read_long(script);
    vm4_add_long(self->acc_y, *d0);
    uint32_t handler;
    op_next(&handler, script);
}

/* Op4_AddMotionRadius - a4 op: adds three longs to the object's radius, its velocity and its
 * acceleration (the drawn point lies the radius along the heading).
 * In:  script = longs: value, velocity, acceleration (16.16)
 * Out: d0 = the acceleration long; self->radius, vel_radius, acc_radius; NEXT
 * Evidence: three add.l (a4)+ to $26/$36/$46(a6); script uses $14C14, $1AB5E, $26BA6; 151 corpus runs
 */
void Op4_AddMotionRadius(uint32_t *d0, const u16 **script, Vm4Obj *self)
{
    vm4_add_fix(self->radius, self->radius_frac, read_long(script));
    vm4_add_long(self->vel_radius, read_long(script));
    *d0 = read_long(script);
    vm4_add_long(self->acc_radius, *d0);
    uint32_t handler;
    op_next(&handler, script);
}

/* Op4_AddMotionHeading - a4 op: adds three longs to the object's heading, turn speed and turn
 * acceleration ($400 a turn in the integer word).
 * In:  script = longs: value, velocity, acceleration (16.16)
 * Out: d0 = the acceleration long; self->heading, vel_heading, acc_heading; NEXT
 * Evidence: three add.l (a4)+ to $2A/$3A/$4A(a6); script use $14C22; 1 corpus run (the TAS)
 */
void Op4_AddMotionHeading(uint32_t *d0, const u16 **script, Vm4Obj *self)
{
    vm4_add_fix(self->heading, self->heading_frac, read_long(script));
    vm4_add_long(self->vel_heading, read_long(script));
    *d0 = read_long(script);
    vm4_add_long(self->acc_heading, *d0);
    uint32_t handler;
    op_next(&handler, script);
}

/* Op4_ChildAddMotionX - a4 op: adds three longs to the last spawned child's x, x velocity and x
 * acceleration.
 * In:  script = longs: value, velocity, acceleration; child = a5 (the preceding spawn op's object)
 * Out: d0 = the acceleration long; the child's x, vel_x, acc_x; the last add's condition codes;
 *      NEXT
 * Evidence: three add.l (a4)+ to $2E/$3E/$4E(a5); 17 corpus runs
 */
void Op4_ChildAddMotionX(uint32_t *d0, uint32_t *handler, const u16 **script, Vm4Obj *child, dk::Flags *ccr)
{
    vm4_add_fix(child->x, child->x_frac, read_long(script));
    vm4_add_long(child->vel_x, read_long(script));
    *d0 = read_long(script);
    child->acc_x = (int32_t)add_long((uint32_t)(int32_t)child->acc_x, *d0, ccr);
    op_next(handler, script);
}

/* Op4_ChildAddMotionY - a4 op: adds three longs to the child's y, y velocity and y acceleration.
 * In:  script = longs: value, velocity, acceleration; child = a5
 * Out: d0 = the acceleration long; the child's y, vel_y, acc_y; the last add's condition codes;
 *      NEXT
 * Evidence: three add.l (a4)+ to $32/$42/$52(a5); script uses $1AA68, $1F288, $25AC8 (after a spawn
 *           op); 51 corpus runs
 */
void Op4_ChildAddMotionY(uint32_t *d0, uint32_t *handler, const u16 **script, Vm4Obj *child, dk::Flags *ccr)
{
    vm4_add_fix(child->y, child->y_frac, read_long(script));
    vm4_add_long(child->vel_y, read_long(script));
    *d0 = read_long(script);
    child->acc_y = (int32_t)add_long((uint32_t)(int32_t)child->acc_y, *d0, ccr);
    op_next(handler, script);
}

/* Op4_ChildAddMotionRadius - a4 op: adds three longs to the child's radius, radius velocity and
 * radius acceleration.
 * In:  script = longs: value, velocity, acceleration; child = a5
 * Out: d0 = the acceleration long; the child's radius, vel_radius, acc_radius; the last add's
 *      condition codes; NEXT
 * Evidence: three add.l (a4)+ to $26/$36/$46(a5); script uses $1AB34 (after Op4_SpawnAddVel),
 *           $1E560; 49 corpus runs
 */
void Op4_ChildAddMotionRadius(uint32_t *d0, uint32_t *handler, const u16 **script, Vm4Obj *child, dk::Flags *ccr)
{
    vm4_add_fix(child->radius, child->radius_frac, read_long(script));
    vm4_add_long(child->vel_radius, read_long(script));
    *d0 = read_long(script);
    child->acc_radius = (int32_t)add_long((uint32_t)(int32_t)child->acc_radius, *d0, ccr);
    op_next(handler, script);
}
