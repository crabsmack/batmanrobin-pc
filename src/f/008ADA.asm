; ---------------------------------------------------------------------------
; VIntObj_RunList - calls the handler of every object on the V-int object list with a6 = the object, until the tail sentinel's VIntObj_ListEnd returns for it
; In:  none (called inside the V-int)
; Out: returns through VIntObj_ListEnd
; Evidence: VInt_Handler calls it on both the full path ($7736) and VInt_BusyPath; handlers include Layer_FlushTileQueueIfIdle, Stage1_VIntFacade and VM6_VIntClearRows/BlitRows; the same 8x unrolled walk as Obj_RunList from g_VIntListHead; TAS 130141 frames (every V-int from f18)
; ---------------------------------------------------------------------------
VIntObj_RunList:
	lea.l	(g_VIntListHead).w,a6
loc_008ADE:
	movea.w	$6(a6),a6
	movea.l	(a6),a5
	jsr	(a5)
	movea.w	$6(a6),a6
	movea.l	(a6),a5
	jsr	(a5)
	movea.w	$6(a6),a6
	movea.l	(a6),a5
	jsr	(a5)
	movea.w	$6(a6),a6
	movea.l	(a6),a5
	jsr	(a5)
	movea.w	$6(a6),a6
	movea.l	(a6),a5
	jsr	(a5)
	movea.w	$6(a6),a6
	movea.l	(a6),a5
	jsr	(a5)
	movea.w	$6(a6),a6
	movea.l	(a6),a5
	jsr	(a5)
	movea.w	$6(a6),a6
	movea.l	(a6),a5
	jsr	(a5)
	bra.w	loc_008ADE
