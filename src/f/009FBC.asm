; ---------------------------------------------------------------------------
; Section_RunNext - looks up the section that follows g_SectionId in an order table, makes it current and calls its routine from Tbl_00A0AC
; In:  a0 = order table of (from, to) word pairs, long 0 ends
; Out: d0 = -1 (Z clear) after the section routine returns; d0 = 0 (Z set) when g_SectionId has no entry (the game is complete)
; Evidence: Tbl_00A0AC = (word id, long routine): $0001 Stage_Intro1, $1100 $1FB3A, $1300 $1FC50 ... $4401 $47BD0; the a2 op $42FE and the a6 op $772E read/set the same g_SectionId; callers Main_RunSection; 20 TAS frames
; ---------------------------------------------------------------------------
Section_RunNext:
	move.w	(g_SectionId).w,d1
loc_009FC0:
	move.l	(a0)+,d0
	beq.s	loc_009FE4
	swap	d0
	cmp.w	d1,d0
	bne.s	loc_009FC0
	swap	d0
	move.w	d0,(g_SectionId).w
	lea.l	(Tbl_00A0AC).l,a0
loc_009FD6:
	cmp.w	(a0)+,d0
	beq.s	loc_009FDE
	addq.l	#$4,a0
	bra.s	loc_009FD6
loc_009FDE:
	movea.l	(a0),a0
	jsr	(a0)
	moveq	#-$1,d0
loc_009FE4:
	rts
