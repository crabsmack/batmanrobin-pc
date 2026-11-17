; ---------------------------------------------------------------------------
; Spr_QueueSATDMA - terminates the sprite link chain and queues the DMA of the sprite attribute buffer $FF9B56 to VRAM $0000
; In:  g_SprCount ($9DD6), g_SATPtr ($9B54)
; Out: one DMA queue entry (length = count*4 words); when no sprite was drawn it writes one empty 8-byte sprite first. Takes a queue slot but no budget; skipped when the queue is full
; Evidence: called once per frame from $9BF0 right after Spr_BuildSAT; source regs $95AB/$96CD/$977F = $FF9B56>>1, command $40000080 = VRAM write DMA at 0; clr.b -5(a3) zeroes the link byte of the last 8-byte entry
; ---------------------------------------------------------------------------
Spr_QueueSATDMA:
	subq.w	#$1,(g_DMAQueueFree).w
	bmi.s	loc_007A06
	movea.w	(g_DMAQueuePtr).w,a0
	movea.w	(g_SATPtr).w,a3
	move.w	(g_SprCount).w,d0
	bne.s	loc_0079DA
	moveq	#$1,d0
	clr.l	(a3)+
	clr.l	(a3)+
loc_0079DA:
	clr.b	-$5(a3)
	move.l	#$94009300,d1
	move.b	d0,d1
	add.b	d1,d1
	add.b	d1,d1
	swap	d1
	lsr.w	#$6,d0
	move.b	d0,d1
	move.l	d1,(a0)+
	move.l	#$95AB96CD,(a0)+
	move.w	#$977F,(a0)+
	move.l	#$40000080,(a0)+
	move.w	a0,(g_DMAQueuePtr).w
loc_007A06:
	rts
