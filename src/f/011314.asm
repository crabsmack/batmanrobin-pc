; ---------------------------------------------------------------------------
; HUD_StepBranches - HUD step jump table entries 4-$14: one bra each to HUD_StepLoadArt, HUD_StepP1Ring, HUD_StepP1Panel, HUD_StepP2Ring, HUD_StepP2Panel
; In:  a6 free
; Out: as the step taken
; Evidence: offsets 4..$14 of jmp $11310(pc,d0.w) at $1130C; the steps chain 4 -> 8 -> $C -> $10 -> $14 -> 8 through g_HUDStep
; ---------------------------------------------------------------------------
HUD_StepBranches:
	bra.w	HUD_StepLoadArt
; ---------------------------------------------------------------------------
; HUD_StepToP1Ring - HUD step jump table slot 8: on to HUD_StepP1Ring
; In:  none
; Out: as HUD_StepP1Ring
; Evidence: offset 8 of jmp $11310(pc,d0.w) at $1130C (HUD_Update), g_HUDStep = 8 set by HUD_StepLoadArt and HUD_StepP2Panel
; ---------------------------------------------------------------------------
HUD_StepToP1Ring:
	bra.w	HUD_StepP1Ring
; ---------------------------------------------------------------------------
; HUD_StepToP1Panel - HUD step jump table slot $C: on to HUD_StepP1Panel
; In:  none
; Out: as HUD_StepP1Panel
; Evidence: offset $C of jmp $11310(pc,d0.w) at $1130C (HUD_Update), g_HUDStep = $C set by HUD_StepP1Ring
; ---------------------------------------------------------------------------
HUD_StepToP1Panel:
	bra.w	HUD_StepP1Panel
; ---------------------------------------------------------------------------
; HUD_StepToP2Ring - HUD step jump table slot $10: on to HUD_StepP2Ring
; In:  none
; Out: as HUD_StepP2Ring
; Evidence: offset $10 of jmp $11310(pc,d0.w) at $1130C (HUD_Update), g_HUDStep = $10 set by HUD_StepP1Panel
; ---------------------------------------------------------------------------
HUD_StepToP2Ring:
	bra.w	HUD_StepP2Ring
; ---------------------------------------------------------------------------
; HUD_StepToP2Panel - HUD step jump table slot $14: on to HUD_StepP2Panel
; In:  none
; Out: as HUD_StepP2Panel
; Evidence: offset $14 of jmp $11310(pc,d0.w) at $1130C (HUD_Update), g_HUDStep = $14 set by HUD_StepP2Ring
; ---------------------------------------------------------------------------
HUD_StepToP2Panel:
	bra.w	HUD_StepP2Panel
; ---------------------------------------------------------------------------
; HUD_StepLoadArt - HUD step 4: queues the DMA of the HUD's fixed art (Art_058452, $2A0 bytes) to VRAM $0280, then moves on to step 8
; In:  none
; Out: g_HUDStep = 8 once the DMA was reserved; a full queue leaves it 0 (HUD_Update cleared it), so the step cycle stops on HUD_StepIdle (never seen in the corpus)
; Evidence: HUD_Init sets step 4; DMA regs $93509401/$952996C2/$9702 = $150 words from $58452, command $42800080 = VRAM write $0280; bne past the move.w #8,(g_HUDStep).w
; ---------------------------------------------------------------------------
HUD_StepLoadArt:
	moveq	#$1,d0
	moveq	#$15,d1
	jsr	(DMA_Reserve).l
	bne.s	loc_011354
	move.l	#$93509401,(a0)+
	move.l	#$952996C2,(a0)+
	move.w	#$9702,(a0)+
	move.l	#$42800080,(a0)+
	move.w	a0,(g_DMAQueuePtr).w
	move.w	#$8,(g_HUDStep).w
loc_011354:
	rts
; ---------------------------------------------------------------------------
; HUD_StepP1Ring - HUD step 8: when player 1's panel shows stats, draws the life ring (HUD_DrawLifeRing) and DMAs its 9 tiles from g_HUDTileBuf to VRAM $0520
; In:  none
; Out: g_HUDStep = $C
; Evidence: DMA $90 words from $FFEA24 ($951296F5/$977F), command $45200080 = VRAM $0520; skipped in text mode (+$25 bit 7)
; ---------------------------------------------------------------------------
HUD_StepP1Ring:
	lea.l	(g_Player1).l,a6
	btst	#$7,$25(a6)
	bne.s	loc_01138E
	bsr.w	HUD_DrawLifeRing
	moveq	#$1,d0
	moveq	#$A,d1
	jsr	(DMA_Reserve).l
	bmi.s	loc_01138E
	move.l	#$93909400,(a0)+
	move.l	#$951296F5,(a0)+
	move.w	#$977F,(a0)+
	move.l	#$45200080,(a0)+
	move.w	a0,(g_DMAQueuePtr).w
