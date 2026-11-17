; ---------------------------------------------------------------------------
; Enemy_PropToy_SpawnChain6Right - spawner of a propeller toy entering from the right (H-flipped) with the 6-link chain ToyChain_MakeHanging6B, course coroutine $03AAAC (cruise $1AA, two waypoint legs), body $037E26
; In:  d4/d5 = position (trigger entry: x $168, screen y $10)
; Out: jumps to Obj_SpawnEnemy with a2 = Scr2_03AA94
; Evidence: Scr2_03AA94: Op2_SpawnChild $2A dc.l $03AF1A, Op2_ModSprAttr $F008; dc.l $03AA88 in Tbl_034142+$4FC; ctarget in 1 corpus run, not in the TAS
; ---------------------------------------------------------------------------
Enemy_PropToy_SpawnChain6Right:
	lea.l	(Scr2_03AA94).l,a2
	jmp	(Obj_SpawnEnemy).l
