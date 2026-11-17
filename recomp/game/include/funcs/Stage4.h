/* funcs/Stage4.h - prototypes of the Stage4 module: one line per function, its register
 * convention in the tag (dkit genesis recomp bridge). First written from the measured
 * contracts (dkit disasm regs); edited by the C's authors from then on. */
#pragma once
uint32_t Stage4_TreadFrameByX(const Vm2Obj *obj);	/* $042E14 regs(a6)->d0 */
void Stage4_InitConeRaster(void);	/* $0471C0 regs() clobbers(d7,a0,a1) */
void Stage4_RunHall(uint32_t *d1, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *a1, uint32_t *a2, uint32_t *a3, uint32_t *a4, uint32_t *a5, uint32_t *a6);	/* $0475C2 regs(&d1,&d3,&d4,&d5,&d6,&a1,&a2,&a3,&a4,&a5,&a6) clobbers(d0,d2,d7,a0) */
void Stage4_HallFloorRipple(uint32_t *d1, uint32_t *d2, uint16_t *d3, uint16_t *d4, uint16_t *d5);	/* $047674 regs(&d1,&d2,&d3,&d4,&d5) clobbers(d0,d7,a0,a1) */
void Stage4_RunLab(uint32_t *d1, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *a1, uint32_t *a2, uint32_t *a3, uint32_t *a4, uint32_t *a5, uint32_t *a6);	/* $047772 regs(&d1,&d3,&d4,&d5,&d6,&a1,&a2,&a3,&a4,&a5,&a6) clobbers(d0,d2,d7,a0) */
void Stage4_LabScroll(uint32_t *d1, uint32_t *d2, uint32_t *d3, uint16_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *a2, uint32_t *a3);	/* $0477D4 regs(&d1,&d2,&d3,&d4,&d5,&d6,&a2,&a3) clobbers(d0,d7,a0,a1) */
uint32_t Stage4_LabFloorLines(void);	/* $047842 regs()->d0 clobbers(d1,d7,a2) */
void Stage4_LabUpperLines(uint32_t *d3, uint16_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *a2, uint32_t *a3);	/* $04788C regs(&d3,&d4,&d5,&d6,&a2,&a3) clobbers(d0,d1,d2,d7) */
void Stage4_RunTube(uint32_t *d1, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *a1, uint32_t *a2, uint32_t *a3, uint32_t *a4, uint32_t *a5, uint32_t *a6);	/* $0479EE regs(&d1,&d3,&d4,&d5,&d6,&a1,&a2,&a3,&a4,&a5,&a6) clobbers(d0,d2,d7,a0) */
void Stage4_TubeScroll(uint32_t *d1, uint32_t *d2, uint32_t *d3, uint16_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *a2, uint32_t *a3, uint32_t *a4, uint32_t *a5);	/* $047A72 regs(&d1,&d2,&d3,&d4,&d5,&d6,&a2,&a3,&a4,&a5) clobbers(d0,d7) */
void Stage4_RunFreezeFight(uint32_t *d1, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *a1, uint32_t *a2, uint32_t *a3, uint32_t *a4, uint32_t *a5, uint32_t *a6);	/* $047BD0 regs(&d1,&d3,&d4,&d5,&d6,&a1,&a2,&a3,&a4,&a5,&a6) clobbers(d0,d2,d7,a0) */
void Stage4_DrawCones(uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *a2, uint32_t *a3, uint32_t *a4, uint32_t *a6);	/* $0471CC regs(&d1,&d2,&d3,&d4,&d5,&d6,&a2,&a3,&a4,&a6) clobbers(d0,d7,a0,a1) */
void Stage4_ConeAlloc(uint32_t *d0, uint32_t *d7, S4Obj *self);	/* $047336 regs(&d0,&d7,a6) clobbers(a0) */
void Stage4_ConeUpdate(uint32_t *d0, uint16_t *d1, const Vm2Motion *motion, S4Obj *self);	/* $0473BA regs(&d0,&d1,a3,a6) clobbers(a0) */
void Stage4_ConeSetInline(uint32_t *d0, const u8 **args, S4Obj *self);	/* $0474C0 regs(&d0,&a2,a6) clobbers(a0) */
uint16_t Stage4_ConeFree(S4Obj *self);	/* $0474F8 regs(a6)->d0 clobbers(a0) */
void Stage4_LabPlaneBLines(uint32_t *d1, uint32_t *d2);	/* $0478F6 regs(&d1,&d2) clobbers(d0,d7,a0,a1) */
