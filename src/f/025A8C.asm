; ---------------------------------------------------------------------------
; Enemy_PoleThug_SlideSparks - pole-thug native: while it slides down faster than 0.5 px/frame ($1C(a3) > $8000) spawns the a4 effect Scr4_025AC0 at its position + the two inline words (dx mirrored by the H-flip bit)
; In:  a2 = inline dx, dy words, a3/a5/a6 = thug
; Out: a2 past the two words (skipped unspawned when slower)
; Evidence: op $2CB2 at $025818 (dx $E, dy $18) in the motion coroutine $025808 after Op2_MoveTerrain; Scr4_025AC0 spawns small particles with random x speed (sparks from the slide: appearance inferred); TAS f21284-f29651 (69 frames)
; ---------------------------------------------------------------------------
Enemy_PoleThug_SlideSparks:
	cmpi.l	#$8000,$1C(a3)
	bgt.s	loc_025A9A
	addq.l	#$4,a2
	rts
loc_025A9A:
	move.w	(a2)+,d4
	move.w	(a2)+,d5
	btst	#$3,$38(a5)
	beq.s	loc_025AA8
	neg.w	d4
loc_025AA8:
	add.w	$3E(a6),d4
	add.w	$42(a6),d5
	swap	d4
	swap	d5
	lea.l	(Scr4_025AC0).l,a0
	jmp	(Fx_RunScript).l
