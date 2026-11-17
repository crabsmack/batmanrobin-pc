; ---------------------------------------------------------------------------
; ToyFigure_SpawnD - spawner of a single toy figure with coroutine 3 $03811E (inside the standard charge cycle $0380B8-$038138) and hop coroutine $038DD8; body $036BCE
; In:  d4/d5 = position (from the caller: Spawn_AtAnchor / Spawn_AtOwnOfs)
; Out: jumps to Obj_SpawnEnemy with a2 = Scr2_038BC8
; Evidence: ToyFigure_Spawn's stub; named by the dc.l operand of $03890A, $038A88, $039642 (Op2_CallNative $008D52); no corpus run; converted with ToyRing_SpawnSixB (widescreen_gameplay=1 reaches $038D48: scenario-level-skip sync 8810)
; ---------------------------------------------------------------------------
ToyFigure_SpawnD:
	lea.l	(Scr2_038BC8).l,a2
	jmp	(Obj_SpawnEnemy).l
