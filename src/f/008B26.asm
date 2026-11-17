; ---------------------------------------------------------------------------
; VIntObj_InitList - empties the V-int object list: links g_VIntListHead straight to the tail sentinel g_VIntListTail and gives the sentinel the handler VIntObj_ListEnd
; In:  none
; Out: head +6 = tail, tail +4 = head, tail +0 = VIntObj_ListEnd
; Evidence: only caller the common scene init $9AAC, right before Obj_InitList; TAS 25 frames
; ---------------------------------------------------------------------------
VIntObj_InitList:
	lea.l	(g_VIntListHead).w,a0
	lea.l	(g_VIntListTail).w,a1
	move.w	a1,$6(a0)
	move.w	a0,$4(a1)
	move.l	#VIntObj_ListEnd,(a1)
	rts
