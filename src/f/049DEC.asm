; ---------------------------------------------------------------------------
; SegaLogo_RotatePalette - rotates colours 8-15 of palette line 0 by one place (15 moves to 8) and flags the line for reload: the running shine on the SEGA logo
; In:  -
; Out: g_PalSource line 0 colours 8-15 rotated; g_Pal0Reload set
; Evidence: a0 = g_PalSource+$1E, 7 x move.w -(a0),2(a0), old colour 15 to colour 8; called every 3rd frame from Scr2_049DCE; TAS f18-f155 (47 frames)
; ---------------------------------------------------------------------------
SegaLogo_RotatePalette:
	st.b	(g_Pal0Reload).w
	lea.l	(g_PalSource+$1E).l,a0
	move.w	(a0),d0
	moveq	#$6,d7
loc_049DFA:
	move.w	-(a0),$2(a0)
	dbra	d7,loc_049DFA
	move.w	d0,(a0)
	rts
; ---------------------------------------------------------------------------
; ClockworkLogo_Run - scene routine of the Clockwork Tortoise (developer) logo, last scene of the attract story list: plain scene (Tbl_049CEE, Scr6_049D68), palette line 3 from Tbl_049ED2, the logo object, then the scene loop $47C64
; In:  -
; Out: returns after 230 frames or on Start
; Evidence: last entry of Tbl_00A136 (Main_Title's story list); idle f3678; png w31-idle f3720/f3840: the wind-up tortoise under the CLOCKWORK TORTOISE INC. lettering
; ---------------------------------------------------------------------------
ClockworkLogo_Run:
	lea.l	Tbl_049CEE(pc),a6
	jsr	(Scene_InitPlain).l
	lea.l	Scr6_049D68(pc),a6
	jsr	(VM6_Run).l
	lea.l	(Tbl_049ED2).l,a0
	jsr	(Pal_LoadLine3).l
	bsr.w	ClockworkLogo_Spawn
	bra.w	Scene_LoopSkippableDelayed
