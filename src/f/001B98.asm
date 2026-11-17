; ---------------------------------------------------------------------------
; Mot_AccelForDistance - computes the acceleration for a script move: distance word * speed factor S ($2E(a4)) / (T * (T + S/16) / 2) * tick^2 ($32(a4)), the arithmetic-series form of covering the distance in T timer units
; In:  a2 = script: word distance, word time T; a4 = object block (speed factor $2E, tick squared $32)
; Out: d1 = acceleration (long); a2 past both words; d0, d2, d3 clobbered
; Evidence: callers Op2_SetAccelForDist ($1782), Op2_SetAccelXForDist ($17AC), Op2_SetAccelYForDist ($1BB4); the distance reading is inferred from the formula; executed in 153 corpus runs
; ---------------------------------------------------------------------------
Mot_AccelForDistance:
	move.w	$2E(a4),d0
	move.w	(a2)+,d1
	move.w	(a2)+,d2
	muls.w	d0,d1
	lsr.w	#$4,d0
	move.w	d2,d3
	add.w	d0,d2
	mulu.w	d3,d2
	lsr.w	#$1,d2
	divu.w	d2,d1
	mulu.w	$32(a4),d1
	rts
; ---------------------------------------------------------------------------
; Op2_SetAccelYForDist - a2 op: sets the y acceleration $28(a3) from a distance and a time (Mot_AccelForDistance)
; In:  a2 = script: word distance, word time; a3 = motion block
; Out: NEXT
; Evidence: bsr Mot_AccelForDistance then move.l d1,$28(a3); 118 corpus runs incl. the TAS
; ---------------------------------------------------------------------------
Op2_SetAccelYForDist:
	bsr.s	Mot_AccelForDistance
	move.l	d1,$28(a3)
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_SetCruiseY - a2 op: sets the y cruise speed $34(a3) (also the fall speed limit of the jump ops) to word * the speed factor $2E(a4)
; In:  a2 = script: word speed; a3 = motion block
; Out: NEXT
; Evidence: mulu.w $2E(a4) to $34(a3); Op2_Jump copies $34 into the y target speed; 146 corpus runs incl. the TAS
; ---------------------------------------------------------------------------
Op2_SetCruiseY:
	move.w	(a2)+,d0
	mulu.w	$2E(a4),d0
	move.l	d0,$34(a3)
	movea.w	(a2)+,a0
	jmp	(a0)
