; ---------------------------------------------------------------------------
; Boss_HarleyJet_Spawn - warehouse trigger callback: spawns Harley on her jetpack (boss class, Scr2_01EE64: energy $250 / $450 with 2 players, waits until fewer than 2 other enemies, flies in)
; In:  d4/d5 = x/y (trigger Tbl_016632 +$50)
; Out: as Obj_SpawnBoss
; Evidence: Scr2_01EE64: SetEnergy1P2P $250/$450, score $7D0, IncPopulation, WaitPopBelow 2, Camera_IsLocked switch; TAS f9904; shot f10860 (Harley with a jetpack, tagged Scr2_01EE64)
; ---------------------------------------------------------------------------
Boss_HarleyJet_Spawn:
	lea.l	(Scr2_01EE64).l,a2
	jmp	(Obj_SpawnBoss).l
