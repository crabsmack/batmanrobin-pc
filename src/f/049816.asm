; ---------------------------------------------------------------------------
; Title_PaletteFadeChannel - one channel of the title fade-in: after its start delay, every period frames it raises its brightness by 2 (-$E .. 0) and copies its colour range from the buffer into g_PalSource at that brightness (Pal_CopyBright), flagging the line for reload, so the logo's parts light up one after another
; In:  a5 = channel (+0 delay, +2 period count, +4 level, +6 record: period, reload-flag address, colours - 1, buffer offset, destination); a6 = the fade object (+$A buffer base)
; Out: a5 kept; d0-d7/a0-a4 trashed
; Evidence: subq delay / subq.w #1,2(a5) bcc / level bpl stops at 0 / addq.w #2 / st.b (flag) / jmp Pal_CopyBright; Tbl_04984C e.g. delay $2D, level -$10, 5+1 colours to $E114 (line 2), flag $F4D2 = g_Pal2Reload; idle f1172-f33094
; ---------------------------------------------------------------------------
Title_PaletteFadeChannel:
	tst.w	(a5)
	beq.s	loc_04981E
	subq.w	#$1,(a5)
loc_04981C:
	rts
loc_04981E:
	subq.w	#$1,$2(a5)
	bcc.s	loc_04981C
	move.w	$4(a5),d0
	bpl.s	loc_04981C
	addq.w	#$2,d0
	move.w	d0,$4(a5)
	movea.l	$6(a5),a4
	move.w	(a4)+,$2(a5)
	movea.w	(a4)+,a0
	st.b	(a0)
	move.w	(a4)+,d7
	movea.l	$A(a6),a0
	adda.w	(a4)+,a0
	movea.w	(a4)+,a1
	jmp	(Pal_CopyBright).l
