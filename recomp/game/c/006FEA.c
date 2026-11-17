#include "game.h"

/* VM5_SpawnEntry - spawns one enemy from an a5 wave spawn entry: the position from the entry
 * (screen-relative or world) plus the wave's origin, then the entry's spawner; on success counts
 * the spawn and gives the new object its spawn action (action 1 also joins the wave's open kill
 * group, which closes when its member target is reached).
 * In:  a5 = script at a word offset to the entry (self-relative: word x, word y ($E000-range =
 *      screen-relative: + $2000 and the camera; else world: - $2000), long = spawn action byte +
 *      spawner routine); a6 = the wave; the other registers go to the spawner
 * Out: a5 past the offset word; d4 / d5 = the position; a0 = the new object (the spawner's a6),
 *      a1 past the entry (or the group block); d0 = 0 when spawned, or (action 0, or the spawner
 *      failed: its Z clear) d0 as before the spawner, its low byte the action. a6 kept
 * Evidence: movea.l a5,a1 / adda.w (a5)+,a1 / move.w (a1)+,d4 / move.w (a1)+,d5 / bmi / the camera /
 *           add.w $26(a6),d4 / add.w $28(a6),d5 / move.b (a1),d0 / movea.l (a1)+,a0 / movem.l
 *           d0/a5-a6,-(a7) / jsr (a0) / movea.l a6,a0 / movem.l (a7)+,d0/a5-a6 / bne (the
 *           spawner's Z: movem leaves the condition codes) / addq.w #1,$C(a6) / tst.b d0 / beq /
 *           cmpi.b #1 / the group / move.b d0,$19(a0) / moveq #0,d0; bsr from Op5_Spawn,
 *           SpawnMulti, SpawnRandom; entry $16994 `0154 E018 0001 B6FA`; 119 corpus runs
 */
