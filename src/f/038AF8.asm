; ---------------------------------------------------------------------------
; ToyFigure_SpawnB - spawner of a single toy figure with coroutine 3 $038074 and its own hop coroutine $038B40 (random speed $33-$E6); body $036BCE
; In:  d4/d5 = position (from the caller: Spawn_AtAnchor / Spawn_AtOwnOfs)
; Out: jumps to Obj_SpawnEnemy with a2 = Scr2_038B04
; Evidence: ToyFigure_Spawn's stub; named by the dc.l operand of $036B94, $036BA8, $036BBC (Op2_CallNative $008D80); no corpus run; converted with ToyRing_SpawnSixB (widescreen_gameplay=1 reaches $038D48: scenario-level-skip sync 8810)
; ---------------------------------------------------------------------------
ToyFigure_SpawnB:
	lea.l	(Scr2_038B04).l,a2
	jmp	(Obj_SpawnEnemy).l
