; ---------------------------------------------------------------------------
; ToyFigure_SpawnE - spawner of a single toy figure with coroutine 3 $03811E (as ToyFigure_SpawnD) and hop coroutine $038DE2; body $036BCE
; In:  d4/d5 = position (from the caller: Spawn_AtAnchor / Spawn_AtOwnOfs)
; Out: jumps to Obj_SpawnEnemy with a2 = Scr2_038BE6
; Evidence: ToyFigure_Spawn's stub; named by the dc.l operand of $038A04 (Op2_CallNative $008D52); no corpus run; converted with ToyRing_SpawnSixB (widescreen_gameplay=1 reaches $038D48: scenario-level-skip sync 8810)
; ---------------------------------------------------------------------------
ToyFigure_SpawnE:
	lea.l	(Scr2_038BE6).l,a2
	jmp	(Obj_SpawnEnemy).l
