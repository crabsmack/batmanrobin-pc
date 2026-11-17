/* funcs/VM6.h - prototypes of the VM6 module: one line per function, its register
 * convention in the tag (dkit genesis recomp bridge). First written from the measured
 * contracts (dkit disasm regs); edited by the C's authors from then on. */
#pragma once
/* VM6_RunHook's own contract ends at its `jmp (a0)` to the hook (regs(&d0,&a0)); a C caller (Frame_End)
 * sees the a6 script the hook resumes, so every register goes through the call, as VM6_Run's. */
void VM6_RunHook(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *d7, uint32_t *a0, uint32_t *a1, uint32_t *a2, uint32_t *a3, uint32_t *a4, uint32_t *a5, uint32_t *a6);	/* $007086 regs(&d0,&d1,&d2,&d3,&d4,&d5,&d6,&d7,&a0,&a1,&a2,&a3,&a4,&a5,&a6) */
void VM6_StartFromTrigger(uint32_t *a1, uint32_t *a6);	/* $007092 regs(&a1,&a6) ccout(Z) clobbers(d0) */
uint32_t VM6_Start(uint32_t script);	/* $007094 regs(a6)->d0 tst(d0.l) ccout(Z) */
/* VM6_Run's own contract ends at its `jmp (a0)` (regs(&a6)); a C caller sees the whole script:
 * every op until Op6_End's rts (they write d0-d7/a0-a3, the natives anything), so every register
 * goes through the call (the stub restores the ones not named here). */
void VM6_Run(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *d7, uint32_t *a1, uint32_t *a2, uint32_t *a3, uint32_t *a4, uint32_t *a5, uint32_t *a6);	/* $0070A4 regs(&d0,&d1,&d2,&d3,&d4,&d5,&d6,&d7,&a1,&a2,&a3,&a4,&a5,&a6) clobbers(a0) */
uint32_t VM6_Resume(void);	/* $0070A8 regs()->a6 clobbers(a0) */
void VM6_LoadPalettes(uint8_t *mask, uint32_t *d7, uint32_t *a0, uint32_t *a1, const u16 **script);	/* $007186 regs(&d0,&d7,&a0,&a1,&a6) ccout(NZVC) */
void VM6_InitLayer(uint16_t *d0, uint16_t *d1, uint32_t *d2, uint16_t *d6, uint32_t *a1, uint32_t *a2, uint32_t *a3, uint32_t *a4, uint32_t *a6);	/* $007202 regs(&d0,&d1,&d2,&d6,&a1,&a2,&a3,&a4,&a6) clobbers(a0) */
void VM6_SetupClearRect(uint16_t d0, uint32_t *d1, uint16_t *d2, uint16_t *d3, uint32_t *a0, uint32_t *a6);	/* $007256 regs(d0,&d1,&d2,&d3,&a0,&a6) clobbers(d0) */
void VM6_StartClearJob(uint32_t *d0, uint32_t *a0, uint32_t *a1, uint32_t *a6, dk::Flags *ccr);	/* $0072D2 regs(&d0,&a0,&a1,&a6,&ccr) ccout(NZVC) */
void VM6_StartRedraw(uint32_t a0, uint32_t a6, dk::Flags *ccr);	/* $007416 regs(a0,a6,&ccr) ccout(NZVC) */
void VM6_SetupMapBlit(uint16_t d0, uint32_t *d1, uint16_t *d2, uint32_t *d3, uint16_t *d4, uint16_t *d5, uint32_t *d6, uint16_t *d7, uint32_t *a0, uint32_t *a1, uint32_t *a6);	/* $0074A8 regs(d0,&d1,&d2,&d3,&d4,&d5,&d6,&d7,&a0,&a1,&a6) */
uint32_t VM6_Reload(void);	/* $0070AC regs()->a6 clobbers(a0) */
void VM6_Next(const u16 **script);	/* $0070B0 regs(&a6) clobbers(a0) */
void VM6_WaitBgDecompress(uint32_t *a6);	/* $00713E regs(&a6) clobbers(a0) */
void VM6_EndClearJob(uint32_t *a1, uint32_t *a6);	/* $0072F4 regs(&a1,&a6) clobbers(a0) */
void VM6_VIntClearRows(uint32_t ctrl, uint32_t data, uint32_t job);	/* $007302 regs(a0,a1,a6) clobbers(d0,d1,d2,d6,d7) */
void VM6_RedrawStep(uint32_t *d1, uint32_t *d2, uint32_t *d3, uint16_t *d4, uint32_t *d5, uint16_t *d6, uint16_t *d7, uint32_t *a2, uint32_t *a3, uint32_t *a6);	/* $00743A regs(&d1,&d2,&d3,&d4,&d5,&d6,&d7,&a2,&a3,&a6) clobbers(d0,a0) */
void VM6_BlitMapNow(uint16_t *d0, uint32_t *d1, uint32_t *d2, uint16_t *d3, uint32_t *d6, uint16_t *d7, uint32_t *a1, uint32_t *a3, uint32_t *a6);	/* $007522 regs(&d0,&d1,&d2,&d3,&d6,&d7,&a1,&a3,&a6) clobbers(a0) */
void VM6_StartBlitJob(uint32_t *d0, uint32_t *a0, uint32_t *a1, uint32_t *a6, dk::Flags *ccr);	/* $00756E regs(&d0,&a0,&a1,&a6,&ccr) ccout(NZVC) */
void VM6_EndBlitJob(uint32_t *a1, uint32_t *a6);	/* $007590 regs(&a1,&a6) clobbers(a0) */
void VM6_VIntBlitRows(uint32_t ctrl, uint32_t data, uint32_t job);	/* $00759E regs(a0,a1,a6) clobbers(d0,d1,d2,d3,d5,d6,d7,a3) */
void VM6_DMAStep(uint16_t *d1, uint32_t *d2, uint32_t *d3, uint32_t a0, uint32_t *a6);	/* $0076AE regs(&d1,&d2,&d3,a0,&a6) clobbers(d0,a0) */
void VM6_ClearJobIdle(void);	/* $007364 regs() */
void VM6_BlitJobIdle(void);	/* $0075FC regs() */
