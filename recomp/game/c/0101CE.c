#include "game.h"

/* A player slot object while no player is on screen ($0101CE-$0102D5). */

static constexpr uint32_t Scr2_04ACD0 = 0x04ACD0u;   /* Batman's a2 script */
static constexpr uint32_t Scr2_04D80C = 0x04D80Cu;   /* Robin's */

/* the pad's newly pressed byte: bit 7 = Start */
static constexpr uint8_t PAD_START = 0x80;

/* Player_JoinOrSpawn - handler of a player slot object while no player is on screen. A player in
 * play (or one who just joined: Start newly pressed with a credit left and joining not locked;
 * the credit is taken, the HUD switched and the record reset) is spawned when the pool has 3
 * blocks: motion, VM and sprite blocks, the handler Player_Run, Batman's or Robin's script, at the
 * saved position (a respawn) or the camera plus the join offset with $78 frames of
 * invulnerability; then its first frame (Player_RunScript).
 * In:  self = the slot object (a6: +$1E record)
 * Out: a0 = the record, or the script on a spawn; a1 = the pad; a2 = the record; a3/a4 = the
 *      motion and VM blocks; d0/d1 = the spawn position; then Player_RunScript's
 * Evidence: btst #0,$25(a0) / tst.b g_JoinLocked / tst.w g_Credits / btst #7,$2(a1) / bset /
 *           subq.w #1,g_Credits / bsr HUD_PlayerJoined / bsr.s Player_InitRecord / move.w #3,d0 /
 *           bsr ObjPool_HasFree / move.l #$10322,(a6) / 3 x ObjPool_AllocClear / the handles,
 *           $3A(a5) = 1, $38(a5) = g_SceneSprAttr, the serial, $24(a4) = g_PlayMode, $58(a6) =
 *           +$3E, +$3C = the serial / the camera position, bclr #1,$25(a2) / $16(a5) = $78 / the
 *           script by +$25 bit 3 / move.l a0,$2A(a4) / bra.w Player_RunScript; every gameplay frame
 */
void Player_JoinOrSpawn(uint32_t *d0, uint32_t *d1, uint32_t d2, uint32_t d3, uint32_t d4, uint32_t d5, uint32_t d6,
                        uint32_t d7, uint32_t *a0, uint32_t *a1, uint32_t *a2, uint32_t *a3, uint32_t *a4,
                        Vm2Obj *self)
{
    (void)d2, (void)d3, (void)d4, (void)d5, (void)d6, (void)d7;
    PTR(PlayerRec) rec;
    rec.v = self->player;
    *a0 = rec.raw();
    if (!(rec->flags & PLAYER_IN_PLAY)) {
        if (g_JoinLocked || !g_Credits)
            return;
        const u8 *pad = (rec->pad == 1) ? g_Pad1 : g_Pad2;
        *a1 = dk::addr(pad);
        if (!(pad[2] & PAD_START))
            return;
        rec->flags = (uint8_t)(rec->flags | PLAYER_IN_PLAY);
        g_Credits -= 1;
        *a1 = HUD_PlayerJoined(rec);
        Player_InitRecord(rec.raw());
    }
    *d0 = (*d0 & 0xFFFF0000u) | 3;
    ObjPool_HasFree(d0);
    /* ObjPool_HasFree: d0 = 0 (moveq, Z) when the pool has the blocks, else -1 */
    if (*d0 != 0)
        return;
    self->handler = at_Player_Run;
    ObjPool_AllocClear(a3);
    ObjPool_AllocClear(a4);
    uint32_t a5 = 0;
    ObjPool_AllocClear(&a5);
    *a0 = a5;
    PTR(Vm2Block) vm;
    vm.v = *a4;
    PTR(Vm2Sprite) spr;
    spr.v = a5;
    self->motion = (int16_t)(uint16_t)*a3;
    self->vm = (int16_t)(uint16_t)*a4;
    self->sprite = (int16_t)(uint16_t)a5;
    spr->depth = 1;
    spr->attr = (uint16_t)g_SceneSprAttr;
    self->serial = (uint16_t)g_ObjSerial;
    g_ObjSerial += 1;
    vm->var[1] = (uint16_t)g_PlayMode;
    *a2 = rec.raw();
    self->owner_id = rec->pad;
    rec->serial = self->serial;
    uint32_t cx = g_CameraX, cy = g_CameraY;
    uint16_t x = (uint16_t)(cx >> 16), y = (uint16_t)(cy >> 16);
    bool respawn = (rec->flags & PLAYER_RESPAWN) != 0;
    rec->flags = (uint8_t)(rec->flags & ~PLAYER_RESPAWN);
    if (respawn) {
        x = (uint16_t)(x + (uint16_t)rec->x);
        y = (uint16_t)(y + (uint16_t)rec->y);
    } else {
        spr->invulnerable = 0x78;
        x = (uint16_t)(x + (uint16_t)g_JoinOfsX);
        y = (uint16_t)(y + (uint16_t)g_JoinOfsY);
    }
    *d0 = ((uint32_t)x << 16) | (cx & 0xFFFFu);
    *d1 = ((uint32_t)y << 16) | (cy & 0xFFFFu);
    self->x = (int16_t)x;
    self->x_frac = (uint16_t)cx;
    self->y = (int16_t)y;
    self->y_frac = (uint16_t)cy;
    *a0 = (rec->flags & PLAYER_ROBIN) ? Scr2_04D80C : Scr2_04ACD0;
    vm->resume.v = *a0;
    dk::jump(at_Player_RunScript);
}

/* Player_DespawnNative - a2 native of the player scripts after the lives run out: Player_Despawn,
 * then ends the script's frame (the stack pointer VM2_RunObject saved, and its return).
 * In:  self = the player object (a6: +$1E record)
 * Out: a2 = the record; Player_Despawn's; does not return to the script
 * Evidence: movea.l $1E(a6),a2 / bsr.w Player_Despawn / movea.l (g_VM2SavedSP).w,a7 / rts; dc.l after
 *           op $2CBC in the Batman ($4B1B8, $4D00A) and Robin ($4FB46, $4FDBC) scripts
 */
void Player_DespawnNative(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5,
                          uint32_t *d6, uint32_t *d7, uint32_t *a0, uint32_t *a1, uint32_t *a2, uint32_t *a3,
                          uint32_t *a4, uint32_t *a5, Vm2Obj *self)
{
    *a2 = self->player;
    Player_Despawn(d0, d1, d2, d3, d4, d5, d6, d7, a0, a1, a2, a3, a4, a5, self);
    dk::set_stack_pointer(g_VM2SavedSP);
}
