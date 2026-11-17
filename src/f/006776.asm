; ---------------------------------------------------------------------------
; Op4_DrawKillSidesBelowOrClear - a4 op: deletes the object during a screen clear or when d4/d5 is off the sides or below the on-screen window (above the top is kept); otherwise draws it (Op4_Draw), ending the frame
; In:  a4 = script (no operands); d4/d5 = drawn x/y words
; Out: ends the run (draw) or Op4_Kill
; Evidence: x - g_OnScreenLeft >= $170 unsigned, y - g_OnScreenTop >= $110 signed (blt keeps negative y); script uses $1ABAA, $1F116; 6 corpus runs
; ---------------------------------------------------------------------------
Op4_DrawKillSidesBelowOrClear:
	tst.w	(g_ScreenClearTimer).w
	bne.w	Op4_Kill
; ---------------------------------------------------------------------------
; Op4_DrawKillSidesBelow - a4 op: Op4_DrawKillSidesBelowOrClear without the screen-clear test: kills off the sides or below the window, else draws
; In:  a4 = script (no operands); d4/d5 = drawn x/y words
; Out: ends the run (draw) or Op4_Kill
; Evidence: dispatched entry after the g_ScreenClearTimer test (computed-jump target in 43 corpus runs); 19 script uses, e.g. $1A9B8, $1B564
; ---------------------------------------------------------------------------
Op4_DrawKillSidesBelow:
	move.w	d4,d0
	sub.w	(g_OnScreenLeft).w,d0
	cmpi.w	#$170,d0
	bcc.w	Op4_Kill
	move.w	d5,d0
	sub.w	(g_OnScreenTop).w,d0
	cmpi.w	#$110,d0
	blt.s	Op4_Draw
	bra.w	Op4_Kill
; ---------------------------------------------------------------------------
; Op4_DrawKillOutsideOrClear - a4 op: deletes the object during a screen clear or when its drawn point is outside a camera-relative rectangle; otherwise draws it (Op4_Draw)
; In:  a4 = script: words x lo, x hi, y lo, y hi (screen px from g_CameraX/Y); d4/d5 = drawn x/y words
; Out: ends the run (draw) or Op4_Kill
; Evidence: signed cmp of d4 - g_CameraX / d5 - g_CameraY with the 4 words; script uses $15072 (`679C 0000 0190 FF9C 01A8`), $150B0; 26 corpus runs
; ---------------------------------------------------------------------------
Op4_DrawKillOutsideOrClear:
	tst.w	(g_ScreenClearTimer).w
	bne.w	Op4_Kill
; ---------------------------------------------------------------------------
; Op4_DrawKillOutside - a4 op: Op4_DrawKillOutsideOrClear without the screen-clear test
; In:  a4 = script: words x lo, x hi, y lo, y hi; d4/d5 = drawn x/y words
; Out: ends the run (draw) or Op4_Kill
; Evidence: dispatched entry after the g_ScreenClearTimer test (computed-jump target in 17 corpus runs); script uses $1FF9C (`67A4 FFB0 03E8 0000 00E0`), $306B4, $428A2
; ---------------------------------------------------------------------------
Op4_DrawKillOutside:
	move.w	d4,d0
	sub.w	(g_CameraX).w,d0
	cmp.w	(a4)+,d0
	blt.w	Op4_Kill
	cmp.w	(a4)+,d0
	bgt.w	Op4_Kill
	move.w	d5,d0
	sub.w	(g_CameraY).w,d0
	cmp.w	(a4)+,d0
	blt.w	Op4_Kill
	cmp.w	(a4)+,d0
	ble.s	Op4_Draw
	bra.w	Op4_Kill
; ---------------------------------------------------------------------------
; Op4_DrawKillOffScreenOrClear - a4 op: deletes the object during a screen clear or when d4/d5 is outside the on-screen window (unsigned x < $170, y < $110 from g_OnScreenLeft/Top); otherwise draws it (Op4_Draw)
; In:  a4 = script (no operands); d4/d5 = drawn x/y words
; Out: ends the run (draw) or Op4_Kill
; Evidence: g_ScreenClearTimer, then two unsigned window tests; script use $1515E; 63 corpus runs
; ---------------------------------------------------------------------------
Op4_DrawKillOffScreenOrClear:
	tst.w	(g_ScreenClearTimer).w
	bne.w	Op4_Kill
