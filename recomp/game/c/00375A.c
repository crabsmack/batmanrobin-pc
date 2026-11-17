#include "game.h"

/* Op2_BranchTouchBox's two list cases with their shared exits, and the attack-box ops
 * ($00375A-$003797). */

/* a script pointer from a 68000 address held in a register */
static const u16 *script_at(uint32_t a)
{
    PTR(const u16) p;
    p.v = a;
    return p;
}

/* NEXT: the handler the script's next word names, jumped to */
static void next(uint32_t *handler, const u16 **script)
{
    *handler = dk::code((int16_t)(uint16_t)*(*script)++);
    dk::jump(*handler);
}

/* the box searches (Hitbox_FindPlayerBox / FindEnemyBox) return Z clear with a2 = the box
   found, or Z set with a2 = the list's tail sentinel; the sentinel is the one node whose next
   link is 0 (the search only tests nodes with tst.w (a2) non-zero), so the link says which */
static bool found_box(uint32_t a2)
{
    return dk::handle<HitBox>((int16_t)(uint16_t)a2)->next != 0;
}

/* Op2_BranchTouchBox_Players - Op2_BranchTouchBox's g_PlayerBoxList case (kinds 0 and 4):
 * searches the list for a box overlapping the rectangle, then the shared exits.
 * In:  left/top/right/bottom (d4-d7) = the world rectangle; a0 = the op's offset word
 * Out: d0 as the search leaves it; found: Op2_BranchTouchBox_Hit, else Op2_BranchTouchBox_Miss
 * Evidence: bsr.w Hitbox_FindPlayerBox / bne.s $376E; reached from the jump table at $374A/$374E;
 *           114 runs
 */
void Op2_BranchTouchBox_Players(uint32_t *d0, uint16_t left, uint16_t top, uint16_t right, uint16_t bottom,
                                uint32_t *a0, const u16 **script)
{
    uint32_t a2 = dk::addr(*script);
    Hitbox_FindPlayerBox(d0, left, top, right, bottom, &a2);
    if (found_box(a2)) {
        Op2_BranchTouchBox_Hit(d0, left, top, right, bottom, a0, &a2);
        *script = script_at(a2);
        return;
    }
    Op2_BranchTouchBox_Miss(d0, left, top, right, bottom, a0, script);
}

/* Op2_BranchTouchBox_Miss - the shared not-found exit: the script goes on after the offset word.
 * In:  a0 = the op's offset word
 * Out: script = a0 + 2; handler = the next op's; NEXT
 * Evidence: lea $2(a0),a2 / NEXT
 */
void Op2_BranchTouchBox_Miss(uint32_t *d0, uint16_t left, uint16_t top, uint16_t right, uint16_t bottom,
                             uint32_t *a0, const u16 **script)
{
    *script = script_at(*a0) + 1;
    next(a0, script);
}

/* Op2_BranchTouchBox_Enemies - Op2_BranchTouchBox's g_EnemyBoxList case (kind 8): searches the
 * enemy list, then the shared exits.
 * In:  as Op2_BranchTouchBox_Players
 * Out: as Op2_BranchTouchBox_Players
 * Evidence: bsr.w Hitbox_FindEnemyBox / beq.s $3760, falling into $376E; reached from $3752; 86 runs
 */
void Op2_BranchTouchBox_Enemies(uint32_t *d0, uint16_t left, uint16_t top, uint16_t right, uint16_t bottom,
                                uint32_t *a0, const u16 **script)
{
    uint32_t a2 = dk::addr(*script);
    Hitbox_FindEnemyBox(d0, left, top, right, bottom, &a2);
    if (!found_box(a2)) {
        Op2_BranchTouchBox_Miss(d0, left, top, right, bottom, a0, script);
        return;
    }
    Op2_BranchTouchBox_Hit(d0, left, top, right, bottom, a0, &a2);
    *script = script_at(a2);
}

