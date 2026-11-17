/* funcs/HUD.h - prototypes of the HUD module: one line per function, its register
 * convention in the tag (dkit genesis recomp bridge). First written from the measured
 * contracts (dkit disasm regs); edited by the C's authors from then on. */
#pragma once
uint32_t HUD_ShowGoSignCb(void);	/* $0093D2 regs()->d0 tst(d0.l) ccout(NVC) */
void HUD_Init(uint32_t *a0);	/* $0110A2 regs(&a0) */
void HUD_InitPanel(PlayerRec *rec);	/* $0110BC regs(a0) */
uint32_t HUD_PlayerJoined(PlayerRec *rec);	/* $011100 regs(a0)->a1 */
uint32_t HUD_PlayerOut(PlayerRec *rec);	/* $011140 regs(a0)->a1 */
void HUD_ShowBossTime(PlayerRec *rec);	/* $0111C2 regs(a0) */
void HUD_ShowPressStart(PlayerRec *rec, dk::Flags *ccr);	/* $0111FC regs(a0,&ccr) ccout(NVC) */
void HUD_Update(uint32_t *d0, uint32_t d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *d7, uint32_t *a0, uint32_t *a1, uint32_t *a2, uint32_t *a3, uint32_t *a4, uint32_t a5);	/* $0112C8 regs(&d0,d1,&d2,&d3,&d4,&d5,&d6,&d7,&a0,&a1,&a2,&a3,&a4,a5) clobbers(d1,a6) */
void HUD_StepIdle(void);	/* $011310 regs() */
void HUD_StepBranches(uint32_t *d0, uint32_t *a0);	/* $011314 regs(&d0,&a0) clobbers(d1) */
/* The HUD step jump table's slots 8-$14 (entered by HUD_Update's jmp HUD_StepIdle(pc,d0.w), not
 * measured: not functions of their own): every register goes through, as Op2_BranchTouchBox_Kind0's;
 * the C reproduces each one the step leaves. */
