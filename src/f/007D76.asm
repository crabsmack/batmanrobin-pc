; ---------------------------------------------------------------------------
; Spr_EmitClipped - emits the pieces of one frame into the SAT buffer, clipping against the window g_SprDirectMaxY/MaxX and biasing by g_SprDirectBiasY/BiasX
; In:  a0 = frame-table entry, a1 = SAT write pointer, d2 = sprite count, d3 = 0, d4/d5 = x/y, d6 = attribute word
; Out: a1 and d2 advanced; stops at 80 sprites
; Evidence: only caller Spr_DrawDirect; same piece loop as Spr_BuildSAT but the limits come from $9B4C/$9B50 and the bias from $9B4E/$9B52 (scene init $9AAC sets $100/$160 and $60/$60, Spr_BuildSAT's constants)
; ---------------------------------------------------------------------------
Spr_EmitClipped:
	cmpi.b	#$50,d2
	beq.s	loc_007DDA
	movea.w	(a0)+,a2
	move.w	(a2),d0
	beq.s	loc_007DDA
	movea.w	d0,a2
	movea.l	(a0),a0
	move.w	(a0)+,d7
	btst	#$B,d6
	bne.w	loc_007DDC
	btst	#$C,d6
	bne.w	loc_007E2A
loc_007D98:
	move.l	(a0)+,d0
	move.l	(a0)+,d1
	ext.w	d2
	or.w	(a0)+,d2
	move.b	(a0)+,d3
	move.b	(a0)+,d6
	add.w	d5,d0
	cmp.w	(g_SprDirectMaxY).w,d0
	bcc.s	loc_007DD6
	add.w	d4,d1
	cmp.w	(g_SprDirectMaxX).w,d1
	bcc.s	loc_007DD6
	add.w	(g_SprDirectBiasY).w,d0
	add.w	(g_SprDirectBiasX).w,d1
	addq.w	#$1,d2
	move.w	d0,(a1)+
	move.w	d2,(a1)+
	move.w	$6(a2,d3.w),d0
	add.w	d6,d0
	move.w	d0,(a1)+
	move.w	d1,(a1)+
	cmpi.b	#$50,d2
	dbeq	d7,loc_007D98
	rts
loc_007DD6:
	dbra	d7,loc_007D98
loc_007DDA:
	rts
loc_007DDC:
	btst	#$C,d6
	bne.w	loc_007E70
loc_007DE4:
	move.l	(a0)+,d0
	move.l	(a0)+,d1
	ext.w	d2
	or.w	(a0)+,d2
	move.b	(a0)+,d3
	move.b	(a0)+,d6
	swap	d1
	add.w	d5,d0
	cmp.w	(g_SprDirectMaxY).w,d0
	bcc.s	loc_007E24
	add.w	d4,d1
	cmp.w	(g_SprDirectMaxX).w,d1
	bcc.s	loc_007E24
	add.w	(g_SprDirectBiasY).w,d0
	add.w	(g_SprDirectBiasX).w,d1
	addq.w	#$1,d2
	move.w	d0,(a1)+
	move.w	d2,(a1)+
	move.w	$6(a2,d3.w),d0
	add.w	d6,d0
	move.w	d0,(a1)+
	move.w	d1,(a1)+
	cmpi.b	#$50,d2
	dbeq	d7,loc_007DE4
	rts
loc_007E24:
	dbra	d7,loc_007DE4
	rts
loc_007E2A:
	move.l	(a0)+,d0
	move.l	(a0)+,d1
	ext.w	d2
	or.w	(a0)+,d2
	move.b	(a0)+,d3
	move.b	(a0)+,d6
	swap	d0
	add.w	d5,d0
	cmp.w	(g_SprDirectMaxY).w,d0
	bcc.s	loc_007E6A
	add.w	d4,d1
	cmp.w	(g_SprDirectMaxX).w,d1
	bcc.s	loc_007E6A
	add.w	(g_SprDirectBiasY).w,d0
	add.w	(g_SprDirectBiasX).w,d1
	addq.w	#$1,d2
	move.w	d0,(a1)+
	move.w	d2,(a1)+
	move.w	$6(a2,d3.w),d0
	add.w	d6,d0
	move.w	d0,(a1)+
	move.w	d1,(a1)+
	cmpi.b	#$50,d2
	dbeq	d7,loc_007E2A
	rts
loc_007E6A:
	dbra	d7,loc_007E2A
	rts
loc_007E70:
	move.l	(a0)+,d0
	move.l	(a0)+,d1
	ext.w	d2
	or.w	(a0)+,d2
	move.b	(a0)+,d3
	move.b	(a0)+,d6
	swap	d0
	swap	d1
	add.w	d5,d0
	cmp.w	(g_SprDirectMaxY).w,d0
	bcc.s	loc_007EB2
	add.w	d4,d1
	cmp.w	(g_SprDirectMaxX).w,d1
	bcc.s	loc_007EB2
	add.w	(g_SprDirectBiasY).w,d0
	add.w	(g_SprDirectBiasX).w,d1
	addq.w	#$1,d2
	move.w	d0,(a1)+
	move.w	d2,(a1)+
	move.w	$6(a2,d3.w),d0
	add.w	d6,d0
	move.w	d0,(a1)+
	move.w	d1,(a1)+
	cmpi.b	#$50,d2
	dbeq	d7,loc_007E70
	rts
loc_007EB2:
	dbra	d7,loc_007E70
	rts
