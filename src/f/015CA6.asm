; ---------------------------------------------------------------------------
; Stage1_DrawSignPanel - writes one 12x7-tile facade panel of the stage 1 street (a 'BANK OF GOTHAM CITY' sign, intact or blasted) into plane B
; In:  d0 = state (0 intact, 8 blasted), a4 = slot record (Tbl_015CF4 + 16*slot: per state a long VDP write command and a long offset into the map buffer $F502), a0/a1 = VDP control/data
; Out: a3/d0-d3 trashed
; Evidence: 7 rows x 6 longs + $23122312 (palette 1, tile base $312), source stride $D8, destination +$100 per row (128-wide plane B at $E000; columns 8/26/46/64 of row 14); $20866 marks a slot 8 when a blast hits a sign (TAS f3600: the right sign blasted); 20 TAS frames f1830-f4478
; ---------------------------------------------------------------------------
Stage1_DrawSignPanel:
	andi.w	#$FF,d0
	move.l	(a4,d0.w),d1
	movea.l	$4(a4,d0.w),a3
	adda.l	(g_SceneVars+$E).w,a3
	move.l	#$23122312,d2
	moveq	#$6,d3
loc_015CBE:
	move.l	d1,(a0)
	move.l	(a3)+,d0
	add.l	d2,d0
	move.l	d0,(a1)
	move.l	(a3)+,d0
	add.l	d2,d0
	move.l	d0,(a1)
	move.l	(a3)+,d0
	add.l	d2,d0
	move.l	d0,(a1)
	move.l	(a3)+,d0
	add.l	d2,d0
	move.l	d0,(a1)
	move.l	(a3)+,d0
	add.l	d2,d0
	move.l	d0,(a1)
	move.l	(a3)+,d0
	add.l	d2,d0
	move.l	d0,(a1)
loc_015CE4:
	lea.l	$C0(a3),a3
	addi.l	#$1000000,d1
	dbra	d3,loc_015CBE
	rts