loc_01138E:
	move.w	#$C,(g_HUDStep).w
	rts
; ---------------------------------------------------------------------------
; HUD_StepP1Panel - HUD step $C: draws player 1's panel (pending text through HUD_DrawPanelText, or score + weapon bar) into g_HUDTileBuf and DMAs its 18 tiles to VRAM $0760
; In:  none
; Out: g_HUDStep = $10; the record's text pointer +$1A cleared once drawn
; Evidence: text mode (+$25 bit 7): only when +$1A is set; stats: HUD_DrawScore (d4=0), HUD_ClearTiles, HUD_DrawWeaponBar with Tbl_051792/Tbl_052512; DMA $120 words, command $47600080 = VRAM $0760
; ---------------------------------------------------------------------------
HUD_StepP1Panel:
	lea.l	(g_Player1).l,a6
	lea.l	(g_HUDTileBuf).l,a2
	btst	#$7,$25(a6)
	beq.s	loc_0113C8
	move.l	$1A(a6),d0
	beq.s	loc_011408
	lea.l	(Tbl_0586F2).l,a0
	lea.l	(Art_058AA2).l,a1
	movea.l	d0,a3
	bsr.w	HUD_DrawPanelText
	clr.l	$1A(a6)
	bra.s	loc_0113E2
loc_0113C8:
	moveq	#$0,d4
	bsr.w	HUD_DrawScore
	bsr.w	HUD_ClearTiles
	lea.l	(Tbl_051792).l,a0
	lea.l	(Tbl_052512).l,a1
	bsr.w	HUD_DrawWeaponBar
loc_0113E2:
	moveq	#$1,d0
	moveq	#$13,d1
	jsr	(DMA_Reserve).l
	bmi.s	loc_011408
	move.l	#$93209401,(a0)+
	move.l	#$951296F5,(a0)+
	move.w	#$977F,(a0)+
	move.l	#$47600080,(a0)+
	move.w	a0,(g_DMAQueuePtr).w
loc_011408:
	move.w	#$10,(g_HUDStep).w
	rts
; ---------------------------------------------------------------------------
; HUD_StepP2Ring - HUD step $10: player 2's life ring, as HUD_StepP1Ring but to VRAM $0640
; In:  none
; Out: g_HUDStep = $14
; Evidence: same DMA source/length as HUD_StepP1Ring, command $46400080 = VRAM $0640; a6 = g_Player2
; ---------------------------------------------------------------------------
HUD_StepP2Ring:
	lea.l	(g_Player2).l,a6
	btst	#$7,$25(a6)
	bne.s	loc_011448
	bsr.w	HUD_DrawLifeRing
	moveq	#$1,d0
	moveq	#$A,d1
	jsr	(DMA_Reserve).l
	bmi.s	loc_011448
	move.l	#$93909400,(a0)+
	move.l	#$951296F5,(a0)+
	move.w	#$977F,(a0)+
	move.l	#$46400080,(a0)+
	move.w	a0,(g_DMAQueuePtr).w
loc_011448:
	move.w	#$14,(g_HUDStep).w
	rts
; ---------------------------------------------------------------------------
; HUD_StepP2Panel - HUD step $14: player 2's panel, as HUD_StepP1Panel but right-aligned score, P2 bar tables and VRAM $09A0; then back to step 8
; In:  none
; Out: g_HUDStep = 8
; Evidence: HUD_DrawScore with d4=$60, HUD_DrawWeaponBar with Tbl_054812/Tbl_055592, command $49A00080 = VRAM $09A0; the P2 PRESS START text in the TAS comes from here
; ---------------------------------------------------------------------------
HUD_StepP2Panel:
	lea.l	(g_Player2).l,a6
	lea.l	(g_HUDTileBuf).l,a2
	btst	#$7,$25(a6)
	beq.s	loc_011482
	move.l	$1A(a6),d0
	beq.s	loc_0114CA
	lea.l	(Tbl_0586F2).l,a0
	lea.l	(Art_058AA2).l,a1
	movea.l	d0,a3
	bsr.w	HUD_DrawPanelText
	clr.l	$1A(a6)
	bra.s	loc_0114A4
loc_011482:
	move.w	#$60,d4
	bsr.w	HUD_DrawScore
	lea.l	(g_HUDTileBuf).l,a2
	bsr.w	HUD_ClearTiles
	lea.l	(Tbl_054812).l,a0
	lea.l	(Tbl_055592).l,a1
	bsr.w	HUD_DrawWeaponBar
loc_0114A4:
	moveq	#$1,d0
	moveq	#$13,d1
	jsr	(DMA_Reserve).l
	bmi.s	loc_0114CA
	move.l	#$93209401,(a0)+
	move.l	#$951296F5,(a0)+
	move.w	#$977F,(a0)+
	move.l	#$49A00080,(a0)+
	move.w	a0,(g_DMAQueuePtr).w
loc_0114CA:
	move.w	#$8,(g_HUDStep).w
	rts
