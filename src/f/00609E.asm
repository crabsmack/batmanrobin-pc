; ---------------------------------------------------------------------------
; Op4_Fork - a4 op: forks a child a4 object (VM4_Fork) running the script at operand + offset; continues with the next op (child ops then edit a5); a full pool ends this run
; In:  a4 = script: word child-script offset; d2-d5 = spawn radius/heading/x/y
; Out: NEXT with a5 = child; rts (end of the run) when the pool is full
; Evidence: bsr VM4_Fork / beq VM4_Next / rts; 161 script uses, e.g. skid dust $10D9A (`609E 0022 5E3A FFCD FFCD`: then Op4_ChildRangeVelY), $14BD2; 151 corpus runs
; ---------------------------------------------------------------------------
Op4_Fork:
	movea.l	a4,a3
	adda.w	(a4)+,a3
	bsr.w	VM4_Fork
	beq.w	VM4_Next
	rts
; ---------------------------------------------------------------------------
; Op4_ForkAbs - a4 op: as Op4_Fork with the child script at an absolute address
; In:  a4 = script: long child-script address; d2-d5 = spawn registers
; Out: NEXT with a5 = child; rts when the pool is full
; Evidence: movea.l (a4)+,a3 / bsr VM4_Fork; script uses $28F74 (`60AC 0001 4B98`), $28716; 40 corpus runs
; ---------------------------------------------------------------------------
Op4_ForkAbs:
	movea.l	(a4)+,a3
	bsr.w	VM4_Fork
	beq.w	VM4_Next
	rts
; ---------------------------------------------------------------------------
; Op4_ForkAbsAndEnd - a4 op: forks a child running the script at an absolute address and ends this run (the parent resumes next frame from its saved pc $8)
; In:  a4 = script: long child-script address; d2-d5 = spawn registers
; Out: ends the run (VM4_Fork's rts)
; Evidence: movea.l (a4)+,a3 / bra VM4_Fork; script use $3B75A (`60B8 0001 4A20`); 3 corpus runs
; ---------------------------------------------------------------------------
Op4_ForkAbsAndEnd:
	movea.l	(a4)+,a3
	bra.s	VM4_Fork
