/* funcs/Sound.h - prototypes of the Sound module: one line per function, its register
 * convention in the tag (dkit genesis recomp bridge). First written from the measured
 * contracts (dkit disasm regs); edited by the C's authors from then on. */
#pragma once
void Sound_PlayMusic(uint32_t *d0, uint32_t *d1, uint32_t *d7, uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4, uint32_t a5, uint32_t a6);	/* $00A1BE regs(&d0,&d1,&d7,a1,a2,a3,a4,a5,a6) clobbers(a0) */
void Sound_PlayMusicAnyway(uint32_t *d0, uint32_t *d1, uint32_t *d7, uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4, uint32_t a5, uint32_t a6);	/* $00A1C4 regs(&d0,&d1,&d7,a1,a2,a3,a4,a5,a6) clobbers(a0) */
void Sound_RestartMusic(uint32_t *d0, uint32_t *d1, uint32_t *d7, uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4, uint32_t a5, uint32_t a6);	/* $00A1CA regs(&d0,&d1,&d7,a1,a2,a3,a4,a5,a6) clobbers(a0) */
void Sound_MusicDone(void);	/* $00A1EE regs() */
