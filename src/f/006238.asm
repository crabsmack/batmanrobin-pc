; ---------------------------------------------------------------------------
; Op4_SpawnAtDrawPos - a4 op: sets the spawn position registers d4/d5 to the object's drawn point (16.16), so the next spawn starts where the object is drawn
; In:  a4 = script (no operands); a6 = object
; Out: NEXT; d4/d5 = drawn x/y 16.16
; Evidence: bsr VM4_CalcDrawPos only; script uses $14C3C (`6238 557A ...` then Op4_ForkRestAndEnd); 146 corpus runs
; ---------------------------------------------------------------------------
Op4_SpawnAtDrawPos:
	bsr.w	VM4_CalcDrawPos
	movea.w	(a4)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op4_CalcDrawPos - a4 op: computes the object's drawn point without moving it and leaves its pixel words in the low words of d4/d5 (as the motion ops do)
; In:  a4 = script (no operands); a6 = object
; Out: NEXT; d4/d5 = drawn x/y words (swapped)
; Evidence: bsr VM4_CalcDrawPos / swap d4 / swap d5; script use $302A2 (`6240 67D2`); 9 corpus runs
; ---------------------------------------------------------------------------
Op4_CalcDrawPos:
	bsr.w	VM4_CalcDrawPos
	swap	d4
	swap	d5
	movea.w	(a4)+,a0
	jmp	(a0)
