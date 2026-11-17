; ---------------------------------------------------------------------------
; Op2_SetCurHeadingArg0 - a2 op: sets the integer part of the current heading $C(a3) to g_VM2Arg0 & $3FF
; In:  a2 = script (no operand); g_VM2Arg0 = heading
; Out: NEXT
; Evidence: the file also holds Op2_AddHeading, Op2_SetHeadingNow, Op2_HeadingFromVel; 3.8k TAS frames
; ---------------------------------------------------------------------------
Op2_SetCurHeadingArg0:
	move.w	(g_VM2Arg0).w,d0
	andi.w	#$3FF,d0
	move.w	d0,$C(a3)
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_AddHeading - a2 op: adds a word to the target heading $A(a3), wrapping at $400
; In:  a2 = script: word delta
; Out: NEXT
; Evidence: executed in the TAS
; ---------------------------------------------------------------------------
Op2_AddHeading:
	move.w	(a2)+,d0
	add.w	$A(a3),d0
	andi.w	#$3FF,d0
	move.w	d0,$A(a3)
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_SetHeadingNow - a2 op: sets both the target heading $A(a3) and the current heading $C(a3) (fraction 0) to word & $3FF, so it takes effect without turning
; In:  a2 = script: word heading
; Out: NEXT
; Evidence: executed in the TAS
; ---------------------------------------------------------------------------
Op2_SetHeadingNow:
	moveq	#$0,d0
	move.w	(a2)+,d0
	andi.w	#$3FF,d0
	move.w	d0,$A(a3)
	swap	d0
	move.l	d0,$C(a3)
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_HeadingFromVel - a2 op: points the target and current headings along the motion block's velocity (atan2 of $18/$1C(a3) >> 8, Mot_Atan2) and clears the target speeds $24/$2C(a3)
; In:  a2 = script (no operand)
; Out: NEXT
; Evidence: also the tail of Op2_BranchPastTarget when it branches; executed in the TAS
; ---------------------------------------------------------------------------
Op2_HeadingFromVel:
	move.l	$18(a3),d0
	move.l	$1C(a3),d1
	asr.l	#$8,d0
	asr.l	#$8,d1
	jsr	(Mot_Atan2).l
	andi.w	#$3FF,d2
	move.w	d2,$A(a3)
	swap	d2
	clr.w	d2
	move.l	d2,$C(a3)
	clr.l	$24(a3)
	clr.l	$2C(a3)
	movea.w	(a2)+,a0
	jmp	(a0)
