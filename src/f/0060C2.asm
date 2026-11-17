; ---------------------------------------------------------------------------
; Op4_ForkRestAndEnd - a4 op: forks a child whose script is the rest of this one (the ops after this word) and ends this run, so a parent that resumes here emits one child per frame
; In:  a4 = script (no operands); d2-d5 = spawn registers
; Out: ends the run (falls into VM4_Fork with a3 = a4)
; Evidence: movea.l a4,a3 falling into VM4_Fork; 36 script uses, e.g. $14A1E (`60C2 6418 FFFF 0001 4A2C 634E 67EE`: the child sets its animation and loops Op4_LoadXY / Op4_Draw); 129 corpus runs
; ---------------------------------------------------------------------------
Op4_ForkRestAndEnd:
	movea.l	a4,a3
