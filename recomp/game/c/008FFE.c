#include "game.h"

/* a 68000 address held in a register, as a pointer (PTR(T): the 68000 value kept) */
template <class T> static T *ptr(uint32_t a)
{
    PTR(T) p;
    p.v = a;
    return p;
}

enum {
    WIPE_PAUSE_FRAMES = 0xF,   /* g_ScreenClearTimer: the spawn walker and on-screen tests wait */
    WIPE_DAMAGE = 0x200,       /* more than any enemy's energy */
    WIPE_ATTACKER = 0,         /* struck_by: no attacker */
};

/* Hitbox_WipeEnemies - native callback (a screen clear): every enemy box takes $200 damage, and
 * spawning pauses for 15 frames.
 * In:  none
 * Out: g_ScreenClearTimer = $F; d0.w = $200, d1.w = 0; then Hitbox_DamageAllEnemies
 * Evidence: move.w #$F,g_ScreenClearTimer / move.w #$200,d0 / move.w #0,d1 /
 *           bra.w Hitbox_DamageAllEnemies; script uses after Op2_CallNative at $199F4, $1D854,
 *           $29344, $3D1C8, $462A4
 */
void Hitbox_WipeEnemies(uint16_t *d0, uint16_t *d1)
{
    g_ScreenClearTimer = WIPE_PAUSE_FRAMES;
    *d0 = WIPE_DAMAGE;
    *d1 = WIPE_ATTACKER;
    dk::jump(at_Hitbox_DamageAllEnemies);
}

/* Fx_SpawnAtOfsAngle - native callback: runs an a4 spawn script at the object's position plus an
 * offset (not mirrored), the new objects heading the script's way.
 * In:  a6 = the object; a2 = the script: dc.w heading, dx, dy, dc.l the a4 script
 * Out: d2 = 0 (radius), d3 = heading << 16, d4/d5 = (x + dx, y + dy) << 16; a2 at the dc.l; then
 *      Fx_RunScriptArg
 * Evidence: moveq #0,d3 / move.w (a2)+,d3 / swap d3 / moveq #0,d2 / moveq #0,d4 / moveq #0,d5 /
 *           move.w $3E(a6),d4 / move.w $42(a6),d5 / add.w (a2)+,d4 / add.w (a2)+,d5 / swap d4 /
 *           swap d5 / bra.w Fx_RunScriptArg; 114 script uses after Op2_CallNative ($29CBC...)
 */
void Fx_SpawnAtOfsAngle(uint32_t d0, uint32_t d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t d6,
                        uint32_t d7, uint32_t a1, uint32_t *a2, uint32_t a3, uint32_t a4, uint32_t a5, uint32_t a6)
{
    (void)d0, (void)d1, (void)d6, (void)d7, (void)a1, (void)a3, (void)a4, (void)a5;
    const Vm2Obj *self = ptr<const Vm2Obj>(a6);
    const u16 *args = ptr<const u16>(*a2);
    *d3 = (uint32_t)(uint16_t)args[0] << 16;
    *d2 = 0;
    uint16_t x = (uint16_t)((uint16_t)self->x + (uint16_t)args[1]);
    uint16_t y = (uint16_t)((uint16_t)self->y + (uint16_t)args[2]);
    *d4 = (uint32_t)x << 16;
    *d5 = (uint32_t)y << 16;
    *a2 += 6;
    dk::jump(at_Fx_RunScriptArg);
}
