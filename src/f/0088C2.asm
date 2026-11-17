; ---------------------------------------------------------------------------
; Obj_InitList - empties the main object list: links the head node g_ObjListHead straight to the tail sentinel g_ObjListTail and gives the sentinel the handler Obj_ListEnd
; In:  none
; Out: head +6 next = tail, tail +4 prev = head, tail +0 = Obj_ListEnd; a0/a1 = head/tail
; Evidence: only caller the common scene init $9AAC (next to VIntObj_InitList, ObjPool_Init and Hitbox_InitLists); Obj_RunList walks head +6 until the sentinel's handler pops its return; TAS 25 frames (one per scene)
; ---------------------------------------------------------------------------
Obj_InitList:
	lea.l	(g_ObjListHead).w,a0
	lea.l	(g_ObjListTail).w,a1
	move.w	a1,$6(a0)
	move.w	a0,$4(a1)
	move.l	#Obj_ListEnd,(a1)
	rts
