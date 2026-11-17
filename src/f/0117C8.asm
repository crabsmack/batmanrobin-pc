; ---------------------------------------------------------------------------
; HudOp_ShowStats - panel script op $1C: leaves text mode (clears +$25 bit 7) so the panel steps draw score, bar and ring again
; In:  a6 = player record
; Out: +$25 bit 7 cleared
; Evidence: entry $1C of Tbl_011764; first op of Tbl_010F84; HUD_StepP1Panel tests bit 7 to choose text or stats
; ---------------------------------------------------------------------------
HudOp_ShowStats:
	bclr	#$7,$25(a6)
	bra.s	HUD_PanelNext
; ---------------------------------------------------------------------------
; HudOp_ShowText - panel script op $18: shows a 12-character text (word wait, word offset from the op) by setting +$1A and text mode, then waits
; In:  a0 = operand pointer, a6 = player record
; Out: +$1E = wait, +$1A = text, +$25 bit 7 set, +$20 = next op
; Evidence: entry $18 of Tbl_011764; decoded texts ' GAME  OVER ', ' PRESS START', ' BOSS  TIME ', 'PLEASE WAIT ' and blanks at $10F38-$10F73; executed in 151 runs
; ---------------------------------------------------------------------------
HudOp_ShowText:
	move.w	(a0)+,$1E(a6)
	move.w	(a0)+,d0
	lea.l	-$6(a0,d0.w),a1
	move.l	a1,$1A(a6)
	bset	#$7,$25(a6)
	move.l	a0,$20(a6)
	rts
; ---------------------------------------------------------------------------
; HudOp_ShowCredits - panel script op $20: builds 'CREDIT' + g_Credits as 2 digits (max 99) in the text buffer $ED66, shows it like HudOp_ShowText and waits
; In:  a0 = operand pointer (wait word), a6 = player record
; Out: +$1E, +$1A = $ED66, +$25 bit 7 set, +$20 = next op
; Evidence: entry $20 of Tbl_011764; writes $43524544/$49542020 ('CREDIT  ') then the digits; used in the GAME OVER loops ($10F8A, $10FB2, $11012) and Tbl_010F76
; ---------------------------------------------------------------------------
HudOp_ShowCredits:
	move.w	(a0)+,$1E(a6)
	lea.l	(g_CreditText).l,a1
	move.l	a1,$1A(a6)
	bset	#$7,$25(a6)
	move.l	a0,$20(a6)
	move.l	#$43524544,(a1)+
	move.l	#$49542020,(a1)+
	move.w	(g_Credits).w,d0
	cmpi.w	#$63,d0
	bcs.s	loc_01181C
	move.w	#$63,d0
loc_01181C:
	divu.w	#$A,d0
	addi.b	#$30,d0
	move.b	d0,(a1)+
	swap	d0
	addi.b	#$30,d0
	move.b	d0,(a1)+
	move.w	#$2020,(a1)+
	rts
