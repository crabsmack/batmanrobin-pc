; ---------------------------------------------------------------------------
; Boss_Stage2_SetTurret1Coroutine - boss native: sets turret 1's (a6 +$1E) coroutine 0 to the script at the inline relative offset, so the hull's attack script commands that turret (to fire)
; In:  a2 = inline offset word, a6 = boss
; Out: turret's a4 +$38 = target
; Evidence: dc.l after op $2CBC at $026FE4, $027034, $027084 ... (all with offsets to $0277C6); TAS f32290-f32770 (3 frames)
; ---------------------------------------------------------------------------
Boss_Stage2_SetTurret1Coroutine:
	movea.w	$1E(a6),a0
	bra.s	loc_026062
; ---------------------------------------------------------------------------
; Boss_Stage2_SetTurret2Coroutine - entry of Boss_Stage2_SetTurret1Coroutine for turret 2 (a6 +$20)
; In:  a2 = inline offset word
; Out: turret 2's coroutine 0 set
; Evidence: dc.l after op $2CBC at $026FF8, $027048, $02708E ...
; ---------------------------------------------------------------------------
Boss_Stage2_SetTurret2Coroutine:
	movea.w	$20(a6),a0
	bra.s	loc_026062
; ---------------------------------------------------------------------------
; Boss_Stage2_SetTurret3Coroutine - entry of Boss_Stage2_SetTurret1Coroutine for turret 3 (a6 +$22)
; In:  a2 = inline offset word
; Out: turret 3's coroutine 0 set
; Evidence: dc.l after op $2CBC at $02700C, $02705C, $027098 ...
; ---------------------------------------------------------------------------
Boss_Stage2_SetTurret3Coroutine:
	movea.w	$22(a6),a0
	bra.s	loc_026062
; ---------------------------------------------------------------------------
; Boss_Stage2_SetTurret4Coroutine - entry of Boss_Stage2_SetTurret1Coroutine for turret 4 (a6 +$24)
; In:  a2 = inline offset word
; Out: turret 4's coroutine 0 set
; Evidence: dc.l after op $2CBC at $027020, $027070, $0270A2 ...
; ---------------------------------------------------------------------------
Boss_Stage2_SetTurret4Coroutine:
	movea.w	$24(a6),a0
loc_026062:
	movea.l	a2,a1
	adda.w	(a2)+,a1
	movea.w	$A(a0),a0
	move.l	a1,$38(a0)
	rts
; ---------------------------------------------------------------------------
; Boss_Stage2_MoveHullRunTurrets - boss native, every frame: moves the hull's y by -3/4 of the floor roll speed (g_SceneVars2+$12; not above -500), copies the hull x/y to the plane B picture position (g_SceneVars2+$16/$1A), then runs the four unlinked turret objects' scripts inline
; In:  a6 = boss (+$1E-$24 turret objects, +$3E/$42 x/y)
; Out: g_VM2SavedSP and a1-a6 preserved
; Evidence: op $2CB2 at $025C9C (phase 1) and $025CEA (phase 2); VM2_RunObject x4 (turrets spawned with Obj_SpawnBossUnlinked); TAS f32140-f35416
; ---------------------------------------------------------------------------
Boss_Stage2_MoveHullRunTurrets:
	move.l	(g_SceneVars2+$12).w,d0
	move.l	d0,d1
	asr.l	#$2,d0
	sub.l	d1,d0
	add.l	$42(a6),d0
	cmpi.l	#$FE0C0000,d0
	bgt.s	loc_02608C
	move.l	#$FE0C0000,d0
loc_02608C:
	move.l	d0,$42(a6)
	move.w	$3E(a6),(g_SceneVars2+$16).w
	move.w	$42(a6),(g_SceneVars2+$1A).w
	move.l	(g_VM2SavedSP).w,-(a7)
	movem.l	a1-a6,-(a7)
	move.w	$24(a6),-(a7)
	move.w	$22(a6),-(a7)
	move.w	$20(a6),-(a7)
	movea.w	$1E(a6),a6
	jsr	(VM2_RunObject).l
	movea.w	(a7)+,a6
	jsr	(VM2_RunObject).l
	movea.w	(a7)+,a6
	jsr	(VM2_RunObject).l
	movea.w	(a7)+,a6
	jsr	(VM2_RunObject).l
	movem.l	(a7)+,a1-a6
	move.l	(a7)+,(g_VM2SavedSP).w
	rts
; ---------------------------------------------------------------------------
; Boss_Stage2_PlaceTurret - native of the turret / ladder scripts: places the object at its hull offset (script vars 1/2) tilted with the arena floor (Boss_Stage2_TiltPoint)
; In:  a4 +$24/$26 = offset x/y from the hull picture
; Out: $3E/$42(a6) = screen x/y words
; Evidence: op $2CB2 at $026200 and $026230 (turret loops), bsr from Enemy_LadderThug_PlaceOnHull; RAM f32220: turret var1 $B7 + picture x -85 = x 98
; ---------------------------------------------------------------------------
Boss_Stage2_PlaceTurret:
	move.w	$24(a4),d0
	move.w	$26(a4),d1
	bsr.w	Boss_Stage2_TiltPoint
	move.w	d4,$3E(a6)
	move.w	d5,$42(a6)
	rts