void VM5_SpawnEntry(uint32_t *d0, uint32_t *d1, uint32_t d2, uint32_t d3, uint32_t *d4, uint32_t *d5,
                    uint32_t *d6, uint32_t *d7, uint32_t *a0, uint32_t *a1, uint32_t *a2, uint32_t *a3, uint32_t *a4,
                    uint32_t *a5, uint32_t a6)
{
    PTR(Vm5Wave) wave;
    wave.v = a6;
    PTR(const u16) offset;
    offset.v = *a5;
    *a1 = *a5 + (uint32_t)(int32_t)(int16_t)(uint16_t)*offset;
    *a5 += 2;
    PTR(const u16) entry;
    entry.v = *a1;
    uint16_t x = *entry++;
    uint16_t y = *entry++;
    bool on_screen = (int16_t)y < 0;
    if (on_screen) {   /* screen-relative */
        y = (uint16_t)(y + 0x2000);
        x = (uint16_t)(x + (uint16_t)(g_CameraX >> 16));
        y = (uint16_t)(y + (uint16_t)(g_CameraY >> 16));
    } else {
        y = (uint16_t)(y - 0x2000);
    }
    x = (uint16_t)(x + wave->origin_x);
    y = (uint16_t)(y + wave->origin_y);
    /* native: a screen-placed entry is placed for the wide view's edge (the gameplay mode, R20):
       its screen x (the entry's plus the origin) moved out as ws_place_x moves it; 0 otherwise.
       Screen-placed: a screen-relative entry, or a world entry while the camera is locked at x 0
       (the flights $2300 / $2400 and the fixed arenas, where the map or nothing scrolls: world x
       is screen x there, and the flights' waves place their biplanes and drones at an origin
       x $154 = 340, Op5_SetOrigin) */
    bool fixed_screen = !on_screen && (g_CameraX >> 16) == 0 && g_CameraLockX != 0;
    uint16_t sx = (uint16_t)(x - (uint16_t)(g_CameraX >> 16));
    int16_t moved = (on_screen || fixed_screen) ? (int16_t)(uint16_t)(ws_place_x(sx) - sx) : 0;
    x = (uint16_t)(x + (uint16_t)moved);
    *d4 = (*d4 & 0xFFFF0000u) | x;
    *d5 = (*d5 & 0xFFFF0000u) | y;
    const u8 *action_spawner = (const u8 *)(const u16 *)entry;
    *d0 = (*d0 & 0xFFFFFF00u) | action_spawner[0];
    PTR(const u32) spawner;
    spawner.v = entry.raw();
    *a0 = *spawner;
    *a1 = entry.raw() + 4;
    uint32_t saved_d0 = *d0, saved_a5 = *a5;   /* movem.l d0/a5-a6 around the spawner */
    uint32_t obj = a6;
    dk::Flags spawned{};
    uint32_t spawner_fn = *a0;
    Vm5SpawnerFn(dk::code(*a0), d0, d1, d2, d3, d4, d5, d6, d7, a0, a1, a2, a3, a4, a5, &obj, &spawned);
    *a0 = obj;
    *d0 = saved_d0;
    *a5 = saved_a5;
    if (!spawned.z)
        return;
    WS_NOTE_PLACE(on_screen ? WS_PLACE_WAVE : fixed_screen ? WS_PLACE_WAVE_FIXED : WS_PLACE_WAVE_WORLD, (int16_t)sx,
                  moved, obj, spawner_fn);
    (void)spawner_fn, (void)sx, (void)fixed_screen;
    wave->spawned = (uint16_t)(wave->spawned + 1);
    uint8_t action = (uint8_t)*d0;
    if (action == 0)
        return;   /* tst.b d0 / beq to the rts: d0 stays the entry's */
    PTR(Vm2Obj) enemy;
    enemy.v = *a0;
    if (action == 1) {
        *d1 = (*d1 & 0xFFFF0000u) | (uint16_t)wave->group;
        if ((uint16_t)*d1 == 0) {   /* no open group: the action is not stored (the original's) */
            *d0 = 0;
            return;
        }
        enemy->spawn_arg = (uint16_t)*d1;
        *a1 = (uint32_t)(int32_t)(int16_t)*d1;
        PTR(Vm5Group) group;
        group.v = *a1;
        group->spawned = (uint16_t)(group->spawned + 1);
        *d1 = (*d1 & 0xFFFF0000u) | group->members;
        if ((int16_t)group->members <= (int16_t)group->spawned)
            wave->group = 0;   /* every member spawned: later spawns start no new one */
    }
    enemy->spawn_action = action;
    *d0 = 0;
}

/* VM5_AfterSpawn - the a5 spawn ops' tail: reloads the wait timer from the interval, switches the
 * script to the after-quota pc once the spawn count reaches the quota, and yields the frame.
 * In:  a6 = the wave
 * Out: timer = interval; when the quota is met pc = after_quota and the timer's high word cleared
 *      (clr.w: the low word stays the interval's, the original's); d0 = the quota; the stack
 *      pointer VM5_RunObject saved, then its caller's return; the condition codes of the cmp.w
 *      (or the clr.w)
 * Evidence: move.l $2A(a6),$2E(a6) / move.w $E(a6),d0 / btst #0 g_P1Flags / g_P2Flags / move.w
 *           $10(a6),d0 / cmp.w $C(a6),d0 / bgt / move.l $16(a6),$1E(a6) / clr.w $2E(a6) / movea.l
 *           (g_VM5SavedSP).w,a7 / rts; bra from Op5_Spawn / SpawnMulti / SpawnRandom
 */
uint16_t VM5_AfterSpawn(Vm5Wave *wave, dk::Flags *ccr)
{
    wave->timer = wave->interval;
    uint16_t quota = wave->quota_1p;
    if ((g_P1Flags & 1) && (g_P2Flags & 1))
        quota = wave->quota_2p;
    *ccr = dk::Flags::compare(quota, wave->spawned, 2, ccr->x);
    if ((int16_t)quota <= (int16_t)wave->spawned) {
        wave->pc = wave->after_quota;
        wave->timer = (int32_t)((uint32_t)wave->timer & 0xFFFFu);
        *ccr = dk::Flags{ ccr->x, false, true, false, false };
    }
    dk::set_stack_pointer(g_VM5SavedSP);
    return quota;
}
