; ---------------------------------------------------------------------------
; Stage1_RunStreet - section $1100: the stage 1 street (bank of Gotham City): music 8, scene setup, palettes, the facade V-int object and fire frame list, line-scroll mode $C and the street triggers, then the frame loop with Stage1_UpdateStreetScroll
; In:  -
; Out: never returns normally (the scene ends through Scene_FadeOutExit)
; Evidence: Tbl_00A0AC maps $1100 here; Scene_InitStage Tbl_01FA76, a6 script Tbl_01FAB2 (VM6_Run), g_SceneVars = Tbl_015A7E, g_SceneVars+$E = g_BufPtrs+4 buffer + 4, Stage1_VIntFacade via VIntObj_New, Trigger_LoadX Tbl_015F5A; loop sets g_VScrollA = g_VScrollB = g_ViewY; TAS f503
; ---------------------------------------------------------------------------
Stage1_RunStreet:
	move.w	#$8,d0
	jsr	(Sound_PlayMusic).l
	lea.l	Tbl_01FA76(pc),a6
	jsr	(Scene_InitStage).l
	lea.l	(Tbl_015634).l,a0
	jsr	(Pal_LoadLine0).l
	lea.l	Tbl_01FA36(pc),a0
	jsr	(Pal_LoadLine2).l
	jsr	(Pal_LoadLine3).l
	lea.l	Tbl_01FAB2(pc),a6
	jsr	(VM6_Run).l
	move.l	#$15A7E,(g_SceneVars).w
	movea.l	(g_BufPtrs+$4).w,a0
	addq.l	#$4,a0
	move.l	a0,(g_SceneVars+$E).w
	lea.l	(Stage1_VIntFacade).l,a0
	jsr	(VIntObj_New).l
	move.w	#$C,(g_ScrollMode).w
	lea.l	(Tbl_015F5A).l,a0
	jsr	(Trigger_LoadX).l
loc_01FBA4:
	jsr	(Frame_BeginStage).l
	jsr	(Frame_RunWorld).l
	jsr	(Stage1_UpdateStreetScroll).l
	move.w	(g_ViewY).w,d0
	move.w	d0,(g_VScrollA).w
	move.w	d0,(g_VScrollB).w
	jsr	(Frame_End).l
	bra.s	loc_01FBA4
