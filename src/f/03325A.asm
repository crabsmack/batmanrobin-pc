; ---------------------------------------------------------------------------
; Stage2_RunFlightClouds - section $2300 routine: restarts the flying section at the cloud bank (music 9, loader Scr6_033120: id $2300, corridor without waves, cloud art, Flight_StartClouds)
; In:  -
; Out: never returns (Flight_RunScene)
; Evidence: Tbl_00A0AC entry $00A0DE (id $2300); the $2200 loader sets id $2300 at the same point ($33050), so a continue/section skip resumes here; level-skip scenario f5145
; ---------------------------------------------------------------------------
Stage2_RunFlightClouds:
	move.w	#$9,d0
	jsr	(Sound_PlayMusic).l
	pea.l	Scr6_033120(pc)
	bra.s	Flight_RunScene
; ---------------------------------------------------------------------------
; Stage2_RunFlightAirship - section $2400 routine: restarts the flying section at the airship hull (loader Scr6_033182: id $2400, sky only, hull map, Flight_StartAirship); no Sound_PlayMusic call
; In:  -
; Out: never returns (Flight_RunScene)
; Evidence: Tbl_00A0AC entry $00A0E4 (id $2400); the $2200 run sets id $2400 at $3308A before the hull ($33398 at TAS f67262, png f067800/f072000: the purple hull scrolling past); runtime entry (ctarget) in the level-skip scenario
; ---------------------------------------------------------------------------
Stage2_RunFlightAirship:
	pea.l	Scr6_033182(pc)
	bra.s	Flight_RunScene
; ---------------------------------------------------------------------------
; Stage2_RunFlightBoss - section $2401 routine: restarts the flying section at the final beam-ship boss (loader Scr6_0331F0: id $2401, sky only, Flight_StartBeamShip); no Sound_PlayMusic call
; In:  -
; Out: never returns (Flight_RunScene)
; Evidence: Tbl_00A0AC entry $00A0EA (id $2401); the $2200 run sets id $2401 at $330CE before Flight_StartBeamShip (TAS f77184); runtime entry (ctarget) in the level-skip scenario
; ---------------------------------------------------------------------------
Stage2_RunFlightBoss:
	pea.l	Scr6_0331F0(pc)
