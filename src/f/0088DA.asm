; ---------------------------------------------------------------------------
; Obj_LinkAtEnd - appends an already-built object a0 to the end of the main object list (just before g_ObjListTail), so Obj_RunList runs it from the next walk
; In:  a0 = object (pool block with its +0 handler set)
; Out: a0 +4 prev = the old last object, +6 next = g_ObjListTail; a1 = old last; a2 preserved (saved in usp)
; Evidence: only caller $29464: links $22(a6), an object made by Obj_SpawnScriptedUnlinked at $29250, later; TAS f79029 only; same insert as Obj_New without the pool pop. The raw $88F6-$8909 after it (never run) is the matching unlink of a0
; ---------------------------------------------------------------------------
Obj_LinkAtEnd:
	move.l	a2,usp
	lea.l	(g_ObjListTail).w,a2
	movea.w	$4(a2),a1
	move.w	a0,$4(a2)
	move.w	a0,$6(a1)
	movem.w	a1-a2,$4(a0)
	move.l	usp,a2
	rts
