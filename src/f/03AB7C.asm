; ---------------------------------------------------------------------------
; Enemy_PropToy_SpawnTwinChain8 - spawner of a propeller toy entering from the right (H-flipped) with two 8-link chains (children $2A and $2C, both ToyChain_MakeHanging8B), course coroutine $03ABAC, body $037E26
; In:  d4/d5 = position (a5 wave spawn entry, VM5_SpawnEntry's jsr (a0) $007016)
; Out: jumps to Obj_SpawnEnemy with a2 = its script
; Evidence: Scr2_03AB88: SetAnim, Op2_SpawnChild $2A $03AF78, SetAnim, Op2_SpawnChild $2C $03AF78, Op2_ModSprAttr $F008, coroutine 0 $03ABAC; the wave spawn entry at $0347B8 (x $168, y $E010) names it; no corpus run reaches it (raw-stubs 2026-10-30)
; ---------------------------------------------------------------------------
Enemy_PropToy_SpawnTwinChain8:
	lea.l	(Scr2_03AB88).l,a2
	jmp	(Obj_SpawnEnemy).l
