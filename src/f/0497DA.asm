; ---------------------------------------------------------------------------
; Title_PaletteFadeRun - handler of the title palette fade-in: Start (pad 1 newly pressed bit 7) loads palette lines 0-2 at full brightness and frees the object; otherwise steps the 7 colour-range channels
; In:  a6 = the fade object
; Out: -
; Evidence: btst #7,g_Pad1+2 -> Pal_LoadLine0/1/2 from a6+$A, Obj_UnlinkFree; else 7 x Title_PaletteFadeChannel; idle 4680 frames, TAS f404-f405 (Start pressed at once)
; ---------------------------------------------------------------------------
Title_PaletteFadeRun:
	btst	#$7,(g_Pad1+2).w
	beq.s	loc_0497FE
	movea.l	$A(a6),a0
	jsr	(Pal_LoadLine0).l
	jsr	(Pal_LoadLine1).l
	jsr	(Pal_LoadLine2).l
	jmp	(Obj_UnlinkFree).l
loc_0497FE:
	lea.l	$E(a6),a5
	moveq	#$6,d7
loc_049804:
	move.w	d7,-(a7)
	bsr.w	Title_PaletteFadeChannel
	move.w	(a7)+,d7
	lea.l	$A(a5),a5
	dbra	d7,loc_049804
	rts
