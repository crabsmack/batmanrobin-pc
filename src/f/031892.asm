; ---------------------------------------------------------------------------
; Enemy_PipeCannon_Spawn3Way - spawns a pipe cannon: it slides up and down a hull pipe between map y $C8 and $12C (coroutine 0) and three times, every $1E0 when on screen, fires a 3-way spread to the left (Fx_SpawnAtOfsAngle at (-8, 0), headings $240/$200/$1C0, a4 $31930), then swaps target and pauses $5A0
; In:  d4/d5 = trigger x/y (map space)
; Out: Obj_SpawnEnemy's result
; Evidence: trigger entry Tbl_0307C2 +$6E2 (drop byte $11); body Scr2_030416: owner $B, energy $110, score $64, moves in map space (Flight_AirshipLoadMapPos/StoreMapPos), frames by Enemy_PipeCannon_PipePhase; the first kill leaves it broken (frames $416E/$4178, energy $30) sliding down, the second explodes; TAS f69585
; ---------------------------------------------------------------------------
Enemy_PipeCannon_Spawn3Way:
	lea.l	(Scr2_03189E).l,a2
	jmp	(Obj_SpawnEnemy).l
