; ---------------------------------------------------------------------------
; Enemy_PropToy_SpawnChain5Right - spawner of a propeller toy entering from the right (H-flipped) with the 5-link chain ToyChain_MakeHanging5A, course $03AD7C (cruise x $200, waits, slows, leaves), body $037E26
; In:  d4/d5 = position (wave spawn entry: x $168, screen y $10)
; Out: jumps to Obj_SpawnEnemy with a2 = Scr2_03AC6E
; Evidence: Scr2_03AC6E: Op2_SpawnChild $2A dc.l $03AFA8, Op2_ModSprAttr $F008, Op2_Goto $03AD7C; entries in Scr5_034932+$98C (action $29) and Scr5_0352CA (+$1BE, +$202, +$276); TAS f94419-f95660 (4 spawns)
; ---------------------------------------------------------------------------
Enemy_PropToy_SpawnChain5Right:
	lea.l	(Scr2_03AC6E).l,a2
	jmp	(Obj_SpawnEnemy).l
