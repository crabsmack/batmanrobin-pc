; ---------------------------------------------------------------------------
; ToyFigure_SpawnShed - spawner of a single toy figure shed from a chain, as ToyFigure_SpawnWithDrop without the drop gosub $03B11C: charge cycle $0380A8 (coroutine 3), big-hop coroutine $03A96E (coroutine 2), body $036BCE
; In:  d4/d5 = position (the chain anchor, from Spawn_AtAnchor)
; Out: jumps to Obj_SpawnEnemy with a2 = its script
; Evidence: Scr2_03A95E: SetSpeed $100, StartCoroutine3Rel $0380A8, StartCoroutine2Rel $03A96E, Goto $036BCE; dc.l of Op2_CallNative Spawn_AtAnchor ($008D52) at $03A946 in ToyFigure_SpawnWithDrop's script; no corpus run reaches it (raw-stubs 2026-10-30)
; ---------------------------------------------------------------------------
ToyFigure_SpawnShed:
	lea.l	(Scr2_03A95E).l,a2
	jmp	(Obj_SpawnEnemy).l
