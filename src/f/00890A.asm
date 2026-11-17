; ---------------------------------------------------------------------------
; Obj_NewMultiFail - failure exit of Obj_NewMulti / Obj_NewMultiUnlinked when the pool holds fewer than d7 blocks: gives the d7 back to g_ObjPoolFree and returns -1
; In:  d7 = blocks asked for (already subtracted from g_ObjPoolFree, which borrowed)
; Out: d0 = -1, g_ObjPoolFree restored, nothing allocated
; Evidence: entered by bcs from Obj_NewMulti ($8916), Obj_NewMultiUnlinked ($8988) and the raw insert-after variant at $899E; TAS f73410 once (pool full); unlike ObjPool_Alloc it restores the count instead of clearing it
; ---------------------------------------------------------------------------
Obj_NewMultiFail:
	add.b	d7,(g_ObjPoolFree).w
	moveq	#-$1,d0
	rts
