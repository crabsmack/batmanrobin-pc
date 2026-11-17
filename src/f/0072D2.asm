; ---------------------------------------------------------------------------
; VM6_StartClearJob - a6: starts the plane-fill V-int job: allocates a V-int list object (retrying each frame while the pool is full) with handler VM6_VIntClearRows, and parks the script
; In:  g_A6TileWord / g_A6VdpCmd / g_A6RectCols / g_A6RectRows set up by VM6_SetupClearRect
; Out: parks: g_A6ResumeHook = 0 (or $72D2 to retry), g_A6VIntJob = the object
; Evidence: jsr $8BB8 (pool block on the V-int list $DE9C, handler $8BEC); move.l #$7302,(a6); 2 corpus runs (TAS f32080, f77148)
; ---------------------------------------------------------------------------
VM6_StartClearJob:
	jsr	(VIntObj_NewIdle).l
	beq.s	loc_0072E4
	move.l	#$72D2,(g_A6ResumeHook).w
	rts
loc_0072E4:
	clr.l	(g_A6ResumeHook).w
	move.w	a6,(g_A6VIntJob).w
	move.l	#$7302,(a6)
	rts
; ---------------------------------------------------------------------------
; VM6_EndClearJob - a6 resume hook after a plane fill: frees the V-int job object and resumes the script
; In:  g_A6VIntJob = the job object
; Out: VM6_Resume
; Evidence: hook value $72F4 set by VM6_VIntClearRows when the last row is written; jsr $8BF6 (unlinks the block from the V-int list and frees it); 2 corpus runs
; ---------------------------------------------------------------------------
VM6_EndClearJob:
	movea.w	(g_A6VIntJob).w,a6
	jsr	(VIntObj_UnlinkFree).l
	bra.w	VM6_Resume
; ---------------------------------------------------------------------------
; VM6_VIntClearRows - V-int list handler of the plane-fill job: writes as many rows of the fill tile as the DMA budget allows straight to the VDP; after the last row it idles and hands the end to Frame_End
; In:  a6 = job object, a0 = $C00004, a1 = $C00000 (V-int registers); g_A6TileWord, g_A6VdpCmd, g_A6RectCols, g_A6RectRows
; Out: g_A6VdpCmd advanced by rows written; when done: handler = $7364 (rts), g_A6ResumeHook = VM6_EndClearJob
; Evidence: run from $8ADA in the V-int; skips while g_FrameBusy; rows this V-int = g_DMABudgetLeft * 1.5 / (cols + 1); entered by the V-int object walk in 2 corpus runs
; ---------------------------------------------------------------------------
VM6_VIntClearRows:
	tst.b	(g_FrameBusy).w
	bne.s	VM6_ClearJobIdle
	moveq	#$0,d7
	move.w	(g_DMABudgetLeft).w,d7
	move.w	d7,d6
	asr.w	#$1,d6
	add.w	d6,d7
	move.w	(g_A6RectCols).w,d6
	addq.w	#$1,d6
	divu.w	d6,d7
	subq.w	#$1,d7
	bmi.s	VM6_ClearJobIdle
	move.w	(g_A6TileWord).w,d0
	move.l	(g_A6VdpCmd).w,d1
	moveq	#$0,d2
	move.w	(g_PlaneRowBytes).w,d2
	lsl.l	#$2,d2
loc_007330:
	move.l	d1,(a0)
	move.w	(g_A6RectCols).w,d6
loc_007336:
	move.w	d0,(a1)
	dbra	d6,loc_007336
	swap	d1
	rol.w	#$2,d1
	add.l	d2,d1
	ror.w	#$2,d1
	swap	d1
	subq.w	#$1,(g_A6RectRows).w
	dbmi	d7,loc_007330
	bmi.s	loc_007356
	move.l	d1,(g_A6VdpCmd).w
	rts
loc_007356:
	move.l	#$7364,(a6)
	move.l	#$72F4,(g_A6ResumeHook).w
; ---------------------------------------------------------------------------
; VM6_ClearJobIdle - V-int list handler of a finished plane-fill job: does nothing (an rts) until Frame_End's VM6_EndClearJob frees the object
; In:  a6 = the job object
; Out: rts
; Evidence: move.l #$7364,(a6) in VM6_VIntClearRows after the last row; also its early exits (g_FrameBusy set, no row fits the budget); 2 corpus runs
; ---------------------------------------------------------------------------
VM6_ClearJobIdle:
	rts
