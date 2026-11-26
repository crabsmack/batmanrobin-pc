; ---------------------------------------------------------------------------
; Op2_SetTimer1Rand - a2 op: sets timer 1 ($1C(a4)) to a random value below a range plus a base
; In:  a2 = script: word range, word base; a4 = object's a4 block
; Out: $1C(a4) = Rand_Below(range) + base; NEXT
; Evidence: move.w (a2)+,d0 / jsr Rand_Below / add.w (a2)+,d0 / move.w d0,$1C(a4), Op2_SetTimer1 with Op2_SetSpeedRand's operands; Scr2_0392A0 at $0392C2 (`0A40 012C 39BA 00C0 0F88 0780 0780`); never run in the corpus; named at a script op position no corpus run read (tools/raw_reach.py, R27)
; ---------------------------------------------------------------------------
Op2_SetTimer1Rand:
	move.w	(a2)+,d0
	jsr	(Rand_Below).l
	add.w	(a2)+,d0
	move.w	d0,$1C(a4)
	movea.w	(a2)+,a0
	jmp	(a0)
