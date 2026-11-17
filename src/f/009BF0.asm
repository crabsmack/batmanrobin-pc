; ---------------------------------------------------------------------------
; Frame_End - finishes a frame and waits for the vertical blank: builds the SAT, releases the frame to the V-int, resumes the background decompressor while waiting, then pause check, RNG step, the scene's per-frame callback and the frame counters
; In:  none
; Out: g_FrameCounter +1, $F4B6 inverted (odd/even frame), $F4B7 += $40; with $F4F0 set it waits 2 more V-ints (slow motion)
; Evidence: Spr_BuildSAT, Spr_QueueSATDMA, $7086, clears g_VIntDone/g_FrameBusy/g_VIntOff, BgTask_Resume, spins on g_VIntDone, Pause_Update, Rand_Next, jsr (g_FrameCallback $F604); last call of all 17 scene loops; 129292 TAS frames
; ---------------------------------------------------------------------------
Frame_End:
	jsr	Spr_BuildSAT(pc)
	jsr	Spr_QueueSATDMA(pc)
	jsr	(VM6_RunHook).l
	clr.b	(g_VIntDone).w
	clr.b	(g_FrameBusy).w
	clr.b	(g_VIntOff).w
	jsr	(BgTask_Resume).l
loc_009C10:
	tst.b	(g_VIntDone).w
	beq.s	loc_009C10
	jsr	(Pause_Update).l
	jsr	Rand_Next(pc)
	movea.l	(g_FrameCallback).w,a0
	jsr	(a0)
	addq.l	#$1,(g_FrameCounter).w
	not.b	(g_FrameParity).w
	addi.b	#$40,(g_FrameQuarter).w
	tst.b	(g_SlowMotion).w
	beq.s	loc_009C4E
	st.b	(g_FrameBusy).w
	move.l	(g_VIntCount).w,d0
	addq.l	#$2,d0
loc_009C44:
	cmp.l	(g_VIntCount).w,d0
	bgt.s	loc_009C44
	sf.b	(g_FrameBusy).w
loc_009C4E:
	rts
