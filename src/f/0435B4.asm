; ---------------------------------------------------------------------------
; Enemy_GasOrb_SpawnPairJets - spawns a gas orb for the stage 4 tube section whose attacks are the two-jet sweep ($440FC) or the single-jet sweeps ($44146), with the slower puffs (cruise y $A00)
; In:  d4/d5 = x/y (wave entry)
; Out: as Obj_SpawnEnemy
; Evidence: Scr2_0435C0: speed rand $19/$100, score $1F4, energy $1C0, coroutine 0 $435DE (one of three hover windows), coroutine 1 $4364E (random pick of the two vector-1 attacks), drop init $4399A, body $41E4A; wave entries at $41D04/$41D0C (Scr2_0417CC tube wave, x $168 / -$28); TAS 124188-127823 (15 spawns)
; ---------------------------------------------------------------------------
Enemy_GasOrb_SpawnPairJets:
	lea.l	(Scr2_0435C0).l,a2
	jmp	(Obj_SpawnEnemy).l
