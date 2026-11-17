; ---------------------------------------------------------------------------
; Trigger_Fire - calls one trigger's callback with d4/d5 = its x/y and, when it succeeds (d0 = 0) and the long's top byte is nonzero, stores that byte as the spawned object's spawn action $19(a6)
; In:  d0 high word = entry offset, a3 = g_TriggerTable
; Out: Z set = fired; d0 nonzero = retry later
; Evidence: called by Trigger_RunX and Trigger_RunY, also by $02F58C; the trigger-table data comments show its read at pc $12E3C; Op2_RunSpawnAction runs the stored action when the object dies
; ---------------------------------------------------------------------------
Trigger_Fire:
	swap	d0
	lea.l	$6(a3,d0.w),a1
	move.w	(a1)+,d4
	move.w	(a1)+,d5
	move.b	(a1),d0
	movea.l	(a1)+,a0
	movem.l	d0/d6-d7/a2-a3,-(a7)
	jsr	(a0)
	movem.l	(a7)+,d0/d6-d7/a2-a3
	bne.s	loc_012E5A
	tst.b	d0
	beq.s	loc_012E5A
	move.b	d0,$19(a6)
	moveq	#$0,d0
loc_012E5A:
	rts
