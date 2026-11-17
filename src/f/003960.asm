; ---------------------------------------------------------------------------
; Obj_TestStruck - tests and clears the struck bit (bit 0 of +$3C) of the object's body boxes, stopping at the first one set
; In:  a4 = object's a4 block (+$2 count, +$4 handles)
; Out: d0 = 1 (Z clear) and g_StruckBox = the box when one was struck, else d0 = 0
; Evidence: bclr #0,$3C(a0) over the $4(a4) list; $3982 stores g_StruckBox; 151 runs
; ---------------------------------------------------------------------------
Obj_TestStruck:
	lea.l	$4(a4),a1
	move.w	$2(a4),d7
	subq.w	#$1,d7
	bmi.s	loc_00397E
loc_00396C:
	move.w	(a1)+,d0
	beq.s	loc_00397A
	movea.w	d0,a0
	bclr	#$0,$3C(a0)
	bne.s	Obj_SetStruckBox
loc_00397A:
	dbra	d7,loc_00396C
loc_00397E:
	moveq	#$0,d0
	rts
; ---------------------------------------------------------------------------
; Obj_SetStruckBox - the shared struck exit of Obj_TestStruck and Obj_TestBoxStruck: remembers the box and returns d0 = 1
; In:  a0 = the struck box
; Out: g_StruckBox = a0; d0 = 1 (Z clear)
; Evidence: move.l a0,(g_StruckBox).w / moveq #1,d0 / rts; the bne targets after bclr #0,$3C(a0) in both tests
; ---------------------------------------------------------------------------
Obj_SetStruckBox:
	move.l	a0,(g_StruckBox).w
	moveq	#$1,d0
	rts
