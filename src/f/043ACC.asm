; ---------------------------------------------------------------------------
; Enemy_GasOrb_SpawnBossPairJets - spawns a gas orb during the Mr. Freeze fight, placed with a jitter and 150 px higher, with the two-jet / single-jet sweeps ($440FC / $44146, slower puffs)
; In:  d4/d5 = x/y
; Out: as Obj_SpawnEnemy
; Evidence: Scr2_043AD8: Op2_JitterPos $12C/$28, op $1640 (y move) $96, coroutine 1 $43B70, drop init $438DC, body $41E4A; spawner long in the Freeze fight scripts at $46F1E, $47004, $470A8, $470BC (Scr2_046D32 / Scr2_046F7C); scenario-level-skip frame 14527 only
; ---------------------------------------------------------------------------
Enemy_GasOrb_SpawnBossPairJets:
	lea.l	(Scr2_043AD8).l,a2
	jmp	(Obj_SpawnEnemy).l
