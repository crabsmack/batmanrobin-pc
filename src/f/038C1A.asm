; ---------------------------------------------------------------------------
; ToyFigure_SpawnStopHop - spawner of a single toy figure whose hop coroutine ($038C38) picks at random between big hops toward the players and small hops on the spot (cruise 0); standard charge cycle $0380B8; body $036BCE
; In:  d4/d5 = position (wave spawn entry or anchor)
; Out: jumps to Obj_SpawnEnemy with a2 = Scr2_038C26
; Evidence: Op2_SwitchRandom then Op2_JumpAnim $64/$240 or $14/$C0 with Op2_SetCruiseX 0; dc.l $038C1A in the wave script Scr5_034932 (7 entries) and Scr2_0386CC+$390; TAS f79422-f91136 (45 frames), scenario-level-skip f8083
; ---------------------------------------------------------------------------
ToyFigure_SpawnStopHop:
	lea.l	(Scr2_038C26).l,a2
	jmp	(Obj_SpawnEnemy).l
