; ---------------------------------------------------------------------------
; Scroll_SetMode - switches the V-int scroll upload to another mode with the V-int held off, turning H-ints off first
; In:  d0.w = mode (offset into VInt_ScrollModeTbl)
; Out: g_ScrollMode = d0; VDP reg 0 = $04 (H-int disabled), reg 10 = $FF
; Evidence: addq/subq g_VIntOff around `move.l #$80048AFF` and the store; callers in the stage-2 section code $033274, $033322, $033382, $033398, $033402 (jsr); 8 corpus runs, TAS 3 frames 35432-67262
; ---------------------------------------------------------------------------
Scroll_SetMode:
	addq.b	#$1,(g_VIntOff).w
	move.l	#$80048AFF,($C00004).l
	move.w	d0,(g_ScrollMode).w
	subq.b	#$1,(g_VIntOff).w
	rts
