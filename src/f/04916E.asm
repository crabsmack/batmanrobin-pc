; ---------------------------------------------------------------------------
; Continue_Run - the continue screen after a game over (Mr. Freeze's head between two countdown monitors, CREDITS n): plain scene init + art, font tile base $E1C9, its controller object, unskippable loop; g_SceneResult 1 = continue
; In:  none
; Out: via Scene_LoopUnskippable; g_SceneResult
; Evidence: jsr from Main_RunSection after music $13 when the result is 2; Scr6_048EDE/048F4E, TextMap_LoadFont $E1C9; scenario-continue/game-over f41970 (png f42600 of scenario-game-over)
; ---------------------------------------------------------------------------
Continue_Run:
	lea.l	Scr6_048EDE(pc),a6
	jsr	(Scene_InitPlain).l
	lea.l	Scr6_048F4E(pc),a6
	jsr	(VM6_Run).l
	move.w	#$E1C9,d0
	jsr	(TextMap_LoadFont).l
	bsr.w	Continue_SpawnController
	bra.w	Scene_LoopUnskippable
