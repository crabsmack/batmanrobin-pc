; ---------------------------------------------------------------------------
; Boss_CatHead_FrameScroll - per-frame scroll hook of the cat-head arena scene: plane A scroll = view + the head's offset from ($A0, $68) (g_SceneVars+8/+$A), so the face picture follows the head object; then calls the current raster routine in g_SceneVars+0 (long, 0 = none)
; In:  g_SceneVars+0 routine, +8/+$A head x/y
; Out: g_HScrollA, g_VScrollA; whatever the raster routine writes
; Evidence: jsr from the arena scene loop $03FFBE ($040008); g_SceneVars+0 = $03BBBC / $03BC66 / $03BCFC set by the Start*Raster routines (RAM f88320: $0003BBBC); TAS f87285-f90392 (3108 frames)
; ---------------------------------------------------------------------------
Boss_CatHead_FrameScroll:
	move.w	(g_SceneVars+$8).w,d0
	subi.w	#$A0,d0
	add.w	(g_ViewX).w,d0
	move.w	d0,(g_HScrollA).w
	move.w	#$68,d0
	sub.w	(g_SceneVars+$A).w,d0
	add.w	(g_ViewY).w,d0
	move.w	d0,(g_VScrollA).w
	move.l	(g_SceneVars).w,d0
	beq.s	loc_03B948
	movea.l	d0,a0
	jmp	(a0)
loc_03B948:
	rts
; ---------------------------------------------------------------------------
; Boss_CatHead_SpawnRasterDriver - spawner of the cat head's raster driver (unlinked child $2E, Scr2_03B956): an invisible object whose position the Driver* natives copy into g_SceneVars for the curtain, sway and fall rasters
; In:  d4/d5 = position (from Op2_SpawnChildOrWait)
; Out: jumps to Obj_SpawnBossUnlinked with a2 = Scr2_03B956
; Evidence: Op2_SpawnChildOrWait $2E dc.l $03B94A at $03B22C; ctarget in 2 corpus runs; RAM f89640/f89880: the driver's y 192/91 matches the curtain's lower edge in the shots
; ---------------------------------------------------------------------------
Boss_CatHead_SpawnRasterDriver:
	lea.l	(Scr2_03B956).l,a2
	jmp	(Obj_SpawnBossUnlinked).l
