; ---------------------------------------------------------------------------
; ToyFigure_SpawnCharging - spawner of a single toy figure that starts its charge cycle at once (coroutine 3 $0380F8: blink, charged about 1 s, calm 2 s, repeat) with the standard hops ($038CC2); body $036BCE
; In:  d4/d5 = position
; Out: jumps to Obj_SpawnEnemy with a2 = Scr2_038C92
; Evidence: dc.l $038C86 in a chain shedding routine (Scr2_0386CC+$294, Spawn_AtAnchor); TAS f93392-f93449 (2 frames)
; ---------------------------------------------------------------------------
ToyFigure_SpawnCharging:
	lea.l	(Scr2_038C92).l,a2
	jmp	(Obj_SpawnEnemy).l
