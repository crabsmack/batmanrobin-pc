; ---------------------------------------------------------------------------
; Frame_BeginStage - starts a gameplay frame: marks the frame busy, resets the DMA queue and sprite list, the scene's fixed sprites ($F808), then the HUD and the shot-art keeper
; In:  none
; Out: g_FrameBusy set; HUD_Update and Weapon_KeepShotArt run; $15674 (an rts) called
; Evidence: first call of every section frame loop (16 callers $1FB3A-$47BD0: Frame_BeginStage, Frame_RunWorld, ..., Frame_End); 129430 TAS frames from f532
; ---------------------------------------------------------------------------
Frame_BeginStage:
	st.b	(g_FrameBusy).w
	jsr	DMA_ResetQueue(pc)
	jsr	Spr_ResetList(pc)
	jsr	(Spr_MaskLeftColumn).l
	jsr	(HUD_Update).l
	jsr	(Weapon_KeepShotArt).l
	jsr	(Frame_BeginStageHook).l
	rts
