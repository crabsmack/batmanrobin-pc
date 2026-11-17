; ---------------------------------------------------------------------------
; Title_BackFromOptions - Start on the options page: restarts the title song ($11, it may have been changed by the music test) and falls into the title menu script Scr2_049720 again
; In:  a2 script context (Op2_BranchVar0Any $80 at $49764 -> Op2_Native $49710)
; Out: jsr $7FE.w
; Evidence: Sound_PlayMusicAnyway d0 = $11; ran only in scenario-options (f1449), after which the menu runs again and START is chosen at f1770
; ---------------------------------------------------------------------------
Title_BackFromOptions:
	move.w	#$11,d0
	jsr	(Sound_PlayMusicAnyway).l
	jsr	($7FE).w
