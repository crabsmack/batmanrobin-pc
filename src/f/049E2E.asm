; ---------------------------------------------------------------------------
; ClockworkLogo_Spawn - spawns the Clockwork Tortoise logo object Scr2_049E3A: ends the scene after 230 frames, moves the picture vertically from y -$84 (cruise, then a hop and bounce in coroutine 0) and every 2 frames cycles line 3's colours
; In:  -
; Out: a6 = the object (Obj_SpawnEnemy)
; Evidence: Scr2_049E3A: Scene_EndAfterDelay 0/$E6, Op2_PlaceOnScreen 0/$FF7C, Op2_SetCruiseY, coroutine $49E72, timer 3 $20 -> Op2_CallNativeRel $49EA4, then $49E98 every frame; idle f3678
; ---------------------------------------------------------------------------
ClockworkLogo_Spawn:
	lea.l	(Scr2_049E3A).l,a2
	jmp	(Obj_SpawnEnemy).l
