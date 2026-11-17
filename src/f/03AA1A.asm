; ---------------------------------------------------------------------------
; Enemy_PropToy_SpawnChain8Left - spawner of a propeller toy entering from the left (not flipped) with the 8-link chain ToyChain_MakeHanging8, course coroutine $03AA3A, body $037E26 (population weight 5)
; In:  d4/d5 = position (wave spawn entry: x -$28, screen y $10)
; Out: jumps to Obj_SpawnEnemy with a2 = Scr2_03AA26
; Evidence: Scr2_03AA26: Op2_SpawnChild $2A dc.l $03AEEA, no Op2_ModSprAttr; entry `FFD8 E010 2903AA1A` (spawn action $29) at Scr2_03554C+$3C; TAS f97264
; ---------------------------------------------------------------------------
Enemy_PropToy_SpawnChain8Left:
	lea.l	(Scr2_03AA26).l,a2
	jmp	(Obj_SpawnEnemy).l
