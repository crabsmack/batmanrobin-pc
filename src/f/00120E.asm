; ---------------------------------------------------------------------------
; Op2_AllocSubBlock - a2 op: makes sure the object owns a pool block at the handle word a6+offset (allocates one with ObjPool_Alloc when it is 0) and clears the block's first 64 bytes; with the pool empty it backs a2 up to this op and yields, retrying next frame
; In:  a2 = script: word offset of a handle in the object (a6)
; Out: NEXT via Op2_ClearNewBlock (a0 = the block); on pool exhaustion Op2_Yield with a2 - 4
; Evidence: subq.l #4,a2 / bra Op2_Yield re-runs the op word + operand; the file also holds Op2_LoadA3Handle ($122C); 20 TAS frames
; ---------------------------------------------------------------------------
Op2_AllocSubBlock:
	move.w	(a2)+,d1
	move.w	(a6,d1.w),d0
	bne.s	loc_001228
	bsr.w	ObjPool_Alloc
	bne.s	loc_001222
	move.w	a0,(a6,d1.w)
	bra.s	Op2_ClearNewBlock
loc_001222:
	subq.l	#$4,a2
	bra.w	Op2_Yield
loc_001228:
	movea.w	d0,a0
	bra.s	Op2_ClearNewBlock
; ---------------------------------------------------------------------------
; Op2_LoadA3Handle - a2 op: makes the block whose handle is the word at a6+offset the current motion block a3, so the motion ops act on it
; In:  a2 = script: word offset of a handle in the object
; Out: NEXT with a3 = that block (Op2_ResetA3 restores $8(a6))
; Evidence: movea.w (a6,d0.w),a3; executed in the TAS
; ---------------------------------------------------------------------------
Op2_LoadA3Handle:
	move.w	(a2)+,d0
	movea.w	(a6,d0.w),a3
	movea.w	(a2)+,a0
	jmp	(a0)
