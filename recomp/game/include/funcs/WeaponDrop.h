/* funcs/WeaponDrop.h - prototypes of the WeaponDrop module: one line per function, its register
 * convention in the tag (dkit genesis recomp bridge). First written from the measured
 * contracts (dkit disasm regs); edited by the C's authors from then on. */
#pragma once
void WeaponDrop_CommandNative(uint32_t *d0, uint16_t *d1, uint16_t d4, uint16_t d5, uint32_t *d7, uint32_t *command_at, const PTR(const u16) **script, uint32_t a4, uint32_t a6);	/* $013EF0 regs(&d0,&d1,d4,d5,&d7,&a1,&a2,a4,a6) ccout(NVC) clobbers(a0) */
void WeaponDrop_Command(uint32_t *d0, uint16_t *d1, uint16_t d4, uint16_t d5, uint32_t *d7, uint32_t *a0, const PTR(const u16) **command, uint32_t *script, uint32_t *a3, uint32_t *vm, uint32_t *a5, uint32_t *director);	/* $013F02 regs(&d0,&d1,d4,d5,&d7,&a0,&a1,&a2,&a3,&a4,&a5,&a6) tst(d0.l) ccout(NZVC) */
void WeaponDrop_Stop(uint32_t d0, uint32_t d1, uint32_t d2, uint32_t d3, uint32_t d4, uint32_t d5, uint32_t d6, uint32_t d7, uint32_t a1, uint32_t a3, uint32_t a4, uint32_t a5, uint32_t a6);	/* $013F36 regs(d0,d1,d2,d3,d4,d5,d6,d7,a1,a3,a4,a5,a6) */
void WeaponDrop_FromTop(uint32_t *d0, uint16_t *d1, uint16_t *x, uint16_t *y, uint32_t *d7, const s16 **script, uint32_t a4, uint32_t a6);	/* $013F5E regs(&d0,&d1,&d4,&d5,&d7,&a2,a4,a6) clobbers(a0) */
void WeaponDrop_SpawnOnScreen(uint32_t *d0, uint16_t *d1, uint16_t *x, uint16_t *y, uint32_t *d7, const u32 **script, uint32_t a4, uint32_t a6);	/* $013FA4 regs(&d0,&d1,&d4,&d5,&d7,&a2,a4,a6) clobbers(a0) */
void WeaponDrop_FromRight(uint32_t *d0, uint16_t *d1, uint16_t *x, uint16_t *y, uint32_t *d7, const s16 **script, uint32_t a4, uint32_t a6);	/* $013F7C regs(&d0,&d1,&d4,&d5,&d7,&a2,a4,a6) clobbers(a0) */
void WeaponDrop_ItemsOut(uint32_t *d0);	/* $013FBA regs(&d0) */
void DropSpawnerFn(uint32_t code, uint32_t *d0, uint16_t *d1, uint16_t x, uint16_t y, uint32_t *d7, uint32_t a4, uint32_t a6, uint32_t *a2);	/* type regs(&d0,&d1,d4,d5,&d7,a4,a6,&a2) */
void WeaponDrop_FromTopRandom(uint32_t *d0, uint16_t *d1, uint16_t *x, uint16_t *y, uint32_t *d7, const s16 **script, uint32_t a4, uint32_t a6);	/* $013F92 regs(&d0,&d1,&d4,&d5,&d7,&a2,a4,a6) clobbers(a0) */
