; ---------------------------------------------------------------------------
; Mem_Clear - zeroes d7 bytes at a0, 32 bytes per loop, then words, then a last odd byte
; In:  a0 = start, d7.w = byte count
; Out: a0 past the cleared area; d0 = 0, d6/d7 used
; Evidence: callers Scene_InitCommon (a0 = $FF8000, d7 = $75F8: all RAM below g_VIntCount $F5F8) and $3CC82; 26 TAS frames, all scene changes
; ---------------------------------------------------------------------------
Mem_Clear:
	moveq	#$0,d0
	move.w	d7,d6
	lsr.w	#$5,d7
	bra.s	loc_0099B6
loc_0099A6:
	move.l	d0,(a0)+
	move.l	d0,(a0)+
	move.l	d0,(a0)+
	move.l	d0,(a0)+
	move.l	d0,(a0)+
	move.l	d0,(a0)+
	move.l	d0,(a0)+
	move.l	d0,(a0)+
loc_0099B6:
	dbra	d7,loc_0099A6
	move.w	d6,d7
	andi.w	#$1F,d7
	lsr.w	#$1,d7
	bra.s	loc_0099C6
loc_0099C4:
	move.w	d0,(a0)+
loc_0099C6:
	dbra	d7,loc_0099C4
	andi.w	#$1,d6
	beq.s	loc_0099D2
	move.b	d0,(a0)+
loc_0099D2:
	rts
