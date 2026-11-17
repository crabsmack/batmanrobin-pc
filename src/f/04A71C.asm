; ---------------------------------------------------------------------------
; Ending_SpawnCellNarrator - spawns the Freeze-cell ending's narrator (Scr2_04A728, boss class): its coroutine (in Tbl_04A74A) types five text pages with fades, palette loads and the snow globe between them, then ends the scene; its loop runs Ending_CellTextStep every frame
; In:  none
; Out: via Obj_SpawnBoss
; Evidence: Scr2_04A728: SetSpeed $100, StartCoroutine0Rel -> $4A74E (TextMap_RunInline $4A7FE / Fade_StartOutBlack / StartA6Script $4A5A0 / TextMap_Clear / Fade_ToNormal / ... / Scene_EndAfterDelay 0); f15109
; ---------------------------------------------------------------------------
Ending_SpawnCellNarrator:
	lea.l	(Scr2_04A728).l,a2
	jmp	(Obj_SpawnBoss).l
