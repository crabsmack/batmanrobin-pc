; ---------------------------------------------------------------------------
; Op2_StopCruiseX - a2 op: clears the x cruise speed $30(a3), then the x target speed (falls into Op2_ClearTargetSpeedX)
; In:  a2 = script (no operand); a3 = motion block
; Out: NEXT
; Evidence: clr.l $30(a3) falls into $185E; 89 corpus runs incl. the TAS
; ---------------------------------------------------------------------------
Op2_StopCruiseX:
	clr.l	$30(a3)
; ---------------------------------------------------------------------------
; Op2_ClearTargetSpeedX - a2 op: sets the x target speed $24(a3) to 0, so the steering tail brakes the x velocity to a stop by the x acceleration
; In:  a2 = script (no operand); a3 = motion block
; Out: NEXT
; Evidence: clr.l $24(a3); $4B8A moves $18 toward $24 by $20; 152 corpus runs incl. the TAS
; ---------------------------------------------------------------------------
Op2_ClearTargetSpeedX:
	clr.l	$24(a3)
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_ClearTargetSpeeds - a2 op: sets both target speeds (x $24, y $2C of a3) to 0, braking in both axes
; In:  a2 = script (no operand); a3 = motion block
; Out: NEXT
; Evidence: two clr.l; 17 corpus runs incl. the TAS
; ---------------------------------------------------------------------------
Op2_ClearTargetSpeeds:
	clr.l	$2C(a3)
	clr.l	$24(a3)
	movea.w	(a2)+,a0
	jmp	(a0)
