; ---------------------------------------------------------------------------
; Boss_Stage2_EndPlayerTilts - native fragment of the boss's phase-2 coroutine: clears g_Stage2TiltAllowed (Stage2_ArenaRequestTilt now refuses), then the coroutine forces a tilt itself every $690 frames (op $2CB2 into $0241DE, past that check)
; In:  a2 script context
; Out: jsr $7FE.w into $025D32
; Evidence: clr.b g_Stage2TiltAllowed; Scr2_025D32: Op2_SetTimer3 $690, tick loop, op $2CB2 offset $E49C = $0241DE (inside Stage2_ArenaRequestTilt after its tests); TAS f33526 (one frame)
; ---------------------------------------------------------------------------
Boss_Stage2_EndPlayerTilts:
	clr.b	(g_Stage2TiltAllowed).w
	jsr	($7FE).w
