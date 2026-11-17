; ---------------------------------------------------------------------------
; Op5_LoopB - a5 op: counts loop counter B $1C(a6) down and branches back while it stays above 0; at 0 continues past the offset (the outer/inner twin of Op5_LoopA)
; In:  a5 = script: word branch offset
; Out: the branch or NEXT
; Evidence: subq.w #1,$1C / bgt Op5_Goto; script uses $21E92 (`6D10 FFF0`), $21F84; 16 corpus runs
; ---------------------------------------------------------------------------
Op5_LoopB:
	subq.w	#$1,$1C(a6)
	bgt.w	Op5_Goto
	clr.w	$1C(a6)
	addq.l	#$2,a5
	movea.w	(a5)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op5_CallNative - a5 op: calls the 68000 routine at the absolute address in the operand and continues with the next op
; In:  a5 = script: long routine address; a6 = wave object
; Out: NEXT
; Evidence: movea.l (a5)+,a0 / jsr (a0); script uses $21752 (`6D22 0002 40EE`), $21788 (`0002 40F6`); 12 corpus runs
; ---------------------------------------------------------------------------
Op5_CallNative:
	movea.l	(a5)+,a0
	jsr	(a0)
	movea.w	(a5)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op5_Gosub - a5 op: calls a wave-script subroutine at offset: pushes the pc onto the object's own return stack (pointer word $34(a6), stack from +$36) and branches
; In:  a5 = script: word offset (relative to itself)
; Out: continues in the subroutine
; Evidence: movea.w $34(a6),a0 / move.l a5,(a0)+ / adda.w (a5),a5; Wave_Start sets $34 = a6 + $36; 103 script uses, e.g. $21D62 (`6D2A 1F38`); 43 corpus runs
; ---------------------------------------------------------------------------
Op5_Gosub:
	movea.w	$34(a6),a0
	move.l	a5,(a0)+
	move.w	a0,$34(a6)
	adda.w	(a5),a5
	movea.w	(a5)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op5_Return - a5 op: returns from an Op5_Gosub: pops the pc from the object's return stack $34(a6) and continues after the call's offset word
; In:  a5 = script (no operands)
; Out: NEXT after the Op5_Gosub
; Evidence: movea.l -(a0),a5 / addq.l #2,a5; 150 script uses, e.g. $21E6A, $21EA4; 42 corpus runs
; ---------------------------------------------------------------------------
Op5_Return:
	movea.w	$34(a6),a0
	movea.l	-(a0),a5
	move.w	a0,$34(a6)
	addq.l	#$2,a5
	movea.w	(a5)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op5_RandomSwitch - a5 op: branches to one of n offsets picked at random (Rand_Below(n))
; In:  a5 = script: word n, n word offsets (each relative to itself)
; Out: the chosen branch
; Evidence: Rand_Below / add.w d0,d0 / adda then Op5_Goto; 60 script uses, e.g. $16DA2 (`6D4A 0002 FFB8 FFD8`), $17640; 48 corpus runs
; ---------------------------------------------------------------------------
Op5_RandomSwitch:
	move.w	(a5)+,d0
	bsr.w	Rand_Below
	add.w	d0,d0
	adda.w	d0,a5
; ---------------------------------------------------------------------------
; Op5_Goto - a5 op: relative goto (operand address + word offset); also the taken path of Op5_LoopA/B and Op5_RandomSwitch
; In:  a5 = script: word offset
; Out: continues at the target
; Evidence: adda.w (a5),a5 / NEXT; 149 script uses, e.g. $16992 (`6D54 FFAE`); 52 corpus runs
; ---------------------------------------------------------------------------
Op5_Goto:
	adda.w	(a5),a5
	movea.w	(a5)+,a0
	jmp	(a0)
