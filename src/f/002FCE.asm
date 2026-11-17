; ---------------------------------------------------------------------------
; Op2_SetRandJitter - a2 op: sets the object's random offset $1C(a6) to a value in [-w, w) (Rand_Next * w * 2 >> 16), so enemies sharing a script keep slightly different distances
; In:  a2 = script: word w
; Out: NEXT
; Evidence: bsr Rand_Next / muls.w (a2)+ / add.l d0,d0 / swap; $1C(a6) is added to the windows of Op2_HoldRangeOfPlayerX* and Op2_HoldPlayerWindow; 26 runs; $1BF76 `2FCE 0020`
; ---------------------------------------------------------------------------
Op2_SetRandJitter:
	bsr.w	Rand_Next
	muls.w	(a2)+,d0
	add.l	d0,d0
	swap	d0
	move.w	d0,$1C(a6)
	movea.w	(a2)+,a0
	jmp	(a0)
