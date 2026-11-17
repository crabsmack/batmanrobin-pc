; ---------------------------------------------------------------------------
; Enemy_PropToy_SpawnChain5RightC - spawner of a propeller toy entering from the right (H-flipped) with the 5-link chain ToyChain_MakeHanging5E, course coroutine $03AC06 at cruise $1AA, body $037E26
; In:  d4/d5 = position (a5 wave spawn entry, VM5_SpawnEntry's jsr (a0) $007016)
; Out: jumps to Obj_SpawnEnemy with a2 = its script
; Evidence: Scr2_03ABEE: Op2_SpawnChild $2A $03AEBA, Op2_ModSprAttr $F008, coroutine 0 $03AC06, Goto $037E26; the wave spawn entries at $035106, $03510E, $035116 and $03511E (x -$28 / $168) name it; no corpus run reaches it (raw-stubs 2026-10-30)
; ---------------------------------------------------------------------------
Enemy_PropToy_SpawnChain5RightC:
	lea.l	(Scr2_03ABEE).l,a2
	jmp	(Obj_SpawnEnemy).l
