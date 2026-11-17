; ---------------------------------------------------------------------------
; Fx_SpawnAimedN - native callback: runs an a4 spawn script at a mirrored offset, heading = the object's heading rounded to one of N directions
; In:  a3 = motion block (+$C heading, 1024 per turn), a2 = script: dc.w N, dx, dy, dc.l a4 script
; Out: a2 past the arguments; a1-a6 preserved
; Evidence: d3 = round($C(a3) * N / 1024) * ($40000 / N) << 8, then Fx_SpawnAtOfsWithAngle; 26 script uses at $31024-$31162 (TAS f67569-f75321)
; ---------------------------------------------------------------------------
Fx_SpawnAimedN:
	move.w	(a2)+,d3
	move.l	#$40000,d0
	divu.w	d3,d0
	mulu.w	$C(a3),d3
	addi.l	#$200,d3
	lsr.l	#$8,d3
	lsr.w	#$2,d3
	mulu.w	d0,d3
	lsl.l	#$8,d3
	bra.s	Fx_SpawnAtOfsWithAngle
