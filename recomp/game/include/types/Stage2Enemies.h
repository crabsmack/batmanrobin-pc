/* types/Stage2Enemies.h - records of stage 2's enemies and bosses ($0265DC-$02AB48: the rope
 * ladder thugs of the lift, the flight section's turret pod and beam ship; docs/NOTES.md "Stage 2
 * lift and boss arena" and the flight section). Views of the fields this code uses; the rest
 * stay unk_XX. */
#pragma once
#include "dk_mem.h"

/* the rope ladder's swing block: a pool block whose handle is +$10 of the ladder thug (and of
 * Boss_Stage2_SpawnEndLadder's wreck ladder); Enemy_LadderThug_SwingLadder steps it */
typedef struct LadderSwing {
    s32 angle;                     /* +$00: swing angle, 16.16 (starts at 6.0) */
    s32 speed;                     /* +$04: angular speed */
    u8  unk_08[0x52];
} LadderSwing;
static_assert(sizeof(LadderSwing) == 0x5A, "LadderSwing: one pool block");
static_assert(offsetof(LadderSwing, speed) == 0x04, "LadderSwing.speed");

/* the ladder thug as its own natives see it (a Vm2Obj; +$10 is its extra block) */
typedef struct LadderThugObj {
    u8  unk_00[0x10];
    s16 ladder;                    /* +$10: handle of its LadderSwing block */
} LadderThugObj;
static_assert(offsetof(LadderThugObj, ladder) == 0x10, "LadderThugObj.ladder");

/* the turret pod mini-boss (flight section, Boss_TurretPod_Spawn $02EA1E): a Vm2Obj whose
 * $1E/$22/$26 hold its three turrets (Boss_TurretPod_SpawnTurret1-3 fill them, 0 = none) */
typedef struct TurretPodObj {
    u8  unk_00[0x1E];
    s16 turret1;                   /* +$1E: turret 1's object handle (var 0 bit 1) */
    u8  unk_20[0x02];
    s16 turret2;                   /* +$22: turret 2 (bit 2) */
    u8  unk_24[0x02];
    s16 turret3;                   /* +$26: turret 3 (bit 4) */
} TurretPodObj;
static_assert(offsetof(TurretPodObj, turret1) == 0x1E, "TurretPodObj.turret1");
static_assert(offsetof(TurretPodObj, turret2) == 0x22, "TurretPodObj.turret2");
static_assert(offsetof(TurretPodObj, turret3) == 0x26, "TurretPodObj.turret3");

/* the flight section's beam ship (Boss_BeamShip_Spawn, script $2DF94) and its radius object (an
 * unlinked Vm2Obj, script Scr2_029598, whose x eases to the beam disc's radius): each holds the
 * other. The ship's beam spot lives in its saved position (Vm2Obj saved_x/saved_y, +$46/+$4A). */
typedef struct BeamShipObj {
    u8  unk_00[0x22];
    PTR(Vm2Obj) radius;            /* +$22: its radius object */
} BeamShipObj;
static_assert(offsetof(BeamShipObj, radius) == 0x22, "BeamShipObj.radius");

typedef struct BeamRadiusObj {
    u8  unk_00[0x1E];
    PTR(Vm2Obj) ship;              /* +$1E: the ship it belongs to */
} BeamRadiusObj;
static_assert(offsetof(BeamRadiusObj, ship) == 0x1E, "BeamRadiusObj.ship");
