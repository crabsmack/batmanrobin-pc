; ---------------------------------------------------------------------------
; Boss_TurretPod_RunTurret - runs one turret child inline: places it at the pod's x/y plus its attachment offset (the child's a3 +$14/+$16), runs its script with VM2_RunObject, and ORs its var 0 into the pod's
; In:  d0 = the turret's a6 word (Z set = none: returns at once); a6/a4 = the pod
; Out: pod var 0 |= turret var 0; a4/a6 preserved
; Evidence: called 3 times by Boss_TurretPod_RunTurrets; the turret bodies (Scr2_02915A, entered from spawners $2F038/$2F05A/$2F07C) set the offset with Op2_SetTarget (0, 6), (0, -$11) ...; turret frames $3994-$39B2 rendered: twin-barrel guns with muzzle flashes
; ---------------------------------------------------------------------------
Boss_TurretPod_RunTurret:
	beq.s	loc_0290EC
	movem.l	a4/a6,-(a7)
	move.w	$3E(a6),d4
	move.w	$42(a6),d5
	movea.w	d0,a6
	movea.w	$8(a6),a3
	add.w	$14(a3),d4
	add.w	$16(a3),d5
	move.w	d4,$3E(a6)
	move.w	d5,$42(a6)
	jsr	(VM2_RunObject).l
	move.w	$22(a4),d0
	movem.l	(a7)+,a4/a6
	or.w	d0,$22(a4)
loc_0290EC:
	rts
; ---------------------------------------------------------------------------
; Boss_TurretPod_SpawnTurret1 - pod native: calls the spawner at the relative word and, on success, sets the new turret's var 0 = 1 and keeps it in the pod's $1E
; In:  a2 = script: word offset to an unlinked-object spawner; a6 = the pod
; Out: a2 past the word; $1E(a6) = turret a6; d0 = the spawner's result
; Evidence: op $2CB2 at $2EA2E (spawner $2F038, Obj_SpawnBossUnlinked); TAS f45122
; ---------------------------------------------------------------------------
Boss_TurretPod_SpawnTurret1:
	movea.l	a2,a0
	adda.w	(a2)+,a0
	movem.l	a1-a6,-(a7)
	jsr	(a0)
	bne.s	loc_02910C
	move.w	#$1,$22(a4)
	movea.l	a6,a0
	movem.l	(a7)+,a1-a6
	move.w	a0,$1E(a6)
	rts
loc_02910C:
	movem.l	(a7)+,a1-a6
	rts
; ---------------------------------------------------------------------------
; Boss_TurretPod_SpawnTurret2 - as SpawnTurret1 for the second turret: var 0 = 2, kept in $22(a6)
; In:  a2 = script: word offset to a spawner; a6 = the pod
; Out: a2 past the word; $22(a6) = turret a6
; Evidence: op $2CB2 at $2EA34 (spawner $2F05A); fuzz-45000 runs
; ---------------------------------------------------------------------------
Boss_TurretPod_SpawnTurret2:
	movea.l	a2,a0
	adda.w	(a2)+,a0
	movem.l	a1-a6,-(a7)
	jsr	(a0)
	bne.s	loc_029130
	move.w	#$2,$22(a4)
	movea.l	a6,a0
	movem.l	(a7)+,a1-a6
	move.w	a0,$22(a6)
	rts
loc_029130:
	movem.l	(a7)+,a1-a6
	rts
; ---------------------------------------------------------------------------
; Boss_TurretPod_SpawnTurret3 - as SpawnTurret1 for the third turret: var 0 = 4, kept in $26(a6)
; In:  a2 = script: word offset to a spawner; a6 = the pod
; Out: a2 past the word; $26(a6) = turret a6
; Evidence: op $2CB2 at $2EA3A (spawner $2F07C); TAS f45122
; ---------------------------------------------------------------------------
Boss_TurretPod_SpawnTurret3:
	movea.l	a2,a0
	adda.w	(a2)+,a0
	movem.l	a1-a6,-(a7)
	jsr	(a0)
	bne.s	loc_029154
	move.w	#$4,$22(a4)
	movea.l	a6,a0
	movem.l	(a7)+,a1-a6
	move.w	a0,$26(a6)
	rts
loc_029154:
	movem.l	(a7)+,a1-a6
	rts
