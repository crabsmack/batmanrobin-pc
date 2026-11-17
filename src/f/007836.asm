; ---------------------------------------------------------------------------
; DMA_ResetQueue - empties the VDP DMA queue and refills this frame's DMA budget
; In:  none
; Out: queue write pointer = $991C, 40 free slots, budget left = g_DMABudget ($9914)
; Evidence: called at every frame start ($9B86, $9BAC) and at scene init ($9AAC); 40 = the number of 8-byte steps in DMA_FlushQueue's unrolled copy ($785A-$799A)
; ---------------------------------------------------------------------------
DMA_ResetQueue:
	move.w	#$991C,(g_DMAQueuePtr).w
	move.w	#$28,(g_DMAQueueFree).w
	move.w	(g_DMABudget).w,(g_DMABudgetLeft).w
	rts
