; ---------------------------------------------------------------------------
; Boss_Harley_Spawn - trigger callback at the end of the stage 1 street: spawns the Harley mid-boss machine (boss class) running Scr2_018066
; In:  d4/d5 = trigger position
; Out: via Obj_SpawnBoss
; Evidence: lea Scr2_018066,a2 / jmp Obj_SpawnBoss; its long is the last entry of the street trigger table ($163B6 in Tbl_015F5A); TAS f4518 (BOSS TIME, then the balloon-topped cart rolls in: f4750); identified as Harley from the red/black jester that rises on the pole (f5850) and NOTES Game flow 'Harley mid-boss' (inferred)
; ---------------------------------------------------------------------------
Boss_Harley_Spawn:
	lea.l	(Scr2_018066).l,a2
	jmp	(Obj_SpawnBoss).l
