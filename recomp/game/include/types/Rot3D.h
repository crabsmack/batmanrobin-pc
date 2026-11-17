/* types/Rot3D.h - a 3D record: three angles, a translation and the rotation matrix built from
 * them (docs/NOTES.md; Rot3D_BuildMatrix / Rot3D_ProjectPoint; e.g. the record at $F3C4). */
#pragma once
#include "dk_mem.h"

typedef struct Rot3D {
    u16 angle_a;           /* +$00: 0-$3FF, 1024 a turn (the word Rot3D_BuildMatrix reads) */
    u8  unk_02[0x02];
    u16 angle_b;           /* +$04 */
    u8  unk_06[0x02];
    u16 angle_c;           /* +$08 */
    u8  unk_0A[0x02];
    s32 tx;                /* +$0C: the translation added to every transformed point */
    s32 ty;                /* +$10 */
    s32 tz;                /* +$14 */
    s16 m[9];              /* +$18: the matrix (1.15), rows x' ($18/$1A/$1C), y' ($1E/$20/$22), z' ($24/$26/$28) */
} Rot3D;
static_assert(offsetof(Rot3D, angle_b) == 0x04, "Rot3D.angle_b");
static_assert(offsetof(Rot3D, angle_c) == 0x08, "Rot3D.angle_c");
static_assert(offsetof(Rot3D, tx) == 0x0C, "Rot3D.tx");
static_assert(offsetof(Rot3D, tz) == 0x14, "Rot3D.tz");
static_assert(offsetof(Rot3D, m) == 0x18, "Rot3D.m");
static_assert(sizeof(Rot3D) == 0x2A, "Rot3D");
