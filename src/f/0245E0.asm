; ---------------------------------------------------------------------------
; Stage2_ArenaHIntSplit - H-int at the arena's split line: points plane B at plane A's name table ($C000) and loads plane B's 20 column V-scrolls from $F576, so below the line plane B shows the other side of the tilting floor
; In:  -
; Out: VDP reg 4 = 6, reg $A = $FF, VSRAM 2.. written; rte
; Evidence: jmp stub from Stage2_ArenaVIntUpload ($0245C0); $8406, $40020010 + 10 longs; TAS f32067-f35416
; ---------------------------------------------------------------------------
Stage2_ArenaHIntSplit:
	move.l	a0,-(a7)
	lea.l	($C00004).l,a0
	move.w	#$8406,(a0)
	move.l	#$8AFF8F04,(a0)
	move.l	#$40020010,(a0)
	subq.w	#$4,a0
	move.l	a1,-(a7)
	lea.l	(g_SceneVars+$82).l,a1
	move.l	(a1)+,(a0)
	move.l	(a1)+,(a0)
	move.l	(a1)+,(a0)
	move.l	(a1)+,(a0)
	move.l	(a1)+,(a0)
	move.l	(a1)+,(a0)
	move.l	(a1)+,(a0)
	move.l	(a1)+,(a0)
	move.l	(a1)+,(a0)
	move.l	(a1)+,(a0)
	movea.l	(a7)+,a1
	movea.l	(a7)+,a0
	rte
; ---------------------------------------------------------------------------
; Stage2_LiftColumnScroll - fills the shaft's 20 plane A column V-scrolls from the view y in bands (Tbl_024642: word bands-1, then per band word columns-1 and a divisor), giving the girders and walls their depth
; In:  $E012 = view y (16.16)
; Out: g_SceneVars+$A.. = y/divisor per column
; Evidence: the lift branch of Stage2_UpdateScroll (bra); TAS f21158-f32065; read by Stage2_LiftVIntScroll
; ---------------------------------------------------------------------------
Stage2_LiftColumnScroll:
	move.l	(g_Stage2ViewY).w,d0
	asr.l	#$8,d0
	lea.l	(Tbl_024642).l,a0
	lea.l	(g_SceneVars+$A).l,a1
	move.w	(a0)+,d7
loc_024630:
	move.w	(a0)+,d6
	move.l	d0,d1
	divs.w	(a0)+,d1
loc_024636:
	move.w	d1,(a1)+
	dbra	d6,loc_024636
	dbra	d7,loc_024630
	rts
