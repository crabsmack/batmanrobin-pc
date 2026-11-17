; ---------------------------------------------------------------------------
; PlayerSelect_SpawnCaption - spawns the PLAYER / SELECT caption object Scr2_049C48: starts at screen (25, $137) cruising up, eases to a stop after 30 frames, and draws both words every frame (PlayerSelect_DrawCaption)
; In:  -
; Out: a6 = the object (Obj_SpawnEnemy)
; Evidence: Scr2_049C48: Op2_PlaceOnScreen $19/$137, Op2_CruiseUp, coroutine $49C70 (wait $1E0, Op2_SetAccelYForDist, Op2_ClearTargetSpeedY), Op2_CallNativeRel $1E -> $49C88; TAS f431
; ---------------------------------------------------------------------------
PlayerSelect_SpawnCaption:
	lea.l	(Scr2_049C48).l,a2
	jmp	(Obj_SpawnEnemy).l
