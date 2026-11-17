; ---------------------------------------------------------------------------
; Op2_LoopCounter2 - a2 op: decrements loop counter 2 ($18(a4)) and branches while it is above 0
; In:  a2 = script: word offset
; Out: branch via Op2_Goto while > 0, else NEXT after the offset
; Evidence: subq.w #1,$18(a4) / bgt; 15 runs
; ---------------------------------------------------------------------------
Op2_LoopCounter2:
	subq.w	#$1,$18(a4)
	bgt.w	Op2_Goto
	addq.l	#$2,a2
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_CallNativeRel - a2 op: calls native 68000 code at a relative address (operand address + offset; typically a routine placed among the scripts) and continues
; In:  a2 = script: word offset
; Out: NEXT
; Evidence: movea.l a2,a0 / adda.w (a2)+,a0 / jsr (a0); 131 runs; e.g. $11B82 `2CB2 014E`
; ---------------------------------------------------------------------------
Op2_CallNativeRel:
	movea.l	a2,a0
	adda.w	(a2)+,a0
	jsr	(a0)
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_CallNative - a2 op: calls a native 68000 routine at an absolute address and continues; the usual way scripts reach native callbacks (Fx_SpawnAt*, Spawn_At*, Pal_FlashWhite ...)
; In:  a2 = script: long address (the routine may read further inline operands through a2)
; Out: NEXT
; Evidence: movea.l (a2)+,a0 / jsr (a0); all 153 runs; $1377E `2CBC 00009584` (Pal_FlashWhite)
; ---------------------------------------------------------------------------
Op2_CallNative:
	movea.l	(a2)+,a0
	jsr	(a0)
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_CallNativeRelSaved - a2 op: like Op2_CallNativeRel, but keeps a1-a6 across the call (for routines that clobber the VM registers)
; In:  a2 = script: word offset
; Out: NEXT
; Evidence: movem.l a1-a6 around jsr (a0); 24 runs; $20D32 `2CC4 FD6A`
; ---------------------------------------------------------------------------
Op2_CallNativeRelSaved:
	movea.l	a2,a0
	adda.w	(a2)+,a0
	movem.l	a1-a6,-(a7)
	jsr	(a0)
	movem.l	(a7)+,a1-a6
	movea.w	(a2)+,a0
	jmp	(a0)
