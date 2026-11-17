; ---------------------------------------------------------------------------
; Op4_ChildRangeVelX - a4 op: adds base + a random [0, w) (8.8 px/frame) to the child's x velocity $3E(a5)
; In:  a4 = script: word base (8.8), word w (8.8); a5 = child
; Out: NEXT
; Evidence: the Op4_RangeVelX code on $3E(a5); script uses $10DA4 / $10DB6 in the skid dust scripts Scr4_010D9A / Scr4_010DAC (`FF80 FF80` / `0080 0080`), $2530A; 89 corpus runs
; ---------------------------------------------------------------------------
Op4_ChildRangeVelX:
	bsr.w	Rand_Next
	move.w	(a4)+,d1
	ext.l	d1
	asl.l	#$8,d1
	lsr.w	#$4,d0
	muls.w	(a4)+,d0
	asr.l	#$4,d0
	add.l	d1,d0
	add.l	d0,$3E(a5)
	movea.w	(a4)+,a0
	jmp	(a0)
