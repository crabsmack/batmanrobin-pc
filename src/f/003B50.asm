; ---------------------------------------------------------------------------
; Op2_GotoVector2Plus - a2 op: continues the script at script vector 2 plus a word displacement (an entry inside the block the vector points to)
; In:  a2 = script: word displacement
; Out: NEXT at $3A(a6) + displacement
; Evidence: movea.w (a2),a2 / adda.l $3A(a6),a2; the never-run $3B5A/$3B68 are the gosub forms for vectors 0/1; 16 corpus runs; $1B05C `3B50 0008`
; ---------------------------------------------------------------------------
Op2_GotoVector2Plus:
	movea.w	(a2),a2
	adda.l	$3A(a6),a2
	movea.w	(a2)+,a0
	jmp	(a0)
