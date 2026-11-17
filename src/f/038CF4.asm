; ---------------------------------------------------------------------------
; ToyFigure_SpawnF - spawner of a single toy figure at speed $100 with the standard charge cycle $0380B8 and hop coroutine $038D10, spawned by a toy ring's break-up; body $036BCE
; In:  d4/d5 = position (from the caller: Spawn_AtAnchor / Spawn_AtOwnOfs)
; Out: jumps to Obj_SpawnEnemy with a2 = Scr2_038D00
; Evidence: ToyFigure_Spawn's stub; named by the dc.l operand of $03960A (Op2_CallNative $008D52, a toy ring's break-up script); no corpus run; converted with ToyRing_SpawnSixB (widescreen_gameplay=1 reaches $038D48: scenario-level-skip sync 8810)
; ---------------------------------------------------------------------------
ToyFigure_SpawnF:
	lea.l	(Scr2_038D00).l,a2
	jmp	(Obj_SpawnEnemy).l
