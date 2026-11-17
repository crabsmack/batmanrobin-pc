; ---------------------------------------------------------------------------
; Op5_Wait - a5 op: waits t/16 frames (timer $2E = t * 16, counted down only while the population is below the cap), then continues
; In:  a5 = script: word t (1/16 frames: $F0 = 15 frames, $780 = 2 s)
; Out: yields (VM5_Yield)
; Evidence: asl.l #4 -> $2E / bra Op5_Yield; 858 script uses, e.g. $16946 (`6AC4 00F0`), $16956 (`6AC4 0780`); 104 corpus runs
; ---------------------------------------------------------------------------
Op5_Wait:
	moveq	#$0,d0
	move.w	(a5)+,d0
	asl.l	#$4,d0
	move.l	d0,$2E(a6)
	bra.s	VM5_Yield
; ---------------------------------------------------------------------------
; Op5_WaitRandom - a5 op: waits base + a random number below range (both in 1/16 frames), then continues
; In:  a5 = script: word range, word base
; Out: yields (VM5_Yield)
; Evidence: Rand_Below(range) << 4 + base << 4 -> $2E; script uses $3433A (`6AD0 0B40 0B40`), $350A4; 3 corpus runs
; ---------------------------------------------------------------------------
Op5_WaitRandom:
	move.w	(a5)+,d0
	bsr.w	Rand_Below
	asl.l	#$4,d0
	moveq	#$0,d1
	move.w	(a5)+,d1
	lsl.l	#$4,d1
	add.l	d1,d0
	move.l	d0,$2E(a6)
	bra.s	VM5_Yield
