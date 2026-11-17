; ---------------------------------------------------------------------------
; Op4_ChildAddY - a4 op: adds a signed pixel word to the integer part of the child's y $32(a5)
; In:  a4 = script: word dy; a5 = child
; Out: NEXT
; Evidence: add.w (a4)+ to $32(a5); script uses $1F270, $25AFA; 20 corpus runs
; ---------------------------------------------------------------------------
Op4_ChildAddY:
	move.w	(a4)+,d0
	add.w	d0,$32(a5)
	movea.w	(a4)+,a0
	jmp	(a0)
