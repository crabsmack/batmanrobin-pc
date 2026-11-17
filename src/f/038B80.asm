; ---------------------------------------------------------------------------
; ToyFigure_SpawnC - spawner of a single toy figure with the charge schedule $0387DA and the pause-then-random-hop coroutine $038D9A (ToyFigure_SpawnRandCharge's, which starts its schedule at $0387EE); body $036BCE
; In:  d4/d5 = position (from the caller: Spawn_AtAnchor / Spawn_AtOwnOfs)
; Out: jumps to Obj_SpawnEnemy with a2 = Scr2_038B8C
; Evidence: ToyFigure_Spawn's stub; named by the dc.l operand of $0388F0, $038A1A, $03965C (Op2_CallNative $008D52); no corpus run; converted with ToyRing_SpawnSixB (widescreen_gameplay=1 reaches $038D48: scenario-level-skip sync 8810)
; ---------------------------------------------------------------------------
ToyFigure_SpawnC:
	lea.l	(Scr2_038B8C).l,a2
	jmp	(Obj_SpawnEnemy).l
