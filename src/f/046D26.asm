; ---------------------------------------------------------------------------
; Boss_Freeze_Spawn - trigger callback of the Freeze fight ($4401): spawns Mr. Freeze (boss class) with Scr2_046D32, which starts his movement/minion coroutine 0 ($046D40), places him at (300, 100) and jumps into his body at $04619C
; In:  trigger entry (Tbl_041138)
; Out: jumps to Obj_SpawnBoss
; Evidence: the only entry of Tbl_041138 (count 1; entry $04113A, callback long $046D26 at $041144); Scr2_046D32 = StartCoroutine0Rel, SetPos $12C $64, Goto $04619C (score 10000, energy $3000/$4000, BossMeter_Show); coroutine 0 calls Spawn_AtScreenCentre with the gas-orb spawners $043ACC/$043C74/$043D48; TAS f128791
; ---------------------------------------------------------------------------
Boss_Freeze_Spawn:
	lea.l	(Scr2_046D32).l,a2
	jmp	(Obj_SpawnBoss).l
