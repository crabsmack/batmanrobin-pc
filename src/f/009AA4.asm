; ---------------------------------------------------------------------------
; Scene_InitPlain - starts a non-gameplay scene (title, menus, story, cards): common scene init from the a6 header, then starts the fade in
; In:  a6 = scene header (Scene_InitCommon layout)
; Out: as Scene_InitCommon, a6 past the header; fade target 0 from fully faded ($A9F6)
; Evidence: 22 callers, all scene routines at $484F6-$4A626 (attract, title $496A2, player select $49AA6, stage cards $4A180...); TAS 9 frames f10-f116145
; ---------------------------------------------------------------------------
Scene_InitPlain:
	bsr.w	Scene_InitCommon
	bra.w	Fade_StartIn
