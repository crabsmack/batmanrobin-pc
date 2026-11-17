; ---------------------------------------------------------------------------
; DMA_Reserve - reserves d0 DMA queue slots and d1 units of this frame's DMA budget, returning where to write the commands
; In:  d0 = queue entries, d1 = transfer cost
; Out: d0 = 0 and a0 = g_DMAQueuePtr on success (caller writes 14 bytes per entry and stores the new pointer); d0 = -1 with nothing reserved when either budget is short
; Evidence: callers $F808, $11314, $11C5E, $126D2, $144D4; $F808 calls it with d0=1, d1=8 for a $80-word transfer, writes $93809400/$9535967C/$9700/$7B000082 at a0 and stores a0 back to $991A
; ---------------------------------------------------------------------------
DMA_Reserve:
	sub.w	d1,(g_DMABudgetLeft).w
	bpl.s	DMA_ReserveSlots
	add.w	d1,(g_DMABudgetLeft).w
	moveq	#-$1,d0
	rts
; ---------------------------------------------------------------------------
; DMA_ReserveSlots - DMA_Reserve without the cost budget: reserves d0 DMA queue slots (also DMA_Reserve's second half)
; In:  d0 = queue entries
; Out: d0 = 0 and a0 = g_DMAQueuePtr on success; d0 = -1 (slots given back) when the queue is short
; Evidence: jsr $79AA with d0 = 1 from VM6_DMAStep (which keeps the budget itself); sub.w d0,(g_DMAQueueFree).w / bmi / movea.w (g_DMAQueuePtr).w,a0 / moveq #0,d0; 8 corpus runs
; ---------------------------------------------------------------------------
DMA_ReserveSlots:
	sub.w	d0,(g_DMAQueueFree).w
	bmi.s	loc_0079B8
	movea.w	(g_DMAQueuePtr).w,a0
	moveq	#$0,d0
	rts
loc_0079B8:
	add.w	d0,(g_DMAQueueFree).w
	moveq	#-$1,d0
	rts
