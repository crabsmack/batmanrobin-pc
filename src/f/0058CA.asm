; ---------------------------------------------------------------------------
; Op4_ChildAddX - a4 op: adds a signed pixel word to the integer part of the last spawned child's x $2E(a5)
; In:  a4 = script: word dx; a5 = child
; Out: NEXT
; Evidence: add.w (a4)+ to $2E(a5); script uses $26C62, $26C76 (after a spawn); 3 corpus runs (fuzz only)
; ---------------------------------------------------------------------------
Op4_ChildAddX:
	move.w	(a4)+,d0
	add.w	d0,$2E(a5)
	movea.w	(a4)+,a0
	jmp	(a0)
