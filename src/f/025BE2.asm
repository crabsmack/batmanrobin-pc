; ---------------------------------------------------------------------------
; Boss_Stage2_Spawn - spawns the stage 2 arena boss director Scr2_025BEE (a boss-class object): it waits until no enemy is left, loads the boss art through the a6 loader script, spawns the four turrets and starts the fight
; In:  -
; Out: d0 = 0, a6 = the boss object
; Evidence: jmp from Stage2_LiftArriveTop ($0241CC) when the lift reaches the top; TAS f32065 (one frame); Scr2_025BEE: Op2_WaitPopBelow 1, Op2_StartA6Script $32ED6, four Op2_SpawnChildOrWait (fields $1E-$24); the boss is the machine hull above the arena (png f032400, drawn on plane B)
; ---------------------------------------------------------------------------
Boss_Stage2_Spawn:
	lea.l	(Scr2_025BEE).l,a2
	jmp	(Obj_SpawnBoss).l
