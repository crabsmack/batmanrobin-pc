; ---------------------------------------------------------------------------
; Enemy_HullTurret_SpawnDelayed - spawns an aiming hull gun turret whose start delay (timer 0) comes from the trigger entry; it fires 3 aimed shots $1E0 apart, swaps target and repeats
; In:  d4/d5 = trigger x/y; a1 = the entry's extra word (start delay)
; Out: as Spawn_SetTimer0FromEntry (d0 = 0); pops one return address when the pool is short
; Evidence: the most common entry of Tbl_0307C2 (57 entries, 24 of them with drop byte $2B); body $2F98C, coroutine 0 $2F9EC aims; jmp Spawn_SetTimer0FromEntry; TAS 67325-75173 (35). Raw, never run: variant spawners at $03129A (Scr2_0312AC) and $03135C (Scr2_031368)
; ---------------------------------------------------------------------------
Enemy_HullTurret_SpawnDelayed:
	lea.l	(Scr2_03130E).l,a2
	jsr	(Obj_SpawnEnemyOrQuit).l
	jmp	(Spawn_SetTimer0FromEntry).l
