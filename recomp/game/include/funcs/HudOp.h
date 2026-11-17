/* funcs/HudOp.h - prototypes of the HudOp module: one line per function, its register
 * convention in the tag (dkit genesis recomp bridge). First written from the measured
 * contracts (dkit disasm regs); edited by the C's authors from then on. */
#pragma once
void HudOp_Halt(uint32_t a0, PlayerRec *rec);	/* $011788 regs(a0,a6) clobbers(a0) */
void HudOp_Wait(uint32_t a0, PlayerRec *rec);	/* $011796 regs(a0,a6) clobbers(a0) */
void HudOp_Goto(uint32_t *a0, uint32_t *a1);	/* $0117A0 regs(&a0,&a1) clobbers(d0) */
void HudOp_SlideOut(uint32_t *a0, uint32_t *a1, PlayerRec *rec);	/* $0117A8 regs(&a0,&a1,a6) clobbers(d0,d1) */
void HudOp_SlideIn(uint32_t *d0, uint32_t *d4, uint32_t *d5, uint32_t *a0, uint32_t *a1, PlayerRec *rec);	/* $0117AE regs(&d0,&d4,&d5,&a0,&a1,a6) */
void HudOp_WaitSlide(uint32_t *a0, uint32_t *a1, PlayerRec *rec);	/* $0117B4 regs(&a0,&a1,a6) clobbers(d0) */
void HudOp_ShowStats(uint32_t *a0, uint32_t *a1, PlayerRec *rec);	/* $0117C8 regs(&a0,&a1,a6) clobbers(d0) */
uint32_t HudOp_ShowText(uint32_t a0, PlayerRec *rec);	/* $0117D0 regs(a0,a6)->a1 clobbers(d0,a0) */
uint32_t HudOp_ShowCredits(uint32_t d0, uint32_t a0, PlayerRec *rec);	/* $0117EA regs(d0,a0,a6)->a1 clobbers(d0,a0) */
