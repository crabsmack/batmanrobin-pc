; ---------------------------------------------------------------------------
; VDP_LoadRegs - writes VDP registers 0-18 from a table of 19 bytes
; In:  a0 = 19 register values (reg 0 first)
; Out: a0 += 19; d0 = $93xx, d7 = -1
; Evidence: control words $8000 + reg*$100 + byte, 19 iterations; callers Main_Start (Tbl_0155BC: $04 $64 $30 $00 $07 ... reg 13 $2F, reg 16 $11) and Scene_InitCommon (the header's register-table long); TAS 26 frames
; ---------------------------------------------------------------------------
VDP_LoadRegs:
	moveq	#$12,d7
	move.w	#$8000,d0
loc_00AE32:
	move.b	(a0)+,d0
	move.w	d0,($C00004).l
	addi.w	#$100,d0
	dbra	d7,loc_00AE32
	rts
