; ---------------------------------------------------------------------------
; VInt_Handler - vertical-blank interrupt: flushes the DMA queue, runs V-int objects, reads pads, ticks sound and fades, parks the background task
; In:  none (IRQ6); exits at once while g_VIntOff ($F6FC) is set
; Out: g_VIntDone ($F6FA) set, g_VIntCount ($F5F8) +1; when g_BgTaskRunning ($F4BD) was set, the interrupted SR/PC and d0-d2/d6-d7/a0-a4 go to the context at $E066 and the RTE returns to the task's caller
; Evidence: vector 30; runs every frame from frame 10 in the TAS. Full path: VDP reg1=$34, reg15=2, DMA_FlushQueue with a0=$C00004, $8ADA (object list $DE9C), pads $A3AE and sound $1E5044 under Z80 bus request, then andi #$F8FF,sr re-enables interrupts. When g_FrameBusy ($F6FB) is nonzero it takes VInt_BusyPath ($77FC): no DMA flush, no sound, no fade, pads via $A3D4
; ---------------------------------------------------------------------------
VInt_Handler:
	tst.b	(g_VIntOff).w
	bne.w	loc_0077FA
	movem.l	d0-d7/a0-a6,-(a7)
	lea.l	($C00004).l,a0
	lea.l	-$4(a0),a1
	lea.l	(g_VDPCmdBuf).l,a2
	move.w	#$8134,(a0)
	move.w	#$8F02,(a0)
	tst.b	(g_FrameBusy).w
	bne.w	VInt_BusyPath
	bsr.w	DMA_FlushQueue
	jsr	(VIntObj_RunList).l
	move.w	#$100,($A11100).l
	move.w	#$100,($A11200).l
loc_00777C:
	btst	#$0,($A11100).l
	bne.s	loc_00777C
	bsr.w	Pad_ReadAll
	move.w	#$0,($A11100).l
	bsr.w	Pal_VIntUpload
	bsr.w	VInt_UploadScroll
	addq.b	#$1,(g_FrameBusy).w
	andi.w	#$F8FF,sr
	move.w	#$100,($A11100).l
	move.w	#$100,($A11200).l
loc_0077B2:
	btst	#$0,($A11100).l
	bne.s	loc_0077B2
	jsr	(Snd_SendSfx).l
	move.w	#$0,($A11100).l
	move.b	(g_FadeLevel).w,(g_FadeLevelVInt).w
	bsr.w	Fade_Step
	st.b	(g_VIntDone).w
	addq.l	#$1,(g_VIntCount).w
	movem.l	(a7)+,d0-d7/a0-a6
	tst.b	(g_BgTaskRunning).w
	beq.s	loc_0077F6
	clr.b	(g_BgTaskRunning).w
	move.w	(a7)+,d0
	movea.l	(a7)+,a4
	movem.l	d0-d2/d6-d7/a0-a4,(g_BgTaskContext).w
	move.w	d0,-(a7)
loc_0077F6:
	subq.b	#$1,(g_FrameBusy).w
loc_0077FA:
	rte
VInt_BusyPath:
	jsr	(VIntObj_RunList).l
	bsr.w	VInt_BusyScroll
	move.w	#$100,($A11100).l
	move.w	#$100,($A11200).l
loc_007816:
	btst	#$0,($A11100).l
	bne.s	loc_007816
	bsr.w	Pad_ReadLive
	move.w	#$0,($A11100).l
	addq.l	#$1,(g_VIntCount).w
	movem.l	(a7)+,d0-d7/a0-a6
	rte
