; ---------------------------------------------------------------------------
; HUD_DrawPanelSprites - writes a player's HUD panel sprites straight into the SAT buffer: the Batman or Robin portrait plus 3 panel pieces, or 2 pieces in text mode
; In:  a6 = player record (+$C y, +$E x, +$25 bit 3 Robin, bit 7 text mode), a0 = 8-byte SAT-entry table (Tbl_011974 P1, Tbl_0119AC P2: portrait Batman, portrait Robin, 3 panel pieces, 2 text-mode pieces)
; Out: g_SATPtr and g_SprCount advanced (link field = running sprite count)
; Evidence: called by HUD_Update for both players every frame (TAS 532-130772, 129k frames); entries are y/size/attr/x words with the panel's position added to y and x; screenshot TAS f4800 shows the portrait and panel at the top left
; ---------------------------------------------------------------------------
HUD_DrawPanelSprites:
	movea.w	(g_SATPtr).w,a2
	move.w	$C(a6),d4
	move.w	$E(a6),d5
	move.w	(g_SprCount).w,d6
	btst	#$7,$25(a6)
	beq.s	loc_01193C
	lea.l	$28(a0),a0
	moveq	#$1,d7
	bra.s	loc_011954
loc_01193C:
	movea.l	a0,a1
	lea.l	$10(a0),a0
	btst	#$3,$25(a6)
	beq.s	loc_01194C
	addq.l	#$8,a1
loc_01194C:
	movem.w	(a1)+,d0-d3
	moveq	#$3,d7
	bra.s	loc_011958
loc_011954:
	movem.w	(a0)+,d0-d3
loc_011958:
	add.w	d4,d0
	add.w	d5,d3
	addq.b	#$1,d6
	move.b	d6,d1
	movem.w	d0-d3,(a2)
	addq.l	#$8,a2
	dbra	d7,loc_011954
	move.w	a2,(g_SATPtr).w
	move.w	d6,(g_SprCount).w
	rts
