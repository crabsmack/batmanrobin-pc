; ---------------------------------------------------------------------------
; Stage3_SpawnRoadDriver - spawns the Hatter road's driver object Scr2_04061A (boss class): every frame it calls Stage3_RoadCameraFollow and Stage3_DrawRoad
; In:  -
; Out: a6 = the object (Obj_SpawnBoss)
; Evidence: Scr2_04061A: Op2_SetSpeed $100, Op2_CallNativeRelSaved $04062A, Op2_CallNativeRelSaved $0406CC, Op2_YieldGoto back; bsr from Stage3_InitHatterRoad; TAS f113311
; ---------------------------------------------------------------------------
Stage3_SpawnRoadDriver:
	lea.l	(Scr2_04061A).l,a2
	jmp	(Obj_SpawnBoss).l
