; ---------------------------------------------------------------------------
; GameOver_Run - scene routine of the GAME OVER screen: plain scene (Scr6_048EF0 header, Scr6_048F02 graphics), the director object and the falling GAME OVER text, then the Start-skippable scene loop
; In:  -
; Out: returns through the scene loop $47C6A (g_SceneResult 0 after 1000 frames, 1 on Start)
; Evidence: only caller Main_NoContinue; ran in scenario-game-over, x-die, x-die2 (f42807); png scenario-game-over f43200: GAME OVER over a snowy Gotham skyline
; ---------------------------------------------------------------------------
GameOver_Run:
	lea.l	Scr6_048EF0(pc),a6
	jsr	(Scene_InitPlain).l
	lea.l	Scr6_048F02(pc),a6
	jsr	(VM6_Run).l
	bsr.w	GameOver_SpawnDirector
	bsr.w	GameOver_SpawnText
	bra.w	Scene_LoopSkippable