; ---------------------------------------------------------------------------
; Op4_DrawKillOffScreen - a4 op: Op4_DrawKillOffScreenOrClear without the screen-clear test: off the on-screen window kills, else draws
; In:  a4 = script (no operands); d4/d5 = drawn x/y words
; Out: ends the run (draw) or Op4_Kill
; Evidence: dispatched entry after the g_ScreenClearTimer test (computed-jump target in 151 corpus runs); 44 script uses, e.g. $13244 (`6286 65B4 ... 67D2`), the player shots $505A0
; ---------------------------------------------------------------------------
Op4_DrawKillOffScreen:
	move.w	d4,d0
	sub.w	(g_OnScreenLeft).w,d0
	cmpi.w	#$170,d0
	bcc.w	Op4_Kill
	move.w	d5,d0
	sub.w	(g_OnScreenTop).w,d0
	cmpi.w	#$110,d0
	bcc.w	Op4_Kill
; ---------------------------------------------------------------------------
; Op4_Draw - a4 op: steps the animation ticker ($1C += rate $1D) and on its carry runs the anim script at $C (whose frame op draws); otherwise draws the current frame (Op4_DrawFrame); ends the frame either way
; In:  a4 = script (no operands); d4/d5 = drawn x/y words
; Out: ends the run (the draw's rts)
; Evidence: add.b $1D,$1C / bcc Op4_DrawFrame / movea.l $C(a6),a4; 117 script uses, e.g. $108E2 (`634E 67EE`); 151 corpus runs
; ---------------------------------------------------------------------------
Op4_Draw:
	move.b	$1D(a6),d0
	add.b	d0,$1C(a6)
	bcc.s	Op4_DrawFrame
	movea.l	$C(a6),a4
	movea.w	(a4)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op4_SetAttr - a4 op: edits the object's sprite attribute byte $22 (priority, palette, flip bits): ANDs a mask, then XORs a value
; In:  a4 = script: byte and-mask, byte xor-value
; Out: NEXT
; Evidence: and.b / eor.b on $22(a6); 123 script uses, e.g. $14612 (`6800 9800`), $14634; 150 corpus runs
; ---------------------------------------------------------------------------
Op4_SetAttr:
	move.b	(a4)+,d0
	and.b	d0,$22(a6)
	move.b	(a4)+,d0
	eor.b	d0,$22(a6)
	movea.w	(a4)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op4_ChildSetAttr - a4 op: edits the last spawned child's attribute byte $22(a5): AND mask, then XOR value
; In:  a4 = script: byte and-mask, byte xor-value; a5 = child
; Out: NEXT
; Evidence: and.b / eor.b on $22(a5); script uses $26C5E (`6810 F008`), $49586 (`6810 7880`); 3 corpus runs
; ---------------------------------------------------------------------------
Op4_ChildSetAttr:
	move.b	(a4)+,d0
	and.b	d0,$22(a5)
	move.b	(a4)+,d0
	eor.b	d0,$22(a5)
	movea.w	(a4)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op4_AnimFrameAttr - anim-script op (a4): edits the attribute byte $22 (AND mask, XOR value), then as Op4_AnimFrame shows a frame, saves the anim pc and draws
; In:  a4 = anim script: byte and-mask, byte xor-value, word frame
; Out: ends the run (draw)
; Evidence: and/eor $22, move.w (a4)+,$1E, move.l a4,$C, falls into Op4_DrawFrame; first op of the anim scripts, e.g. $108E4 (`6820 1880 22E2 682C 22EC ...`); 135 corpus runs
; ---------------------------------------------------------------------------
Op4_AnimFrameAttr:
	move.b	(a4)+,d0
	and.b	d0,$22(a6)
	move.b	(a4)+,d0
	eor.b	d0,$22(a6)
; ---------------------------------------------------------------------------
; Op4_AnimFrame - anim-script op (a4): shows the next animation frame: wanted frame $1E = word, anim pc $C = the op after it, then draws (Op4_DrawFrame) and ends the frame until the next ticker carry
; In:  a4 = anim script: word frame
; Out: ends the run (draw)
; Evidence: dispatched entry inside Op4_AnimFrameAttr (computed-jump target in 151 corpus runs); 639 uses in anim scripts, e.g. $108EA (`682C 22EC 682C 22F6 5406`: frames then Op4_Kill)
; ---------------------------------------------------------------------------
Op4_AnimFrame:
	move.w	(a4)+,$1E(a6)
	move.l	a4,$C(a6)
; ---------------------------------------------------------------------------
; Op4_DrawFrame - a4 op / draw tail: when the wanted frame $1E differs from the shown one $20 swaps the frame art (VRAM art cache acquire + DMA, release of the old; keeps the old frame when VRAM or DMA budget is short), then adds the sprite at d4/d5 with attr $22 to the display list by depth $24
; In:  a6 = object, d4/d5 = x/y words
; Out: ends the run (rts of Spr_AddFront / Spr_AddBack / Spr_AddSorted); $20 = frame shown
; Evidence: the Spr_SwapFrameArt logic inline (Tbl_1DDB60 / RAM table at g_ArtRelRingWr, ArtCache_Alloc, Spr_QueueArtDMA, g_ArtBlocksFree / g_ArtDescsFree); $24 = 0 / 1 / else; script use after Op4_CallNative `5438 0003 70DC 6834`; 32 corpus runs
; ---------------------------------------------------------------------------
Op4_DrawFrame:
	move.w	$1E(a6),d6
	bmi.s	loc_006858
	move.w	$20(a6),d7
	bpl.s	loc_00688A
	lea.l	(Tbl_1DDB60).l,a0
	adda.w	d6,a0
	lea.l	(g_ArtRelRingWr).w,a1
	adda.w	d7,a1
	move.w	(a1),d3
	cmp.w	(a0),d3
	bne.s	loc_00689E
	bra.w	loc_006936
loc_006858:
	move.w	$20(a6),d7
	bpl.s	loc_006872
	lea.l	(g_ArtRelRingWr).w,a0
	lea.l	(a0,d7.w),a1
	adda.w	d6,a0
	move.w	(a1),d3
	cmp.w	(a0),d3
	bne.s	loc_00689E
	bra.w	loc_006936
loc_006872:
	lea.l	(g_ArtRelRingWr).w,a0
	adda.w	d6,a0
	lea.l	(Tbl_1DDB60).l,a1
	adda.w	d7,a1
	move.w	(a1),d3
	cmp.w	(a0),d3
	bne.s	loc_00689E
	bra.w	loc_006936
loc_00688A:
	lea.l	(Tbl_1DDB60).l,a0
	lea.l	(a0,d7.w),a1
	adda.w	d6,a0
	move.w	(a1),d3
	cmp.w	(a0),d3
	beq.w	loc_006936
loc_00689E:
	movea.w	(a0),a2
	move.w	(a2),d0
	bne.s	loc_0068F2
	tst.w	d6
	beq.s	loc_006908
	movea.l	$6(a0),a4
	movem.w	(a4),d0-d1
	add.w	d1,d0
	cmp.w	(g_DMABudgetLeft).w,d0
	bgt.s	loc_0068EA
	cmp.w	(g_DMAQueueFree).w,d1
	bgt.s	loc_0068EA
	sub.b	d1,(g_ArtBlocksFree).w
	bcs.s	loc_0068E6
	subq.b	#$1,(g_ArtDescsFree).w
	bcs.s	loc_0068E2
	move.l	a0,usp
	movem.w	d3-d6,-(a7)
	movea.l	a2,a0
	bsr.w	ArtCache_Alloc
	bsr.w	Spr_QueueArtDMA
	movem.w	(a7)+,d3-d6
	move.l	usp,a0
	bra.s	loc_006908
loc_0068E2:
	clr.b	(g_ArtDescsFree).w
loc_0068E6:
	add.b	d1,(g_ArtBlocksFree).w
loc_0068EA:
	movea.l	a1,a0
	move.w	d7,$20(a6)
	bra.s	loc_00693A
loc_0068F2:
	movea.w	d0,a2
	subq.b	#$1,(a2)+
	bcc.s	loc_006904
	move.b	(a2),d0
	sub.b	d0,(g_ArtBlocksFree).w
	subq.b	#$1,(g_ArtDescsFree).w
	bra.s	loc_006908
loc_006904:
	bmi.s	loc_006908
	addq.b	#$1,(a2)
loc_006908:
	movea.w	d3,a2
	move.w	(a2),d0
	beq.s	loc_006936
	movea.w	d0,a2
	addq.b	#$1,(a2)+
	beq.s	loc_00691C
	bmi.s	loc_006936
	clr.b	-$1(a2)
	bra.s	loc_006936
loc_00691C:
	addq.b	#$1,(g_ArtDescsFree).w
	move.b	(a2)+,d2
	add.b	d2,(g_ArtBlocksFree).w
	tst.w	(a2)
	bne.s	loc_006936
	movea.w	(g_ArtRelRingWr).w,a3
	addq.b	#$2,(g_ArtRelRingWrLo).w
	move.w	a3,(a2)
	move.w	d0,(a3)
loc_006936:
	move.w	d6,$20(a6)
loc_00693A:
	move.w	$22(a6),d6
	move.w	$24(a6),d7
	beq.w	Spr_AddFront
	subq.w	#$1,d7
	beq.w	Spr_AddBack
	bra.w	Spr_AddSorted
