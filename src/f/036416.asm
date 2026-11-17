; ---------------------------------------------------------------------------
; Enemy_PuffPlant_SpawnH - spawner of a puff plant (Scr2_036422, body $0355D4) that every $1E0 ticks opens and puffs 20 puffs (the puff script its vector 0 names), as Enemy_PuffPlant_SpawnA's $168-tick cycle of 10
; In:  d4/d5 = position (a5 wave spawn entry, VM5_SpawnEntry's jsr (a0) $007016)
; Out: jumps to Obj_SpawnEnemy with a2 = its script
; Evidence: Scr2_036422: StartCoroutine0Rel $03642A (wait $1E0, counter $14, SetVector0HereGoto), Goto the puff plant body $0355D4; the world-placed wave entries at $0354DC and $0354E4 (x $B6E / $BAC, y $20B2) name it; no corpus run reaches it (raw-stubs 2026-10-30)
; ---------------------------------------------------------------------------
Enemy_PuffPlant_SpawnH:
	lea.l	(Scr2_036422).l,a2
	jmp	(Obj_SpawnEnemy).l
