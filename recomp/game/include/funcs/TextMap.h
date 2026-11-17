/* funcs/TextMap.h - prototypes of the TextMap module: one line per function, its register
 * convention in the tag (dkit genesis recomp bridge). First written from the measured
 * contracts (dkit disasm regs); edited by the C's authors from then on. */
#pragma once
void TextMap_RunInline(uint32_t *d0, uint16_t *d6, uint16_t *d7, uint32_t *list, uint32_t *a1, const u32 **script, uint32_t *a3, uint32_t *a4, uint32_t *a5, uint32_t *a6);	/* $01438A regs(&d0,&d6,&d7,&a0,&a1,&a2,&a3,&a4,&a5,&a6) ccout(NZVC) */
void TextMap_Run(uint32_t *d0, uint16_t *base, uint16_t *d7, uint32_t *list, uint32_t *a1, uint32_t *a2, uint32_t *a3, uint32_t *a4, uint32_t *font, uint32_t *dst);	/* $01438C regs(&d0,&d6,&d7,&a0,&a1,&a2,&a3,&a4,&a5,&a6) ccout(NZVC) */
void TextMap_CmdEnd(uint32_t *a1, uint32_t *a2, uint32_t *a3, uint32_t *a4, uint32_t *a5, uint32_t *a6);	/* $0143AA regs(&a1,&a2,&a3,&a4,&a5,&a6) */
void TextMap_CmdGoto(uint32_t *d0, uint16_t *d6, uint16_t *d7, const u8 **list, uint32_t *a1, uint32_t *a2, uint32_t *a3, uint32_t *a4, uint32_t *a5, uint32_t *a6);	/* $0143B8 regs(&d0,&d6,&d7,&a0,&a1,&a2,&a3,&a4,&a5,&a6) ccout(NZVC) */
void TextMap_CmdSetPos(uint32_t *d0, uint32_t *d1, uint16_t *d6, uint16_t *d7, const u8 **list, uint32_t *a1, uint32_t *a2, uint32_t *a3, uint32_t *a4, uint32_t *a5, uint32_t *dst);	/* $0143CC regs(&d0,&d1,&d6,&d7,&a0,&a1,&a2,&a3,&a4,&a5,&a6) ccout(NZVC) */
void TextMap_ResumeTyping(uint32_t *d0, uint16_t *d6, uint16_t *count, const u8 **list, uint32_t *a1, uint32_t *a2, uint32_t *a3, uint32_t *a4, uint32_t *a5, uint32_t *dst, dk::Flags *ccr);	/* $0143E4 regs(&d0,&d6,&d7,&a0,&a1,&a2,&a3,&a4,&a5,&a6,&ccr) ccout(NZVC) */
void TextMap_CmdText(uint32_t *d0, uint16_t *d6, uint32_t *count, const u8 **list, uint32_t *a1, uint32_t *a2, uint32_t *a3, uint32_t *a4, uint32_t *a5, uint32_t *a6);	/* $014402 regs(&d0,&d6,&d7,&a0,&a1,&a2,&a3,&a4,&a5,&a6) ccout(NZVC) */
void TextMap_PutChars(uint32_t *d0, uint16_t *base, uint16_t *count, const u8 **list, uint32_t *a1, uint32_t *a2, uint32_t *a3, uint32_t *a4, uint32_t *font, uint32_t *dst, dk::Flags *ccr);	/* $014408 regs(&d0,&d6,&d7,&a0,&a1,&a2,&a3,&a4,&a5,&a6,&ccr) ccout(NZVC) */
void TextMap_CmdNumberPlus1(uint32_t *d0, uint32_t *d1, uint16_t *base, uint16_t *d7, const u8 **list, uint32_t *value_at, uint32_t *a2, uint32_t *a3, uint32_t *a4, uint32_t *font, uint32_t *dst);	/* $014440 regs(&d0,&d1,&d6,&d7,&a0,&a1,&a2,&a3,&a4,&a5,&a6) ccout(NZVC) */
void TextMap_PutDigit(uint32_t *d0, uint16_t *seen, uint16_t base, uint32_t font, uint32_t *dst);	/* $014480 regs(&d0,&d1,d6,a5,&a6) */
void TextMap_CmdMenuItem(uint32_t *d0, uint16_t *d6, uint16_t *d7, const u8 **list, uint32_t *a1, uint32_t *a2, uint32_t *a3, uint32_t *a4, uint32_t *a5, uint32_t *a6);	/* $014494 regs(&d0,&d6,&d7,&a0,&a1,&a2,&a3,&a4,&a5,&a6) ccout(NZVC) */
void TextMap_CmdBlinkGoto(uint32_t *d0, uint16_t *d6, uint16_t *d7, const u8 **list, uint32_t *a1, uint32_t *a2, uint32_t *a3, uint32_t *a4, uint32_t *a5, uint32_t *a6);	/* $0144B2 regs(&d0,&d6,&d7,&a0,&a1,&a2,&a3,&a4,&a5,&a6) ccout(NZVC) */
void TextMap_Clear(uint32_t *d0, uint32_t *d7);	/* $0144D4 regs(&d0,&d7) tst(d0.l) ccout(NVC) clobbers(a0) */
void TextMap_LoadFont(uint16_t base, uint32_t *table);	/* $0145BE regs(d0,&a0) clobbers(d0,a1) */
void TextMap_NextCmd(uint32_t *d0, uint16_t *d6, uint16_t *d7, const u8 **list, uint32_t *a1, uint32_t *a2, uint32_t *a3, uint32_t *a4, uint32_t *a5, uint32_t *a6, dk::Flags *ccr);	/* $0143A4 regs(&d0,&d6,&d7,&a0,&a1,&a2,&a3,&a4,&a5,&a6,&ccr) ccout(NZVC) */
void TextMap_CmdReturn(uint32_t *d0, uint16_t *d6, uint16_t *d7, const u8 **list, uint32_t *a1, uint32_t *a2, uint32_t *a3, uint32_t *a4, uint32_t *a5, uint32_t *a6);	/* $0143BC regs(&d0,&d6,&d7,&a0,&a1,&a2,&a3,&a4,&a5,&a6) ccout(NZVC) */
void TextMap_CmdNumber(uint32_t *d0, uint32_t *d1, uint16_t *base, uint16_t *d7, const u8 **list, uint32_t *value_at, uint32_t *a2, uint32_t *a3, uint32_t *a4, uint32_t *font, uint32_t *dst);	/* $01444A regs(&d0,&d1,&d6,&d7,&a0,&a1,&a2,&a3,&a4,&a5,&a6) ccout(NZVC) */
void TextMap_CmdTypewriter(uint32_t *d0, uint16_t *d6, uint16_t *d7, const u8 **list, uint32_t *a1, uint32_t *a2, uint32_t *a3, uint32_t *a4, uint32_t *a5, uint32_t *a6);	/* $0144C2 regs(&d0,&d6,&d7,&a0,&a1,&a2,&a3,&a4,&a5,&a6) ccout(NZVC) */
void TextMap_Upload(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint16_t *d3, uint16_t *d4, uint16_t *d5, uint16_t *d6, uint32_t *d7, uint32_t a0);	/* $0144FC regs(&d0,&d1,&d2,&d3,&d4,&d5,&d6,&d7,a0) clobbers(a0) */
void TextMap_UploadRows(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint16_t *d3, uint16_t *d4, uint16_t *d5, uint16_t *d6, uint16_t *d7, uint32_t a0);	/* $014502 regs(&d0,&d1,&d2,&d3,&d4,&d5,&d6,&d7,a0) clobbers(a0) */
void TextMap_PutCharsNext(uint32_t *d0, uint16_t *base, uint16_t *count, const u8 **list, uint32_t *a1, uint32_t *a2, uint32_t *a3, uint32_t *a4, uint32_t *font, uint32_t *dst, dk::Flags *ccr);	/* $01442E regs(&d0,&d6,&d7,&a0,&a1,&a2,&a3,&a4,&a5,&a6,&ccr) ccout(NZVC) */
