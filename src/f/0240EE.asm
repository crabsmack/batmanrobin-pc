; ---------------------------------------------------------------------------
; Stage2_LiftStop - lift trigger callback of the zeppelin loading dock: sets the lift's goal speed $DFFE to 0, so Stage2_LiftUpdate brakes it to a stop with a jolt
; In:  d4/d5 = trigger x/y (Trigger_RunY)
; Out: $DFFE = 0, d0 = 0
; Evidence: dc.l entries in the y-keyed trigger table Tbl_02105A ($021754, $021826, $021970, $021A12, $021B28, $021C46); TAS f21729-f29582 (5 stops), the lift halting at each girder floor
; ---------------------------------------------------------------------------
Stage2_LiftStop:
	clr.l	(g_LiftGoalVelY).w
	moveq	#$0,d0
	rts
; ---------------------------------------------------------------------------
; Stage2_LiftStart - starts the zeppelin loading-dock lift upward: goal speed $DFFE = -1.0 px/frame and g_LiftVelY = -$2000
; In:  -
; Out: d0 = 0
; Evidence: called by the scene $32F14 at its start and by lift trigger entries in Tbl_02105A ($02178A, $021914, ...); the map then scrolls (g_MapScrollY falls from $1910)
; ---------------------------------------------------------------------------
Stage2_LiftStart:
	move.l	#$FFFF0000,(g_LiftGoalVelY).w
	move.l	#$FFFFE000,(g_LiftVelY).w
	moveq	#$0,d0
	rts
; ---------------------------------------------------------------------------
; Stage2_LiftUpdate - per-frame lift motion of the zeppelin loading dock: ramps g_LiftVelY by $300 toward the goal $DFFE (braking to 0 with a Shake_Start jolt below 0.25 px/frame), adds it to g_MapScrollY and, when the map reaches the top, switches to the boss arena
; In:  -
; Out: g_LiftVelY, g_MapScrollY updated; at the top: Stage2_LiftArriveTop, Shake_Start, scroll/speed/goal cleared
; Evidence: called every frame by the scene loop $32FBC; TAS f21158-f32065 the shaft scrolls down past the girders; the top is reached at f32065 (Stage2_LiftArriveTop)
; ---------------------------------------------------------------------------
Stage2_LiftUpdate:
	move.l	#$300,d2
	move.l	(g_LiftVelY).w,d0
	move.l	(g_LiftGoalVelY).w,d1
	beq.s	loc_024130
	bpl.s	loc_024126
	sub.l	d2,d0
	cmp.l	d1,d0
	bge.s	loc_024154
	move.l	d1,d0
	bra.s	loc_024154
loc_024126:
	add.l	d2,d0
	cmp.l	d1,d0
	ble.s	loc_024154
	move.l	d1,d0
	bra.s	loc_024154
loc_024130:
	tst.l	d0
	beq.s	loc_024158
	bpl.s	loc_024142
	add.l	d2,d0
	cmpi.l	#$FFFFC000,d0
	blt.s	loc_024154
	bra.s	loc_02414C
loc_024142:
	sub.l	d2,d0
	cmpi.l	#$4000,d0
	bgt.s	loc_024154
loc_02414C:
	jsr	(Shake_Start).l
	moveq	#$0,d0
loc_024154:
	move.l	d0,(g_LiftVelY).w
loc_024158:
	add.l	d0,(g_MapScrollY).w
	bpl.s	loc_02417C
	jsr	(Stage2_LiftArriveTop).l
	jsr	(Shake_Start).l
	clr.l	(g_MapScrollY).l
	clr.l	(g_LiftVelY).l
	clr.l	(g_LiftGoalVelY).l
loc_02417C:
	rts
