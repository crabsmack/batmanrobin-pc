; ---------------------------------------------------------------------------
; Op2_AimAtArgPoint - a2 op: makes (g_VM2Arg0, g_VM2Arg1) the motion target (Mot_SetTarget) and points the target heading $A(a3) at it (atan2, Mot_Atan2)
; In:  a2 = script (no operand); g_VM2Arg0/1 = target x/y
; Out: NEXT
; Evidence: the file also holds Op2_AimAtTargetOffset ($1382) and Op2_AimAtTargetArgOffset ($13A4); 56 TAS frames
; ---------------------------------------------------------------------------
Op2_AimAtArgPoint:
	move.w	(g_VM2Arg0).w,d0
	move.w	(g_VM2Arg1).w,d1
	bsr.w	Mot_SetTarget
	jsr	(Mot_Atan2).l
	andi.w	#$3FF,d2
	move.w	d2,$A(a3)
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_AimAtTargetOffset - a2 op: moves the motion target by (dx, dy) words from its current $14/$16(a3) and points the target heading at it
; In:  a2 = script: words dx, dy
; Out: NEXT
; Evidence: executed in the TAS
; ---------------------------------------------------------------------------
Op2_AimAtTargetOffset:
	move.w	(a2)+,d0
	move.w	(a2)+,d1
	add.w	$14(a3),d0
	add.w	$16(a3),d1
	bsr.w	Mot_SetTarget
	jsr	(Mot_Atan2).l
	andi.w	#$3FF,d2
	move.w	d2,$A(a3)
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_AimAtTargetArgOffset - a2 op: moves the motion target by (g_VM2Arg0, g_VM2Arg1) from its current $14/$16(a3) and points the target heading at it
; In:  a2 = script (no operand)
; Out: NEXT
; Evidence: executed in the TAS
; ---------------------------------------------------------------------------
Op2_AimAtTargetArgOffset:
	move.w	(g_VM2Arg0).w,d0
	move.w	(g_VM2Arg1).w,d1
	add.w	$14(a3),d0
	add.w	$16(a3),d1
	bsr.w	Mot_SetTarget
	jsr	(Mot_Atan2).l
	andi.w	#$3FF,d2
	move.w	d2,$A(a3)
	movea.w	(a2)+,a0
	jmp	(a0)
