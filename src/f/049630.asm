; ---------------------------------------------------------------------------
; GameOver_SpawnText - spawns the GAME OVER sprite text object Scr2_04963C: palette line 3, position ($5A, $FF), a jump-animated drop into place, then each frame draws 'GAME OVER' (Text_DrawAtObject) while keeping pace with the scrolling camera
; In:  -
; Out: a6 = the object (Obj_SpawnEnemy)
; Evidence: Scr2_04963C: Op2_LoadPalette3, Op2_SetPos $5A/$FF, Op2_JumpAnim, $4965A, Op2_ApplyMove, Op2_CallNative Text_DrawAtObject string $14088 'GAME OVER'; png scenario-game-over f43200
; ---------------------------------------------------------------------------
GameOver_SpawnText:
	lea.l	(Scr2_04963C).l,a2
	jmp	(Obj_SpawnEnemy).l
