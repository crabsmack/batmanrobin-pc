; ---------------------------------------------------------------------------
; VIntObj_ListEnd - handler of the V-int list's tail sentinel: drops the jsr's return address so VIntObj_RunList returns to the V-int
; In:  called by VIntObj_RunList's jsr (a5)
; Out: returns to VIntObj_RunList's caller
; Evidence: VIntObj_InitList stores #VIntObj_ListEnd in g_VIntListTail; addq.l #4,a7 / rts as Obj_ListEnd; ctarget in all 153 corpus runs
; ---------------------------------------------------------------------------
VIntObj_ListEnd:
	addq.l	#$4,a7
	rts
