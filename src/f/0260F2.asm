; ---------------------------------------------------------------------------
; Boss_Stage2_TiltPoint - maps a point (d0, d1) of the hull picture to screen x/y with the arena tilt: picture x/y (g_SceneVars2+$16/$1A) + the point, sheared by the floor slope (g_SceneVars+6) about the pivot x (+$8) and the floor y (+$C)
; In:  d0/d1 = point offset in the hull picture
; Out: d4.w/d5.w = screen x/y (fractions in the high words); d2/d3 used
; Evidence: y += (x - pivot) * slope, x -= (y - floor y) * slope (muls with g_SceneVars+6); callers Boss_Stage2_PlaceTurret, Boss_Stage2_FxAtHullPoint; TAS f32140-f35416 (3277 frames)
; ---------------------------------------------------------------------------
Boss_Stage2_TiltPoint:
	moveq	#$0,d5
	move.w	(g_SceneVars+$6).w,d2
	move.w	d2,d4
	ext.l	d4
	asl.l	#$4,d4
	bpl.s	loc_026108
	move.w	d2,d5
	ext.l	d5
	asl.l	#$5,d5
	neg.l	d5
loc_026108:
	move.l	(g_SceneVars2+$16).w,d3
	sub.l	(g_SceneVars2+$8).w,d3
	swap	d3
	add.w	d0,d3
	muls.w	d2,d3
	add.l	(g_SceneVars2+$1A).w,d3
	add.l	d3,d5
	swap	d5
	add.w	d1,d5
	move.l	(g_SceneVars2+$1A).w,d3
	sub.l	(g_SceneVars2+$C).w,d3
	swap	d3
	add.w	d1,d3
	muls.w	d2,d3
	neg.l	d3
	add.l	(g_SceneVars2+$16).w,d3
	add.l	d3,d4
	swap	d4
	add.w	d0,d4
	rts
; ---------------------------------------------------------------------------
; Boss_Stage2_SpawnTurret1 - spawns turret 1 of the stage 2 boss (Scr2_026148, unlinked, run by Boss_Stage2_MoveHullRunTurrets): bit 1 of g_Stage2TurretsDown, hull offset ($B7, $63)
; In:  d4/d5 = position
; Out: d0 = 0, a6 = turret
; Evidence: dc.l at $025C0E (Op2_SpawnChildOrWait field $1E); Scr2_026148 sets vars 0/1/2 = 1, $B7, $63 then the shared turret body $0261AC (energy $800/$1000, BossMeter_AddPart, aims at the player); RAM f32220: x 98 (leftmost)
; ---------------------------------------------------------------------------
Boss_Stage2_SpawnTurret1:
	lea.l	(Scr2_026148).l,a2
	jmp	(Obj_SpawnBossUnlinked).l
