; ---------------------------------------------------------------------------
; Op4_ForkOrBranch - a4 op: forks a child a4 object (VM4_Fork: script at operand + offset, motion from d2-d5) and continues; when the pool is full it branches instead
; In:  a4 = script: word child-script offset (from the operand), word branch offset taken on failure; d2-d5 = spawn radius/heading/x/y (16.16)
; Out: NEXT past the operands (a5 = the child) or the failure branch
; Evidence: a3 = a4 + offset, bsr VM4_Fork, beq Op4_SkipWord else adda.w (a4),a4; script uses $26D5C (`607C 001E 0002`), $37026 (`607C 0062 0044`); 67 corpus runs
; ---------------------------------------------------------------------------
Op4_ForkOrBranch:
	movea.l	a4,a3
	adda.w	(a4)+,a3
	bsr.w	VM4_Fork
	beq.w	Op4_SkipWord
	adda.w	(a4),a4
	movea.w	(a4)+,a0
	jmp	(a0)
