; ---------------------------------------------------------------------------
; Enemy_PropToy_SpawnChain8Right - spawner of a propeller toy (a fat figure with a propeller cap, flying with a toy-figure chain hanging below it) entering from the right (H-flipped), 8-link chain ToyChain_MakeHanging8, course coroutine $03A9CC (cruise x $200 / y $155, waypoints, then hovers), body $037E20 (population +1, energy $40, 50 points)
; In:  d4/d5 = position (wave spawn entry: x $168, screen y $10)
; Out: jumps to Obj_SpawnEnemy with a2 = Scr2_03A9B4
; Evidence: Scr2_03A9B4: Op2_SpawnChild $2A dc.l $03AEEA, Op2_ModSprAttr $F008; entry `0168 E010 2903A9A8` (spawn action $29) at Scr2_03554C+$34; TAS f96963; shot f97400: the propeller toy at screen (185,32) = this object (RAM f97380), the chain below it
; ---------------------------------------------------------------------------
Enemy_PropToy_SpawnChain8Right:
	lea.l	(Scr2_03A9B4).l,a2
	jmp	(Obj_SpawnEnemy).l
