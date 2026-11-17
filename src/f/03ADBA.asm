; ---------------------------------------------------------------------------
; Enemy_PropToy_SpawnTwinChain6 - spawner of a propeller toy entering from the right (H-flipped) with two 6-link chains (children $2A ToyChain_MakeHanging6C, $2C ToyChain_MakeHanging6D), course coroutine $03ADEA, body $037E26
; In:  d4/d5 = position (a5 wave spawn entry, VM5_SpawnEntry's jsr (a0) $007016)
; Out: jumps to Obj_SpawnEnemy with a2 = its script
; Evidence: Scr2_03ADC6: SetAnim, Op2_SpawnChild $2A $03B064, SetAnim, Op2_SpawnChild $2C $03B092, Op2_ModSprAttr $F008, coroutine 0 $03ADEA; the wave spawn entry at $035126 (x $168, y $E002) names it; no corpus run reaches it (raw-stubs 2026-10-30)
; ---------------------------------------------------------------------------
Enemy_PropToy_SpawnTwinChain6:
	lea.l	(Scr2_03ADC6).l,a2
	jmp	(Obj_SpawnEnemy).l
