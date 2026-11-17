; ---------------------------------------------------------------------------
; Enemy_PropToy_SpawnChain4Right - spawner of a propeller toy entering from the right (H-flipped) with the 4-link chain ToyChain_MakeHanging4, course coroutine $03AB42 at cruise $155, body $037E26
; In:  d4/d5 = position (a5 wave spawn entry, VM5_SpawnEntry's jsr (a0) $007016)
; Out: jumps to Obj_SpawnEnemy with a2 = its script
; Evidence: Scr2_03AB2A: Enemy_PropToy_SpawnChain6Right's script with Op2_SpawnChild $2A $03AF4A and cruise $155 (coroutine 0 $03AB42); the wave spawn entry at $034730 (x $17C, y $E01A) names it; no corpus run reaches it (raw-stubs 2026-10-30)
; ---------------------------------------------------------------------------
Enemy_PropToy_SpawnChain4Right:
	lea.l	(Scr2_03AB2A).l,a2
	jmp	(Obj_SpawnEnemy).l
