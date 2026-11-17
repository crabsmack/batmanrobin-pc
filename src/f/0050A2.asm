; ---------------------------------------------------------------------------
; Op2_StopOsc - a2 op: stops both axes of the oscillator block in an a6 field by zeroing their max speeds (+$C, +$20), so Op2_StepOsc brakes them to rest
; In:  a2 = script: word field offset (empty field: nothing)
; Out: NEXT
; Evidence: clr.l $C(a0) / clr.l $20(a0); the never-run $50BA does the x axis only; 3 corpus runs; $3B37E `50A2 000E`
; ---------------------------------------------------------------------------
Op2_StopOsc:
	move.w	(a2)+,d0
	move.w	(a6,d0.w),d0
	beq.w	VM2_Next
	movea.w	d0,a0
	clr.l	$C(a0)
	clr.l	$20(a0)
	movea.w	(a2)+,a0
	jmp	(a0)
