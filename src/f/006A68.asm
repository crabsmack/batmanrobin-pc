; ---------------------------------------------------------------------------
; VM5_Yield - a5-VM yield tail (not dispatched): ends this frame's run of the wave script: saves the pc in $1E(a6) and returns through the saved stack pointer (the timer set by the op before decides when it resumes)
; In:  a5 = script (no operands)
; Out: rts to the object list walk
; Evidence: move.l a5,$1E / movea.l g_VM5SavedSP,a7 / rts; entered only by bra from Op5_Wait / Op5_WaitRandom (no computed-jump entry in the corpus)
; ---------------------------------------------------------------------------
VM5_Yield:
	move.l	a5,$1E(a6)
	movea.l	(g_VM5SavedSP).w,a7
	rts
; ---------------------------------------------------------------------------
; Op5_End - a5 op: ends the wave: restores the saved stack, closes its kill group (VM5_CloseGroup), deletes the object and decrements g_WaveCount
; In:  a5 = script (no operands)
; Out: rts to the object list walk (the object is gone)
; Evidence: movea.l g_VM5SavedSP,a7 then VM5_EndWave; Wave_Start's default quota address $939E holds a lone $6A72; 95 script uses, e.g. $21D76, $21D86; 35 corpus runs
; ---------------------------------------------------------------------------
Op5_End:
	movea.l	(g_VM5SavedSP).w,a7
; ---------------------------------------------------------------------------
; VM5_EndWave - ends a wave director: closes its kill group (VM5_CloseGroup), unlinks and frees the object ($8ABE) and decrements g_WaveCount (floored at 0)
; In:  a6 = wave object
; Out: rts
; Evidence: entered from VM5_RunObject when g_WaveGen > $A(a6) and from Op5_End
; ---------------------------------------------------------------------------
VM5_EndWave:
	bsr.w	VM5_CloseGroup
	bsr.w	Obj_UnlinkFree
	subq.w	#$1,(g_WaveCount).w
	bge.s	loc_006A88
	clr.w	(g_WaveCount).w
loc_006A88:
	rts
; ---------------------------------------------------------------------------
; Op5_ClearInterval - a5 op: clears the spawn interval $2A and the timer $2E, so spawn ops continue on the next frame
; In:  a5 = script (no operands)
; Out: NEXT
; Evidence: clr.l $2A / clr.l $2E; the spawn ops reload $2E from $2A ($7052); 29 script uses, e.g. $21D60 (`6A8A 6D2A`); 28 corpus runs
; ---------------------------------------------------------------------------
Op5_ClearInterval:
	clr.l	$2A(a6)
	clr.l	$2E(a6)
	movea.w	(a5)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op5_SetInterval - a5 op: sets the wait after every spawn op: interval $2A = t * 16 (1/16-frame units at the normal g_WaveTimerStep $100), timer $2E = 0
; In:  a5 = script: word t (1/16 frames)
; Out: NEXT
; Evidence: move.w (a5)+ / asl.l #4 / $2A; 43 script uses, e.g. $16B48 (`6A96 01E0`: 30 frames), $21E0A (`6A96 03C0`); 24 corpus runs
; ---------------------------------------------------------------------------
Op5_SetInterval:
	moveq	#$0,d0
	move.w	(a5)+,d0
	asl.l	#$4,d0
	move.l	d0,$2A(a6)
	clr.l	$2E(a6)
	movea.w	(a5)+,a0
	jmp	(a0)
