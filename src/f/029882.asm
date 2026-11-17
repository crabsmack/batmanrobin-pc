; ---------------------------------------------------------------------------
; Boss_BeamShip_BoltFrameByHeading - a4 native (switch): picks the ship's laser bolt sprite by its heading near due left: > $20C, > $204, > $1FC, > $1F4, else (5 cases: the bolt tilted by up to 2 steps either way)
; In:  a6 = the bolt; a4 = script: 5 word case offsets
; Out: a4 = the chosen case
; Evidence: op $5438 (Op4_CallNative) at $29840 in the bolt script $2980C, which the ship's sprite animations spawn (Op2_FxSpawnOff at $2972A-$2977C); cases draw frames $3D4A/$3D40/$3D36 (thin streaks, vertically flipped for two); TAS 77950-78522
; ---------------------------------------------------------------------------
Boss_BeamShip_BoltFrameByHeading:
	move.w	$2A(a6),d0
	andi.w	#$3FE,d0
	cmpi.w	#$20C,d0
	bgt.s	loc_0298AA
	addq.l	#$2,a4
	cmpi.w	#$204,d0
	bgt.s	loc_0298AA
	addq.l	#$2,a4
	cmpi.w	#$1FC,d0
	bgt.s	loc_0298AA
	addq.l	#$2,a4
	cmpi.w	#$1F4,d0
	bgt.s	loc_0298AA
	addq.l	#$2,a4
loc_0298AA:
	adda.w	(a4),a4
	rts
