; ---------------------------------------------------------------------------
; Spr_ChangeFrame - switches a sprite from frame d2 to frame d1, loading the new frame's art into VRAM when it uses a different art handle
; In:  d1 = new frame, d2 = current frame (byte offsets: >= 0 into the ROM table Tbl_1DDB60, < 0 into the RAM table below $A764), a1 preserved via usp
; Out: a0 = the frame-table entry to draw, d1 = the frame now shown (d2 when the art could not be loaded this frame)
; Evidence: 37 call sites, typically `move.w (a1),d2 / bsr Spr_ChangeFrame / move.w d1,(a1)+ / bsr Spr_AddSorted` (a2 VM sprite ops $4524-$4630); equal handle words return at once, otherwise falls into Spr_SwapFrameArt; HUD digits $15940 step frames by 10
; ---------------------------------------------------------------------------
Spr_ChangeFrame:
	move.l	a1,usp
	tst.w	d1
	bmi.s	loc_007F16
	tst.w	d2
	bmi.s	loc_007F2E
	lea.l	(Tbl_1DDB60).l,a0
	lea.l	(a0,d2.w),a1
	adda.w	d1,a0
	move.w	(a0),d0
	cmp.w	(a1),d0
	bne.s	Spr_SwapFrameArt
	move.l	usp,a1
	rts
loc_007F16:
	tst.w	d2
	bpl.s	loc_007F46
	lea.l	(g_ArtRelRingWr).w,a0
	lea.l	(a0,d2.w),a1
	adda.w	d1,a0
	move.w	(a0),d0
	cmp.w	(a1),d0
	bne.s	Spr_SwapFrameArt
	move.l	usp,a1
	rts
loc_007F2E:
	lea.l	(Tbl_1DDB60).l,a0
	adda.w	d1,a0
	lea.l	(g_ArtRelRingWr).w,a1
	adda.w	d2,a1
	move.w	(a0),d0
	cmp.w	(a1),d0
	bne.s	Spr_SwapFrameArt
	move.l	usp,a1
	rts
loc_007F46:
	lea.l	(g_ArtRelRingWr).w,a0
	adda.w	d1,a0
	lea.l	(Tbl_1DDB60).l,a1
	adda.w	d2,a1
	move.w	(a0),d0
	cmp.w	(a1),d0
	bne.s	Spr_SwapFrameArt
	move.l	usp,a1
	rts
