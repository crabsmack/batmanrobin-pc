; ---------------------------------------------------------------------------
; Pal_VIntUpload - V-int palette upload: runs the white flash (CRAM filled with $0EEE, restored when g_FlashTimer ends) or else DMAs the lines of g_PalFaded that Fade_Step rebuilt to CRAM
; In:  a0 = VDP control port, a1 = VDP data port, a2 = RAM word pair for the DMA command ($FF9910, from VInt_Handler); g_FlashTimer, g_FlashReq, g_SceneShadowHL, the changed flags in g_PalLineCache
; Out: CRAM written; g_FlashReq cleared when the flash starts, g_FlashTimer counted down; VDP reg 12 = $81 during the flash, $89 again after it when g_SceneShadowHL; a3/d0/d1/d7 trashed
; Evidence: changed flags of lines 0-3 become bits 5-2 of d0, which picks one of 15 long pointers at $A87E to one or two DMA descriptors (regs $93/$95/$96/$97 for source $FFE152 + line*$20, CRAM address $C000/$C020/$C040/$C060); the last command word is written from RAM (2(a2) = $0080); flash end forces d0 = $3C (all lines); bsr from VInt_Handler every full V-int (130,055 TAS frames)
; ---------------------------------------------------------------------------
Pal_VIntUpload:
	tst.b	(g_FlashTimer).w
	beq.s	loc_00A830
	subq.b	#$1,(g_FlashTimer).w
	bne.s	loc_00A80C
	tst.b	(g_SceneShadowHL).w
	beq.s	loc_00A806
	move.w	#$8C89,(a0)
loc_00A806:
	move.w	#$3C,d0
	bra.s	loc_00A858
loc_00A80C:
	tst.b	(g_FlashReq).w
	beq.s	loc_00A87C
	clr.b	(g_FlashReq).w
	move.l	#$C0000000,(a0)
	move.l	#$EEE0EEE,d0
	moveq	#$1F,d7
loc_00A824:
	move.l	d0,(a1)
	dbra	d7,loc_00A824
	move.w	#$8C81,(a0)
	rts
loc_00A830:
	move.b	(g_Pal0Changed).w,d0
	andi.w	#$20,d0
	move.b	(g_Pal1Changed).w,d1
	andi.w	#$10,d1
	or.w	d1,d0
	move.b	(g_Pal2Changed).w,d1
	andi.w	#$8,d1
	or.w	d1,d0
	move.b	(g_Pal3Changed).w,d1
	andi.w	#$4,d1
	or.w	d1,d0
	beq.s	loc_00A87C
loc_00A858:
	movea.l	loc_00A87C-2(pc,d0.w),a3
	move.w	#$80,$2(a2)
	move.w	(a3)+,(a2)
	move.l	(a3)+,(a0)
	move.l	(a3)+,(a0)
	move.w	(a2),(a0)
	move.w	$2(a2),(a0)
	move.w	(a3)+,(a2)
	beq.s	loc_00A87C
	move.l	(a3)+,(a0)
	move.l	(a3)+,(a0)
	move.w	(a2),(a0)
	move.w	$2(a2),(a0)
loc_00A87C:
	rts
