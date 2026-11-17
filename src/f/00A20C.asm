; ---------------------------------------------------------------------------
; BgTask_Resume - if a background task is active, continues it where the V-int parked it: interrupts off, g_BgTaskRunning set, context restored, fake RTE to its PC/SR
; In:  g_BgTaskContext
; Out: returns to the caller only when no task is active; otherwise the V-int (on g_BgTaskRunning) saves the task again and returns to Frame_End's wait loop
; Evidence: only caller Frame_End, right before it spins on g_VIntDone; movem.l context then move.l a4,-(a7) / move.w d0,-(a7) / rte; every frame from f18
; ---------------------------------------------------------------------------
BgTask_Resume:
	tst.b	(g_BgTaskActive).w
	bne.s	loc_00A214
	rts
loc_00A214:
	ori.w	#$700,sr
	st.b	(g_BgTaskRunning).w
	movem.l	(g_BgTaskContext).w,d0-d2/d6-d7/a0-a4
	move.l	a4,-(a7)
	move.w	d0,-(a7)
	rte
