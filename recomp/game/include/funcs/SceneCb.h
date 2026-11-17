/* funcs/SceneCb.h - prototypes of the SceneCb module: one line per function, its register
 * convention in the tag (dkit genesis recomp bridge). First written from the measured
 * contracts (dkit disasm regs); edited by the C's authors from then on. */
#pragma once
void SceneCb_None(void);	/* $009CE6 regs() */
void SceneCb_EndTimer(void);	/* $009D08 regs() */
void SceneCb_EndTimerWhite(void);	/* $009D38 regs() */
uint8_t SceneCb_WaitFadeExit(dk::Flags *ccr);	/* $009D54 regs(&ccr)->d0 ccout(NZVC) */
void SceneCb_CheckGameOver(uint16_t *d0);	/* $009D72 regs(&d0) */
void SceneCb_WaitPlayersExit(uint16_t *var, uint32_t *one, uint32_t *vm);	/* $009D96 regs(&d0,&d1,&a4) clobbers(a0,a6) */
