; ---------------------------------------------------------------------------
; Enemy_PropToy_SpawnChain5RightB - spawner of a propeller toy entering from the right (H-flipped) with the 5-link chain ToyChain_MakeHanging5B (charge cycle $038810, no coroutine 0), course $03AD7C, body $037E26
; In:  d4/d5 = position (wave spawn entry: x $168, screen y $10)
; Out: jumps to Obj_SpawnEnemy with a2 = Scr2_03AC8E
; Evidence: Scr2_03AC8E: Op2_SpawnChild $2A dc.l $03AFD8, Op2_ModSprAttr $F008; entries in Scr5_034932+$994 and Scr5_0352CA (+$1C6, +$20A, +$27E); TAS f94297-f95872 (5 spawns)
; ---------------------------------------------------------------------------
Enemy_PropToy_SpawnChain5RightB:
	lea.l	(Scr2_03AC8E).l,a2
	jmp	(Obj_SpawnEnemy).l
