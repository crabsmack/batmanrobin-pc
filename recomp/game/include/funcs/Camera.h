/* funcs/Camera.h - prototypes of the Camera module: one line per function, its register
 * convention in the tag (dkit genesis recomp bridge). First written from the measured
 * contracts (dkit disasm regs); edited by the C's authors from then on. */
#pragma once
uint32_t Camera_IsLocked(void);	/* $0093A0 regs()->d0 */
void Camera_UpdateView(uint32_t *d0, uint32_t *d1, uint32_t *d2);	/* $0093DC regs(&d0,&d1,&d2) */
void Camera_TrackPlayersX(uint32_t *d1, uint32_t *d2);	/* $00943E regs(&d1,&d2) clobbers(d0) */
void Camera_LockHere(void);	/* $0093AE regs() clobbers(d0) */
void Camera_LockRightEdge(uint32_t *d4, dk::Flags *ccr);	/* $0093C2 regs(&d4,&ccr) ccout(Z) clobbers(d0) */
void Camera_TrackPlayersY(uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *d7);	/* $0094D8 regs(&d2,&d3,&d4,&d5,&d6,&d7) clobbers(d0,d1) */
void Camera_TrackPlayersXLayerA(uint32_t *d1, uint32_t *d2, uint32_t *layer);	/* $012790 regs(&d1,&d2,&a6) clobbers(d0) */
