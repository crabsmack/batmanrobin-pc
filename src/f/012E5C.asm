; ---------------------------------------------------------------------------
; Trigger_LoadY - loads a vertical-stage trigger table: sorts entries by y key (+4), largest first, into g_TriggerList ($FFFF head, 0 end)
; In:  a0 = trigger table (as Trigger_LoadX)
; Out: g_TriggerList, g_TriggerNext, g_TriggerTable
; Evidence: only caller the vertical zeppelin stage setup $032F14 (Tbl_02105A); ran once in the TAS (21157), scenario-continue and level-skip
; ---------------------------------------------------------------------------
Trigger_LoadY:
	move.l	a0,d6
	lea.l	(g_TriggerList).l,a1
	move.l	#$FFFF0000,(a1)+
	move.l	a1,(g_TriggerNext).w
	move.w	(a0)+,d7
	move.l	a0,(g_TriggerTable).w
	move.l	a0,d6
	bra.s	loc_012E94
loc_012E78:
	movea.l	a1,a2
	lea.l	$4(a2),a3
	movea.l	a3,a1
	move.w	$4(a0),d0
loc_012E84:
	move.l	-(a2),-(a3)
	cmp.w	(a2),d0
	bhi.s	loc_012E84
	move.w	d0,(a3)+
	move.l	a0,d0
	sub.l	d6,d0
	move.w	d0,(a3)+
	adda.w	(a0),a0
loc_012E94:
	dbra	d7,loc_012E78
	clr.l	(a1)
	rts
; ---------------------------------------------------------------------------
; Trigger_RunY - per-frame for the vertical stage: fires the triggers whose y key lies in the screen ($E00E to +$E0) as it scrolls up
; In:  none
; Out: g_TriggerNext advanced
; Evidence: pointed to from the vertical stage code at $032FC4; executed in 20 corpus runs (TAS, continue, level-skip); skipped while g_ScreenClearTimer runs
; ---------------------------------------------------------------------------
Trigger_RunY:
	tst.w	(g_ScreenClearTimer).w
	bne.s	loc_012ECC
	movea.l	(g_TriggerNext).w,a2
	movea.l	(g_TriggerTable).w,a3
	move.w	(g_MapScrollY).w,d6
	move.w	d6,d7
	addi.w	#$E0,d7
loc_012EB4:
	move.l	(a2)+,d0
	swap	d0
	cmp.w	d6,d0
	bls.s	loc_012EC6
	cmp.w	d7,d0
	bcc.s	loc_012EB4
	bsr.w	Trigger_Fire
	beq.s	loc_012EB4
loc_012EC6:
	subq.l	#$4,a2
	move.l	a2,(g_TriggerNext).w
loc_012ECC:
	rts