void HUD_StepToP1Ring(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *d7, uint32_t *a0, uint32_t *a1, uint32_t *a2, uint32_t *a3, uint32_t *a4, uint32_t *a5, uint32_t *a6);	/* $011318 regs(&d0,&d1,&d2,&d3,&d4,&d5,&d6,&d7,&a0,&a1,&a2,&a3,&a4,&a5,&a6) */
void HUD_StepToP1Panel(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *d7, uint32_t *a0, uint32_t *a1, uint32_t *a2, uint32_t *a3, uint32_t *a4, uint32_t *a5, uint32_t *a6);	/* $01131C regs(&d0,&d1,&d2,&d3,&d4,&d5,&d6,&d7,&a0,&a1,&a2,&a3,&a4,&a5,&a6) */
void HUD_StepToP2Ring(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *d7, uint32_t *a0, uint32_t *a1, uint32_t *a2, uint32_t *a3, uint32_t *a4, uint32_t *a5, uint32_t *a6);	/* $011320 regs(&d0,&d1,&d2,&d3,&d4,&d5,&d6,&d7,&a0,&a1,&a2,&a3,&a4,&a5,&a6) */
void HUD_StepToP2Panel(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *d7, uint32_t *a0, uint32_t *a1, uint32_t *a2, uint32_t *a3, uint32_t *a4, uint32_t *a5, uint32_t *a6);	/* $011324 regs(&d0,&d1,&d2,&d3,&d4,&d5,&d6,&d7,&a0,&a1,&a2,&a3,&a4,&a5,&a6) */
void HUD_DrawWeaponBar(uint32_t *d6, uint32_t *d7, uint32_t *a0, uint32_t a1, uint32_t *a2, uint32_t *a3, uint32_t *a4, uint32_t a6);	/* $0114D2 regs(&d6,&d7,&a0,a1,&a2,&a3,&a4,a6) clobbers(d0,d1) */
void HUD_DrawScore(uint32_t *d2, uint32_t *d3, uint32_t d4, uint32_t *d7, uint32_t *a2, uint32_t a6);	/* $01156E regs(&d2,&d3,d4,&d7,&a2,a6) clobbers(d0,d1,a0,a1,a3) */
void HUD_DrawScoreDigit(uint32_t *d0, uint32_t d1, uint32_t *d2, uint32_t *d7, uint32_t a0, uint32_t a1, uint32_t *a2);	/* $0115F0 regs(&d0,d1,&d2,&d7,a0,a1,&a2) clobbers(d3,a3) */
void HUD_ClearTiles(uint16_t d7, uint32_t a2);	/* $01161C regs(d7,a2) clobbers(d7,a2) */
void HUD_DrawLifeRing(uint32_t *d6, uint32_t *d7, uint32_t *a0, uint32_t *a1, uint32_t *a2, uint32_t *a3, uint32_t *a4, uint32_t a6);	/* $011638 regs(&d6,&d7,&a0,&a1,&a2,&a3,&a4,a6) clobbers(d0,d1) */
void HUD_DrawPanelText(uint32_t *d6, uint32_t *d7, uint32_t a0, uint32_t a1, uint32_t *a2, uint32_t *a3, uint32_t *a4);	/* $011710 regs(&d6,&d7,a0,a1,&a2,&a3,&a4) clobbers(d0) */
void HUD_RunPanelScript(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *d7, uint32_t *a0, uint32_t *a1, uint32_t *a2, uint32_t *a3, uint32_t *a4, uint32_t *a5, uint32_t *a6);	/* $01174A regs(&d0,&d1,&d2,&d3,&d4,&d5,&d6,&d7,&a0,&a1,&a2,&a3,&a4,&a5,&a6) */
void HUD_StepPanelSlide(PlayerRec *rec);	/* $011834 regs(a6) clobbers(d0,d1,d2) */
void HUD_StartSlideIn(uint32_t *d0, uint32_t *d4, uint32_t *d5, PlayerRec *rec);	/* $01189C regs(&d0,&d4,&d5,a6) */
void HUD_StartSlideOut(PlayerRec *rec);	/* $0118E2 regs(a6) clobbers(d0,d1) */
void HUD_DrawPanelSprites(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *d7, uint32_t *a0, uint32_t *a1, uint32_t *a2, uint32_t a6);	/* $01191C regs(&d0,&d1,&d2,&d3,&d4,&d5,&d6,&d7,&a0,&a1,&a2,a6) */
uint32_t HUD_StartGoSignKeepLock(void);	/* $0119E4 regs()->d0 tst(d0.l) ccout(NVC) */
void HUD_StartGoSign(void);	/* $0119EA regs() clobbers(d0) */
void HUD_GoSignLeave(void);	/* $011A0C regs() */
void HUD_UpdateGoSign(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *d7, uint32_t *a0, uint32_t *a1, uint32_t *a2);	/* $011A1C regs(&d0,&d1,&d2,&d3,&d4,&d5,&d6,&d7,&a0,&a1,&a2) */
void HUD_LockJoin(uint32_t *a0);	/* $0111A8 regs(&a0) */
void HUD_UnlockJoin(uint32_t *a0, dk::Flags *ccr);	/* $0111DA regs(&a0,&ccr) ccout(NVC) */
uint32_t HUD_StartGoSign_set(void);	/* $0119EE regs()->d0 tst(d0.l) ccout(NVC) */
void HUD_BossTimeReturn(void);	/* $0111D8 regs() */
void HUD_PressStartReturn(void);	/* $011212 regs() */
void HUD_CopyScoreDigit(uint32_t d2, uint32_t *d3, uint32_t *d7, uint32_t a0, uint32_t a1, uint32_t *a2);	/* $0115FA regs(d2,&d3,&d7,a0,a1,&a2) clobbers(a3) */
/* HUD_PanelNext's own contract ends at its `jmp (a1)` into the op (regs(&d0,&a0,&a1)); the ops
 * run on until one returns, so a C caller (HUD_RunPanelScript) sees what they leave: every
 * register goes through the call, as VM6_Run's. */
void HUD_PanelNext(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *d7, uint32_t *a0, uint32_t *a1, uint32_t *a2, uint32_t *a3, uint32_t *a4, uint32_t *a5, uint32_t *a6);	/* $01175C regs(&d0,&d1,&d2,&d3,&d4,&d5,&d6,&d7,&a0,&a1,&a2,&a3,&a4,&a5,&a6) ccout(NZVC) */
