; ---------------------------------------------------------------------------
; Op4_ForkAndEnd - a4 op: forks a child a4 object running the script at operand + offset and ends this run (Op4_Fork's child, Op4_ForkAbsAndEnd's end)
; In:  a4 = script: word offset to the child script; d2-d5 = spawn registers
; Out: ends the run (bra VM4_Fork with a3 = the child script)
; Evidence: movea.l a4,a3 / adda.w (a4)+,a3 / bra.s VM4_Fork; Scr4_036F88 at $036FA6; never run in the corpus; named at a script op position no corpus run read (tools/raw_reach.py, R27)
; ---------------------------------------------------------------------------
Op4_ForkAndEnd:
	movea.l	a4,a3
	adda.w	(a4)+,a3
	bra.s	VM4_Fork
