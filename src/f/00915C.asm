; ---------------------------------------------------------------------------
; Fx_SpawnKnockback - native callback: runs an a4 spawn script at a mirrored offset, the spawned objects inheriting 3/4 of the object's knockback speed
; In:  a3 = motion block (+2/+6 knockback x/y speed, decayed by $4BBE), a2 = script: dc.w dx, dy, dc.l a4 script
; Out: a2 past the arguments; a1-a6 preserved
; Evidence: g_SpawnCtx+$3E/$42 = a3+2/+6 minus a quarter; a4 ops $55D8/$560E add ctx $36-$42 into the new object's velocities ($36-$42, integrated by $6286); one script use $33C22; 20 TAS frames
; ---------------------------------------------------------------------------
Fx_SpawnKnockback:
	moveq	#$0,d3
	moveq	#$0,d2
	moveq	#$0,d4
	moveq	#$0,d5
	move.w	$3E(a6),d4
	move.w	$42(a6),d5
	move.w	(a2)+,d0
	btst	#$3,$38(a5)
	beq.s	loc_009178
	neg.w	d0
loc_009178:
	add.w	d0,d4
	move.w	(a2)+,d0
	btst	#$4,$38(a5)
	beq.s	loc_009186
	neg.w	d0
loc_009186:
	add.w	d0,d5
	swap	d4
	swap	d5
	movea.l	(a2)+,a0
	movem.l	a1-a6,-(a7)
	movea.l	a0,a4
	lea.l	(g_SpawnCtx).w,a6
	clr.l	$36(a6)
	clr.l	$3A(a6)
	move.w	$38(a5),$22(a6)
	movem.l	$2(a3),d0-d1
	move.l	d0,d6
	move.l	d1,d7
	asr.l	#$2,d6
	asr.l	#$2,d7
	sub.l	d6,d0
	sub.l	d7,d1
	movem.l	d0-d1,$3E(a6)
	bsr.w	VM4_Next
	movem.l	(a7)+,a1-a6
	rts
