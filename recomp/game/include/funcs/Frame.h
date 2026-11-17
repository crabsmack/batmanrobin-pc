/* funcs/Frame.h - prototypes of the Frame module: one line per function, its register
 * convention in the tag (dkit genesis recomp bridge). First written from the measured
 * contracts (dkit disasm regs); edited by the C's authors from then on. */
#pragma once
void Frame_BeginStage(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *d7, uint32_t *a0, uint32_t *a1, uint32_t *a2, uint32_t *a3, uint32_t *a4, uint32_t *a5, uint32_t *a6);	/* $009B86 regs(&d0,&d1,&d2,&d3,&d4,&d5,&d6,&d7,&a0,&a1,&a2,&a3,&a4,&a5,&a6) */
void Frame_Begin(uint32_t *d0, uint32_t *d1, uint32_t *d7, uint32_t *a1);	/* $009BAC regs(&d0,&d1,&d7,&a1) clobbers(a0) */
void Frame_RunWorld(uint32_t d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t d7, uint32_t *a0, uint32_t *a1, uint32_t *a2, uint32_t *a3, uint32_t *a4, uint32_t *a5, uint32_t *a6);	/* $009BC0 regs(d0,&d1,&d2,&d3,&d4,&d5,&d6,d7,&a0,&a1,&a2,&a3,&a4,&a5,&a6) clobbers(d0,d7) */
/* the per-frame scene callback g_FrameCallback (Frame_End's jsr (a0) at $9C24): the union of its
   targets' conventions in the call-site log (SceneCb_None, _EndTimer, _EndTimerWhite, _WaitFadeExit,
   _CheckGameOver, _WaitPlayersExit) */
void FrameCallbackFn(uint32_t code, uint32_t *d0, uint32_t *d1, uint32_t *a4);	/* type regs(&d0,&d1,&a4) */
void Frame_End(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *d7, uint32_t *a2, uint32_t *a3, uint32_t *a4, uint32_t a5, uint32_t a6);	/* $009BF0 regs(&d0,&d1,&d2,&d3,&d4,&d5,&d6,&d7,&a2,&a3,&a4,a5,a6) clobbers(a0,a1,a5,a6) */
void Frame_BeginStageHook(void);	/* $015674 regs() */
void Frame_RunWorldLayerA(uint32_t d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t d7, uint32_t *a0, uint32_t *a1, uint32_t *a2, uint32_t *a3, uint32_t *a4, uint32_t *a5, uint32_t *a6);	/* $009BD4 regs(d0,&d1,&d2,&d3,&d4,&d5,&d6,d7,&a0,&a1,&a2,&a3,&a4,&a5,&a6) clobbers(d0,d7) */
void Frame_RunFreeCam(uint32_t d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *d7, uint32_t *a0, uint32_t *a1, uint32_t *a2, uint32_t a3, uint32_t a4, uint32_t a5, uint32_t *a6);	/* $009BEA regs(d0,&d1,&d2,&d3,&d4,&d5,&d6,&d7,&a0,&a1,&a2,a3,a4,a5,&a6) clobbers(d0) */
