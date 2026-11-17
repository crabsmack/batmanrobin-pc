; ---------------------------------------------------------------------------
; Copyright_SpawnText - spawns the copyright notice object (script Scr2_049F94, boss class): ends the scene after 180 frames, draws the notice text once and kills itself
; In:  -
; Out: a6 = the object (Obj_SpawnBoss)
; Evidence: Scr2_049F94 is an a2 script: Op2_CallNative Scene_EndAfterDelay 0/$B4, Op2_Native $49FA0, then Op2_Kill $95C at $49FB6; TAS f164
; ---------------------------------------------------------------------------
Copyright_SpawnText:
	lea.l	(Scr2_049F94).l,a2
	jmp	(Obj_SpawnBoss).l
