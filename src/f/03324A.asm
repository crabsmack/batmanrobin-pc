; ---------------------------------------------------------------------------
; Stage2_RunFlight - section $2200 routine: starts stage 2's flying section from its beginning (music 9, loader script Tbl_03301E: section id $2200, corridor art, Flight_StartCorridor) and runs Flight_RunScene
; In:  -
; Out: never returns to the caller (the scene loop ends through Scene_FadeOutExit)
; Evidence: Tbl_00A0AC entry $00A0D8 (id $2200); TAS f35416, right after the lift boss (Section_StartExitN), png f036000 the corridor
; ---------------------------------------------------------------------------
Stage2_RunFlight:
	move.w	#$9,d0
	jsr	(Sound_PlayMusic).l
	pea.l	Tbl_03301E(pc)
	bra.s	Flight_RunScene
