; ---------------------------------------------------------------------------
; Boss_JokerBalloon_SpawnThrown - spawns a 5-block a2 object (VM2_RunObject) running script Tbl_01DE8A[d6] (11 kinds, each thrown left with its own speed and an arcing Op2_JumpAnim) at d4/d5 (16.16)
; In:  d4/d5 = x/y longs (16.16), d6 = the kind x 4
; Out: d0 = 0 and a4/a6 = the new object's blocks; nonzero d0 when the pool is short
; Evidence: bsr from Boss_JokerBalloon_ThrowByArg; lea VM2_RunObject,a0 / moveq #5,d7 / jsr Obj_Spawn / bne / move.l $1DE8A(pc,d6.w),$2A(a4) / x/y longs; raw until crane-crash (2026-10-30): no corpus run reaches it
; ---------------------------------------------------------------------------
Boss_JokerBalloon_SpawnThrown:
	lea.l	(VM2_RunObject).l,a0
	moveq	#$5,d7
	jsr	(Obj_Spawn).l
	bne.s	loc_01DE88
	move.l	Tbl_01DE8A(pc,d6.w),$2A(a4)
	move.l	d4,$3E(a6)
	move.l	d5,$42(a6)
loc_01DE88:
	rts
