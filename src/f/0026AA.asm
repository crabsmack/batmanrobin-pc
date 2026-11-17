; ---------------------------------------------------------------------------
; Child_A4OfHandle - shared tail of the Child_GetA4 helpers: turns the child a6 in d1 into its a4 block ($A of the child) and returns to the op
; In:  d1 = child a6 (word address)
; Out: a0 = $A(child) = the child's a4 block
; Evidence: movea.w d1,a0 / movea.w $A(a0),a0 / rts; a script object's a6 +$A holds its a4 block (Obj_InitSpawned); 32 runs
; ---------------------------------------------------------------------------
Child_A4OfHandle:
	movea.w	d1,a0
	movea.w	$A(a0),a0
	rts
