; ---------------------------------------------------------------------------
; Stage1_VIntFacade - V-int object of the stage 1 street: steps the building fire, then redraws (up to 4 per V-int) the bank-sign panels whose wanted state ($F4FA-$F4FD) differs from the drawn one ($F4FE-$F501)
; In:  a0/a1 = VDP control/data (V-int object list $8ADA)
; Out: drawn states updated; skipped while g_FrameBusy
; Evidence: installed by the street scene $1FB3A through $8B7E (V-int list); slot records Tbl_015CF4 (16 bytes each: intact and blasted variants); TAS f532-f6334 every frame (5803 frames), 10 runs
; ---------------------------------------------------------------------------
Stage1_VIntFacade:
	tst.b	(g_FrameBusy).w
	bne.s	loc_015CA4
	move.b	(g_Stage1FireState).w,d0
	ble.s	loc_015C6E
	bsr.w	Stage1_VIntFireStep
loc_015C6E:
	move.l	a6,-(a7)
	lea.l	(loc_015CE4).l,a4
	lea.l	(g_SceneVars+$6).l,a5
	lea.l	(g_SceneVars+$A).l,a6
	moveq	#$4,d6
	moveq	#$3,d7
loc_015C86:
	lea.l	$10(a4),a4
	move.b	(a5)+,d0
	cmp.b	(a6)+,d0
	dbne	d7,loc_015C86
	beq.s	loc_015CA2
	move.b	d0,-$1(a6)
	bsr.w	Stage1_DrawSignPanel
	subq.w	#$1,d6
	dbeq	d7,loc_015C86
loc_015CA2:
	movea.l	(a7)+,a6
loc_015CA4:
	rts
