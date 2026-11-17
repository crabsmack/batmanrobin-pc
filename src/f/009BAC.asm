; ---------------------------------------------------------------------------
; Frame_Begin - starts a non-gameplay frame: marks it busy, resets the DMA queue and sprite list and adds the scene's fixed sprites ($F808), without the HUD
; In:  none
; Out: g_FrameBusy set
; Evidence: the first three calls of Frame_BeginStage only; caller $47C6A (intro scene loop: Frame_Begin, Obj_RunList, Camera_UpdateView, Frame_End); 398 TAS frames f18-f116171
; ---------------------------------------------------------------------------
Frame_Begin:
	st.b	(g_FrameBusy).w
	jsr	DMA_ResetQueue(pc)
	jsr	Spr_ResetList(pc)
	jsr	(Spr_MaskLeftColumn).l
	rts
