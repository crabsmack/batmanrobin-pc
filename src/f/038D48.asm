; ---------------------------------------------------------------------------
; ToyFigure_SpawnG - spawner of a single toy figure at speed $100 with the standard charge cycle $0380B8 and hop coroutine $038D64, spawned by a toy ring's break-up; body $036BCE
; In:  d4/d5 = position (from the caller: Spawn_AtAnchor / Spawn_AtOwnOfs)
; Out: jumps to Obj_SpawnEnemy with a2 = Scr2_038D54
; Evidence: ToyFigure_Spawn's stub; named by the dc.l operand of $039628 (Op2_CallNative $008D52, a toy ring's break-up script); no corpus run; converted with ToyRing_SpawnSixB (widescreen_gameplay=1 reaches $038D48: scenario-level-skip sync 8810)
; ---------------------------------------------------------------------------
ToyFigure_SpawnG:
	lea.l	(Scr2_038D54).l,a2
	jmp	(Obj_SpawnEnemy).l
