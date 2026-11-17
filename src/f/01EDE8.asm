; ---------------------------------------------------------------------------
; Boss_HarleyJet_SpawnFinal - claw-script native: spawns jetpack Harley for the final fight (Scr2_01EDF4: energy $700 / $C00, BossMeter_Show, score 5000)
; In:  d4/d5 = position from Spawn_AtCameraOfs (op $2CBC at $1E7F4, offset -$1E/$46)
; Out: as Obj_SpawnBoss
; Evidence: called from Scr2_01E778 +$82; TAS f11992, object $BEE8 energy $700 at f12060, the boss meter and BOSS TIME at f12420
; ---------------------------------------------------------------------------
Boss_HarleyJet_SpawnFinal:
	lea.l	(Scr2_01EDF4).l,a2
	jmp	(Obj_SpawnBoss).l
