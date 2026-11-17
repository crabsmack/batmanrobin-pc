/* funcs/Scene.h - prototypes of the Scene module: one line per function, its register
 * convention in the tag (dkit genesis recomp bridge). First written from the measured
 * contracts (dkit disasm regs); edited by the C's authors from then on. */
#pragma once
void Scene_InitStage(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *d7, uint32_t *a2, uint32_t *a3, uint32_t *a4, uint32_t *a6);	/* $0099D4 regs(&d0,&d1,&d2,&d3,&d4,&d5,&d6,&d7,&a2,&a3,&a4,&a6) clobbers(a0,a1) */
void Scene_InitPlain(uint32_t *d0, uint16_t *d1, uint32_t *d2, uint16_t *d6, uint32_t *d7, uint32_t *a2, uint32_t *a3, uint32_t *a4, uint32_t a6);	/* $009AA4 regs(&d0,&d1,&d2,&d6,&d7,&a2,&a3,&a4,a6) clobbers(a0,a1,a6) */
void Scene_InitCommon(uint32_t *d0, uint16_t *d1, uint32_t *d2, uint16_t *d6, uint32_t *d7, uint32_t *a2, uint32_t *a3, uint32_t *a4, uint32_t *a6);	/* $009AAC regs(&d0,&d1,&d2,&d6,&d7,&a2,&a3,&a4,&a6) clobbers(a0,a1) */
/* a scene header's setup routine (Scene_InitCommon's jsr (a0) at $9B42): the call-site log's
   targets, Layer_SetPlanes64x64 and Layer_SetPlanes128x32, both regs() */
void SceneSetupFn(uint32_t code);	/* type regs() */
void Scene_FadeOutExit(dk::Flags *ccr);	/* $009D0E regs(&ccr) ccout(NVC) */
void Scene_EndAfterDelayWhite(uint16_t *frames, const u16 **script, dk::Flags *ccr);	/* $009D24 regs(&d0,&a2,&ccr) ccout(NVC) */
/* a scene or section routine called from a list (Scene_RunList's jsr (a0) at $9FEE, Section_RunNext's at
   $9FE0): the union of the targets' conventions in the call-site log (15 and 23 routines, all alike) */
void SceneFn(uint32_t code, uint32_t *d1, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *a1, uint32_t *a2, uint32_t *a3, uint32_t *a4, uint32_t *a5, uint32_t *a6);	/* type regs(&d1,&d3,&d4,&d5,&d6,&a1,&a2,&a3,&a4,&a5,&a6) */
void Scene_RunList(uint32_t *d1, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, const u32 *list, uint32_t *a1, uint32_t *a2, uint32_t *a3, uint32_t *a4, uint32_t *a5, uint32_t *a6);	/* $009FE6 regs(&d1,&d3,&d4,&d5,&d6,a0,&a1,&a2,&a3,&a4,&a5,&a6) clobbers(d0,d2,d7,a0) */
void Scene_ResetBufPtrs(void);	/* $00A228 regs() clobbers(d7,a0,a1) */
void Scene_LoopSkippable(uint32_t d0, uint32_t *d1, uint32_t d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t d7, uint32_t *a1, uint32_t *a2, uint32_t *a3, uint32_t *a4, uint32_t *a5, uint32_t *a6);	/* $047C6A regs(d0,&d1,d2,&d3,&d4,&d5,&d6,d7,&a1,&a2,&a3,&a4,&a5,&a6) clobbers(d0,d2,d7,a0) */
void Scene_EndAfterDelay(uint16_t *frames, const u16 **script, dk::Flags *ccr);	/* $009CE8 regs(&d0,&a2,&ccr) ccout(NVC) */
void Scene_StartEndTimer(uint16_t d0);	/* $009CFA regs(d0) */
void Scene_LoopSkippableDelayed(uint32_t d0, uint32_t *d1, uint32_t d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t d7, uint32_t *a1, uint32_t *a2, uint32_t *a3, uint32_t *a4, uint32_t *a5, uint32_t *a6);	/* $047C64 regs(d0,&d1,d2,&d3,&d4,&d5,&d6,d7,&a1,&a2,&a3,&a4,&a5,&a6) clobbers(d0,d2,d7,a0) */
void Scene_LoopSkippableKeepMode(uint32_t d0, uint32_t *d1, uint32_t d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t d7, uint32_t *a1, uint32_t *a2, uint32_t *a3, uint32_t *a4, uint32_t *a5, uint32_t *a6);	/* $047C70 regs(d0,&d1,d2,&d3,&d4,&d5,&d6,d7,&a1,&a2,&a3,&a4,&a5,&a6) clobbers(d0,d2,d7,a0) */
void Scene_LoopUnskippable(uint32_t d0, uint32_t *d1, uint32_t d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t d7, uint32_t *a1, uint32_t *a2, uint32_t *a3, uint32_t *a4, uint32_t *a5, uint32_t *a6);	/* $047CB0 regs(d0,&d1,d2,&d3,&d4,&d5,&d6,d7,&a1,&a2,&a3,&a4,&a5,&a6) clobbers(d0,d2,d7,a0) */
void Scene_LoopUnskippableKeepMode(uint32_t d0, uint32_t d1, uint32_t d2, uint32_t d3, uint32_t d4, uint32_t d5, uint32_t d6, uint32_t d7, uint32_t a2, uint32_t a3, uint32_t a4);	/* $047CB6 regs(d0,d1,d2,d3,d4,d5,d6,d7,a2,a3,a4) */
