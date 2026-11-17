; ---------------------------------------------------------------------------
; Enemy_ArenaDebris_SpawnBounceAtX - spawner of a piece of arena debris that bounces twice on the floor (coroutine 0 $03B66A) as Enemy_ArenaDebris_SpawnBounce but at the wave entry's x (no Op2_JitterPos); body $03B5EC
; In:  d4/d5 = position (a5 wave spawn entry, VM5_SpawnEntry's jsr (a0) $007016)
; Out: jumps to Obj_SpawnEnemy with a2 = its script
; Evidence: Scr2_03C642: SetSpeed $100, StartCoroutine0Rel $03B66A, Goto $03B5EC; the wave spawn entries at $03C54E, $03C56C, $03C596 and $03C5B4 (x $A0, y $E070) in the cat-head arena's waves name it; no corpus run reaches it (raw-stubs 2026-10-30)
; ---------------------------------------------------------------------------
Enemy_ArenaDebris_SpawnBounceAtX:
	lea.l	(Scr2_03C642).l,a2
	jmp	(Obj_SpawnEnemy).l
