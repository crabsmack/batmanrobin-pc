; ---------------------------------------------------------------------------
; BgTask_Start - starts the LZ decompressor as the background task unless one is active: saves a context whose PC is the decompressor and whose registers carry its arguments
; In:  a0 = packed data, a1 = destination (saved in the context with d1-d2/d6-d7/a2-a3)
; Out: d0 = 0 when started; $F4BC set; g_BgTaskContext = SR, d1-d2/d6-d7/a0-a3, a4 = $A23E
; Evidence: only caller the a6 op $7120 (jmp after setting $E03A = $713E, which waits on $F4BC); the decompressor $A23E clears $F4BC when it finishes; NOTES 'Background task and preemption'; 16 TAS frames
; ---------------------------------------------------------------------------
BgTask_Start:
	tst.b	(g_BgTaskActive).w
	bne.s	loc_00A20A
	st.b	(g_BgTaskActive).w
	lea.l	(Lz_Decompress).l,a4
	move.w	sr,d0
	movem.l	d0-d2/d6-d7/a0-a4,(g_BgTaskContext).w
	moveq	#$0,d0
loc_00A20A:
	rts
