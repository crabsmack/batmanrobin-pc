; ---------------------------------------------------------------------------
; SpawnAction_Run - runs spawn action d0 (low byte) of Tbl_012EF0 at the object's position, preserving a2-a6
; In:  d0 = action number, d1 = its argument, a6 = object (+$3E/+$42 x/y become d4/d5)
; Out: whatever the action does (spawns pickups, items, counters)
; Evidence: callers Op2_RunSpawnAction ($ACC, the death path of scored objects, action from $19(a6)), $6F56 (a group counter's action) and $03D9AE (action $1C); Tbl_012EF0 holds 50 handlers: 0 = rts, 1/2 = $6F70/$6F9A, 3-35 item spawners $1393C-$13B0E, 36-49 the Drop_ handlers here; 623 TAS frames
; ---------------------------------------------------------------------------
SpawnAction_Run:
	movem.l	a2-a6,-(a7)
	move.w	$3E(a6),d4
	move.w	$42(a6),d5
	andi.w	#$FF,d0
	add.w	d0,d0
	add.w	d0,d0
	movea.l	Tbl_012EF0(pc,d0.w),a0
	jsr	(a0)
	movem.l	(a7)+,a2-a6
; ---------------------------------------------------------------------------
; SpawnAction_None - spawn action 0: does nothing (the rts closing SpawnAction_Run, Tbl_012EF0 entry 0)
; In:  none
; Out: none
; Evidence: Tbl_012EF0 long 0 = $00012EEE, this rts; objects with no death action ($19(a6) = 0) call it through jsr (a0) at $012EE8
; ---------------------------------------------------------------------------
SpawnAction_None:
	rts
