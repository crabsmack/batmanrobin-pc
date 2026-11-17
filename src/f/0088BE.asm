; ---------------------------------------------------------------------------
; Obj_ListEnd - handler of the main object list's tail sentinel: drops the jsr return address so Obj_RunList itself returns
; In:  called by Obj_RunList's jsr (a5)
; Out: returns to Obj_RunList's caller
; Evidence: $88C2 stores #$88BE as the handler of the sentinel $DE94; body is addq.l #4,a7 / rts
; ---------------------------------------------------------------------------
Obj_ListEnd:
	addq.l	#$4,a7
	rts
