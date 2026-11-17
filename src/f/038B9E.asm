; ---------------------------------------------------------------------------
; ToyFigure_SpawnRandCharge - spawner of a single toy figure with a randomly timed charge cycle (coroutine 3 $0387EE: random 1-2 s waits before each charge) and the pause-then-random-hop coroutine $038D9A; body $036BCE
; In:  d4/d5 = position (anchor)
; Out: jumps to Obj_SpawnEnemy with a2 = Scr2_038BAA
; Evidence: dc.l $038B9E in two chain shedding routines (Scr2_0386CC+$364, +$37A); in the corpus it ran (ctarget) but has no function-start frames in the TAS
; ---------------------------------------------------------------------------
ToyFigure_SpawnRandCharge:
	lea.l	(Scr2_038BAA).l,a2
	jmp	(Obj_SpawnEnemy).l
