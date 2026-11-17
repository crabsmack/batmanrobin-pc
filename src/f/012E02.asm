; ---------------------------------------------------------------------------
; Trigger_RunX - per-frame: fires the triggers whose x key enters the screen (g_CameraX to +$140), skipping those already left behind; a callback that fails is retried next frame
; In:  none
; Out: g_TriggerNext advanced past fired entries
; Evidence: called by the gameplay frames $9BC0/$9BD4 and stage loops ($03334A, $0401DA, ...); 77k TAS frames; skipped while g_ScreenClearTimer runs
; ---------------------------------------------------------------------------
Trigger_RunX:
	tst.w	(g_ScreenClearTimer).w
	beq.s	loc_012E0A
	rts
loc_012E0A:
	movea.l	(g_TriggerNext).w,a2
	movea.l	(g_TriggerTable).w,a3
	move.w	(g_CameraX).w,d6
	move.w	d6,d7
	addi.w	#$140,d7
loc_012E1C:
	move.l	(a2)+,d0
	swap	d0
	cmp.w	d7,d0
	bcc.s	loc_012E2E
	cmp.w	d6,d0
	bcs.s	loc_012E1C
	bsr.w	Trigger_Fire
	beq.s	loc_012E1C
loc_012E2E:
	subq.l	#$4,a2
	move.l	a2,(g_TriggerNext).w
	rts
