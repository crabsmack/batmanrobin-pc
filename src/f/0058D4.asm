; ---------------------------------------------------------------------------
; Op4_ChildAddXY - a4 op: adds signed pixel words to the integer parts of the child's x and y ($2E/$32 of a5)
; In:  a4 = script: word dx, word dy; a5 = child
; Out: falls into Op4_ChildAddY, then NEXT
; Evidence: add.w (a4)+ to $2E(a5) with no NEXT, so the next file's add to $32(a5) follows; script use $49544; 1 corpus run
; ---------------------------------------------------------------------------
Op4_ChildAddXY:
	move.w	(a4)+,d0
	add.w	d0,$2E(a5)
