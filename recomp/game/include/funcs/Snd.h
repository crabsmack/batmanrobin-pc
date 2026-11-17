/* funcs/Snd.h - prototypes of the Snd module: one line per function, its register
 * convention in the tag (dkit genesis recomp bridge). First written from the measured
 * contracts (dkit disasm regs); edited by the C's authors from then on. */
#pragma once
uint32_t Snd_ResetFade(void);	/* $1E500E regs()->d0 clobbers(a0) */
void Snd_SendSfx(void);	/* $1E5044 regs() clobbers(d0,d6,d7,a0) */
void Snd_StartSong(uint32_t *d0, uint32_t *d1, uint32_t *d7, const SndModule *module, uint32_t a1);	/* $1E5066 regs(&d0,&d1,&d7,a0,a1) clobbers(a0,a2,a3) */
uint32_t Snd_LoadDriver(uint8_t d1);	/* $1E5074 regs(d1)->a5 clobbers(d0,d2,a0) */
uint32_t Snd_StopChannels(void);	/* $1E5082 regs()->d7 clobbers(d0) */
void Snd_PlaySfx(uint32_t *d0);	/* $1E5F88 regs(&d0) */
uint32_t Snd_Init(void);	/* $1E5FFA regs()->a5 clobbers(d0,d1,d2,d7,a0) */
void Snd_PlaySong(uint32_t *d0, uint32_t *d1, uint32_t *d7, uint32_t *a0, uint32_t a1);	/* $1E6012 regs(&d0,&d1,&d7,&a0,a1) clobbers(a2,a3) */
uint32_t Snd_ResetFadeBody(u16 *level);	/* $1E799A regs(a0)->d0 */
void Snd_WriteFadeByte(uint8_t d0);	/* $1E79A4 regs(d0) */
void Snd_SendSfxBody(SndSfxSlot *slots);	/* $1E7AF8 regs(a0) clobbers(d0,d6,d7,a0,a1) */
void Snd_StartSongBody(uint32_t *d0, uint32_t *d1, uint32_t *d7, const SndModule *module);	/* $1E7B70 regs(&d0,&d1,&d7,a0) clobbers(a0,a1,a2,a3) */
void Snd_InitSongChannel(uint32_t *d0, uint32_t *d1, uint32_t *a0, uint32_t a1, uint32_t *a3);	/* $1E7BBE regs(&d0,&d1,&a0,a1,&a3) clobbers(a2) */
uint32_t Snd_StopChannelsBody(void);	/* $1E7D52 regs()->d7 clobbers(d0,a1) */
void Snd_StartSongFinish(uint32_t *a0, uint32_t *a1);	/* $1E7C70 regs(&a0,&a1) ccout(NZVC) */
uint32_t Snd_LoadDriverBody(uint8_t d1);	/* $1E7CAC regs(d1)->a5 clobbers(d0,d2,a0) */
