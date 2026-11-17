; ---------------------------------------------------------------------------
; PlayerSelect_ScrollHalves - player-select per-frame native: steps the line tint of the chosen hero's palette line (1 Robin, 0 Batman: the confirm flashes) and fills plane A's line H-scroll with the object's x for the top 112 lines (Batman) and its y word for the bottom 112 (Robin), so each half slides on its own
; In:  a6 = the chooser (x $3E, y $42 used as two scroll positions)
; Out: g_HScrollLinesA filled (Mem_FillWords2)
; Evidence: Op2_CallNativeRel from $49B52 every frame; Tint_StepLine1/Tint_StepLine0 by g_P1IsRobin; a1 = a0 + $E0, d7 = $70; TAS f431-f476, scenario-robin f485-f614
; ---------------------------------------------------------------------------
PlayerSelect_ScrollHalves:
	tst.b	(g_P1IsRobin).w
	beq.s	loc_049AE2
	jsr	(Tint_StepLine1).l
	bra.s	loc_049AE8
loc_049AE2:
	jsr	(Tint_StepLine0).l
loc_049AE8:
	move.w	$3E(a6),d0
	move.w	$42(a6),d2
	lea.l	(g_HScrollLinesA).l,a0
	lea.l	$E0(a0),a1
	move.w	#$70,d7
	jmp	(Mem_FillWords2).l
; ---------------------------------------------------------------------------
; PlayerSelect_ReadPad - player-select native: packs pad 1's newly pressed byte (high) and held byte (low) into script variable 0 for the chooser's branches
; In:  a4 = the chooser's a4 block
; Out: $22(a4) = g_Pad1+2 << 8 | g_Pad1
; Evidence: Op2_CallNativeRel $FFC2 from $49B40 every frame; the coroutine at $49B5A tests var 0 & $8000 (Start) and & $7300 (A/B/C, Up/Down); runtime entry (ctarget) in the TAS
; ---------------------------------------------------------------------------
PlayerSelect_ReadPad:
	lea.l	(g_Pad1).l,a0
	move.b	$2(a0),d0
	lsl.w	#$8,d0
	move.b	(a0),d0
	move.w	d0,$22(a4)
	rts
