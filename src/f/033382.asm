; ---------------------------------------------------------------------------
; Flight_StartSkyOnly - loader-script native of the $2400/$2401 restarts: scroll mode $C and frame callback Flight_DrawSky (sky backdrop only, no corridor or cloud bank)
; In:  -
; Out: d0 = 0
; Evidence: `70BE 0003 3382` at $331B4 (Scr6_033182) and $3323C (Scr6_0331F0); level-skip scenario f5894-f6633
; ---------------------------------------------------------------------------
Flight_StartSkyOnly:
	move.w	#$C,d0
	jsr	(Scroll_SetMode).l
	move.l	#Flight_DrawSky,(g_SceneVars+$98).w
	moveq	#$0,d0
	rts
