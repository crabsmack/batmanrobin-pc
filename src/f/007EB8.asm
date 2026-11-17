; ---------------------------------------------------------------------------
; Spr_InitBankFrames - builds RAM frame-table entries for a loaded sprite bank, dropping any art still held by the bank's handles
; In:  a0 = bank (word count, then per frame: word handle offset, long self-relative piece-list pointer, long self-relative art pointer), d0 = offset of the first entry from $A764, d1 = handle base ($8002)
; Out: count 10-byte entries (handle address, piece list, art) at $A764+d0; each handle cleared
; Evidence: only caller the a6 VM op $70E6 (a0 = a buffer from the $E092 table, d0 from the script, d1 = $8002); a handle that still has a descriptor gets its back pointer set to the dummy handle $8000 and ArtCache_Release; ran on 16 TAS frames (bank loads)
; ---------------------------------------------------------------------------
Spr_InitBankFrames:
	lea.l	(g_ArtRelRingWr).w,a1
	adda.w	d0,a1
	move.w	(a0)+,d7
	bra.s	loc_007EF0
loc_007EC2:
	movea.w	(a0)+,a2
	adda.w	d1,a2
	move.w	(a2),d0
	beq.s	loc_007EE0
	movem.l	d1/a0,-(a7)
	movea.w	d0,a0
	move.w	#$8000,$4(a0)
	movea.l	a2,a0
	bsr.w	ArtCache_Release
	movem.l	(a7)+,d1/a0
loc_007EE0:
	clr.w	(a2)
	move.w	a2,(a1)+
	movea.l	a0,a2
	adda.l	(a0)+,a2
	move.l	a2,(a1)+
	movea.l	a0,a2
	adda.l	(a0)+,a2
	move.l	a2,(a1)+
loc_007EF0:
	dbra	d7,loc_007EC2
	rts
