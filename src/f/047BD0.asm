; ---------------------------------------------------------------------------
; Stage4_RunFreezeFight - section $4401 (the Mr. Freeze fight in the domed tower room): music $F, stage init, palettes, scene art, animated tiles (Tbl_0B29C6 at tile $3C6), triggers Tbl_041138 (Boss_Freeze_Spawn), Camera_LockHere, the cone raster; frame loop: tile animation, cones, V-scroll of both planes = g_ViewY
; In:  none
; Out: returns through the scene-exit path
; Evidence: Tbl_00A0AC maps $4401 to $047BD0; TAS f128749-130772 (png f129600, f130200: BOSS TIME)
; ---------------------------------------------------------------------------
Stage4_RunFreezeFight:
	move.w	#$F,d0
	jsr	(Sound_PlayMusic).l
	lea.l	Tbl_047B48(pc),a6
	jsr	(Scene_InitStage).l
	lea.l	(Tbl_015634).l,a0
	jsr	(Pal_LoadLine0).l
	lea.l	Tbl_047520(pc),a0
	jsr	(Pal_LoadLine3).l
	lea.l	Tbl_047B84(pc),a6
	jsr	(VM6_Run).l
	lea.l	(Tbl_0B29C6).l,a0
	lea.l	(g_LayerA).l,a6
	move.w	#$3C6,d0
	jsr	(Layer_SetAnimTiles).l
	move.w	#$4,$48(a6)
	lea.l	Tbl_041138(pc),a0
	jsr	(Trigger_LoadX).l
	jsr	(Camera_LockHere).l
	bsr.w	Stage4_InitConeRaster
loc_047C34:
	jsr	(Frame_BeginStage).l
	jsr	(Frame_RunWorld).l
	lea.l	(g_LayerA).l,a6
	jsr	(Layer_AnimateTiles).l
	bsr.w	Stage4_DrawCones
	move.w	(g_ViewY).w,d0
	move.w	d0,(g_VScrollA).w
	move.w	d0,(g_VScrollB).w
	jsr	(Frame_End).l
	bra.s	loc_047C34
; ---------------------------------------------------------------------------
; Scene_LoopSkippableDelayed - scene loop entry that first sets the skip delay g_SceneSkipDelay = $1E (Start ignored for 30 frames), then Scene_LoopSkippable
; In:  none
; Out: as Scene_LoopSkippable
; Evidence: callers: the attract scenes $049DA6, $049DEC, $049F62 (bra.w)
; ---------------------------------------------------------------------------
Scene_LoopSkippableDelayed:
	move.w	#$1E,(g_SceneSkipDelay).w