/* Op2_BranchTouchBox_Hit - the shared found exit: remembers the box in g_FoundBox and takes the
 * branch (the offset word's address plus its value).
 * In:  a2 = the box found; a0 = the op's offset word
 * Out: g_FoundBox = the box; a2 = the branch target; handler = its first op's; NEXT
 * Evidence: move.l a2,(g_FoundBox).w / movea.l a0,a2 / adda.w (a2),a2 / NEXT
 */
void Op2_BranchTouchBox_Hit(uint32_t *d0, uint16_t left, uint16_t top, uint16_t right, uint16_t bottom,
                            uint32_t *a0, uint32_t *a2)
{
    g_FoundBox = *a2;
    const u16 *script = script_at(*a0);
    script = (const u16 *)((const u8 *)script + (int16_t)(uint16_t)*script);
    next(a0, &script);
    *a2 = dk::addr(script);
}

/* Obj_SetAttackBoxes (another file) on a typed script, vm and object */
static void attack_boxes(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5,
                         uint32_t *d6, uint16_t *count, uint32_t *a1, const u16 **script, Vm2Block *vm, Vm2Obj *self)
{
    uint32_t a2 = dk::addr(*script);
    dk::Flags ccr{};
    Obj_SetAttackBoxes(d0, d1, d2, d3, d4, d5, d6, count, a1, &a2, dk::addr(vm), dk::addr(self), &ccr);
    *script = script_at(a2);
}

/* Op2_SetAttackBoxes - a2 op: replaces the object's attack boxes with n boxes described inline
 * (Obj_SetAttackBoxes).
 * In:  script = word n, then n box specs
 * Out: as Obj_SetAttackBoxes; NEXT after the specs
 * Evidence: move.w (a2)+,d7 / bsr.w Obj_SetAttackBoxes / NEXT; 3 runs; $1C53E `377A 0002 ...`
 */
void Op2_SetAttackBoxes(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5,
                        uint32_t *d6, uint16_t *d7, uint32_t *a1, const u16 **script, Vm2Block *vm, Vm2Obj *self)
{
    *d7 = *(*script)++;
    attack_boxes(d0, d1, d2, d3, d4, d5, d6, d7, a1, script, vm, self);
    dk::jump(dk::code((int16_t)(uint16_t)*(*script)++));
}

/* Op2_SetAttackBox - a2 op: replaces the object's attack boxes with the one box described inline
 * (the usual punch / kick box with its hit callback).
 * In:  script = one box spec
 * Out: d7 = 1, then as Obj_SetAttackBoxes; NEXT after the spec
 * Evidence: moveq #1,d7 / bsr.w Obj_SetAttackBoxes / NEXT; 105 runs; $18106 `3784 0004 FFE2 FFEC
 *           001E 0032 0004 000105C6 0020 0300`
 */
void Op2_SetAttackBox(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5,
                      uint32_t *d6, uint32_t *d7, uint32_t *a1, const u16 **script, Vm2Block *vm, Vm2Obj *self)
{
    uint16_t count = 1;   /* moveq: the upper word 0, and the box loop only counts the low word */
    attack_boxes(d0, d1, d2, d3, d4, d5, d6, &count, a1, script, vm, self);
    *d7 = count;
    dk::jump(dk::code((int16_t)(uint16_t)*(*script)++));
}

/* Op2_ClearAttackBoxes - a2 op: removes all of the object's attack boxes (frees their blocks).
 * In:  -
 * Out: d7 = 0, then as Obj_SetAttackBoxes (its condition codes for the next op); NEXT
 * Evidence: moveq #0,d7 / bsr.w Obj_SetAttackBoxes / NEXT; 90 runs
 */
void Op2_ClearAttackBoxes(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5,
                          uint32_t *d6, uint32_t *d7, uint32_t *a0, uint32_t *a1, const u16 **script, Vm2Block *vm,
                          Vm2Obj *self)
{
    uint16_t count = 0;
    attack_boxes(d0, d1, d2, d3, d4, d5, d6, &count, a1, script, vm, self);
    *d7 = count;
    next(a0, script);
}
