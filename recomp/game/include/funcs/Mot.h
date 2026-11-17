/* funcs/Mot.h - prototypes of the Mot module: one line per function, its register
 * convention in the tag (dkit genesis recomp bridge). First written from the measured
 * contracts (dkit disasm regs); edited by the C's authors from then on. */
#pragma once
void Mot_SetTarget(uint16_t *x, uint16_t *y, uint16_t *ady, Vm2Motion *mot, const Vm2Obj *self);	/* $0013CA regs(&d0,&d1,&d3,a3,a6) clobbers(d2) */
void Mot_TurnTowardHeading(uint32_t *target, uint32_t *heading, uint32_t *diff, Vm2Motion *mot);	/* $00156A regs(&d0,&d1,&d2,a3) */
void Mot_SeekScreenX(uint16_t *diff, uint16_t tolerance, uint32_t *handler, const u16 **script, Vm2Motion *mot, const Vm2Obj *self);	/* $001922 regs(&d0,d1,&a0,&a2,a3,a6) ccout(NVC) */
void Mot_SeekScreenXOrBranch(uint16_t *diff, uint16_t tolerance, uint32_t *handler, const u16 **script, Vm2Motion *mot, const Vm2Obj *self);	/* $00194E regs(&d0,d1,&a0,&a2,a3,a6) ccout(NVC) */
void Mot_SeekWorldXOrBranch(uint16_t *diff, uint16_t tolerance, uint32_t *handler, const u16 **script, Vm2Motion *mot, const Vm2Obj *self);	/* $0019F2 regs(&d0,d1,&a0,&a2,a3,a6) ccout(NVC) */
void Mot_SeekScreenY(uint16_t *diff, uint16_t tolerance, uint32_t *handler, const u16 **script, Vm2Motion *mot, const Vm2Obj *self);	/* $001A62 regs(&d0,d1,&a0,&a2,a3,a6) ccout(NVC) */
void Mot_SeekScreenYOrBranch(uint16_t *diff, uint16_t tolerance, uint32_t *handler, const u16 **script, Vm2Motion *mot, const Vm2Obj *self);	/* $001A8E regs(&d0,d1,&a0,&a2,a3,a6) ccout(NVC) */
void Mot_SeekWorldYOrBranch(uint16_t *diff, uint16_t tolerance, uint32_t *handler, const u16 **script, Vm2Motion *mot, const Vm2Obj *self);	/* $001B32 regs(&d0,d1,&a0,&a2,a3,a6) ccout(NVC) */
void Mot_AccelForDistance(uint16_t *speed, uint32_t *accel, uint32_t *series, uint16_t *time, const u16 **script, const Vm2Block *vm);	/* $001B98 regs(&d0,&d1,&d2,&d3,&a2,a4) */
void Mot_JumpGravity(uint32_t *d0, uint32_t *height, uint32_t *d2, uint16_t *d3, const u16 **script, const Vm2Block *vm);	/* $001D7E regs(&d0,&d1,&d2,&d3,&a2,a4) */
void Mot_JumpGravityD1(uint32_t *tick, uint32_t *gravity, uint32_t *span, uint16_t *time, const u16 **script, const Vm2Block *vm);	/* $001D82 regs(&d0,&d1,&d2,&d3,&a2,a4) */
void Mot_JumpLaunchSpeed(uint32_t *runs, uint32_t *speed, const u16 *script, const Vm2Block *vm);	/* $001DBC regs(&d0,&d1,a2,a4) */
void Mot_StorePlayerWindow(uint16_t *x_lo, uint16_t *x_hi, uint16_t *y_lo, uint16_t *y_hi, const u16 **script, Vm2Motion *mot);	/* $0034A6 regs(&d0,&d1,&d2,&d3,&a2,a3) clobbers(a0) */
void Mot_Atan2(uint32_t *d0, uint32_t *d1, uint32_t *d2);	/* $014EC6 regs(&d0,&d1,&d2) */
void Mot_Atan2_RightHalf(uint32_t *d0, uint32_t *d1, uint32_t *d2);	/* $014EE4 regs(&d0,&d1,&d2) */
void Mot_DirToPlayer(uint32_t *d0, uint32_t *d1, uint16_t *d2, uint16_t y, uint16_t n, const PlayerRec *player);	/* $014F78 regs(&d0,&d1,&d2,d3,d4,a0) */
void Mot_SteerToHeading(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *a0, uint32_t *a2, Vm2Motion *mot);	/* $0015BE regs(&d0,&d1,&d2,&d3,&d4,&d5,&d6,&a0,&a2,a3) ccout(NVC) */
void Mot_StepDirToward(uint16_t *dir, uint16_t n, uint16_t *target, uint16_t *half);	/* $01501C regs(&d0,d1,&d2,&d3) */
uint32_t Mot_Atan2_Zero(void);	/* $014ED2 regs()->d2 */
void Mot_Atan2_LeftHalf(uint32_t *d0, uint32_t *d1, uint16_t *d2);	/* $014F28 regs(&d0,&d1,&d2) */
uint16_t Mot_Atan2_Down(void);	/* $014ED6 regs()->d2 */
uint16_t Mot_Atan2_Up(void);	/* $014EDC regs()->d2 */
