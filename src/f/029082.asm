; ---------------------------------------------------------------------------
; Boss_TurretPod_RunTurrets - pod native, every frame: clears script var 0, then runs each live turret attached to the pod and ORs each turret's var 0 bit (1/2/4) into it, so var 0 = which turrets are still alive
; In:  a6/a4 = the pod
; Out: pod var 0 ($22(a4)) = OR of the live turrets' bits; g_VM2SavedSP and a1-a3/a5 preserved
; Evidence: op $2CB2 at $28DE0/$28E2C in the pod script Scr2_028D80, which unlocks its own body box (Op2_BranchVar0None) only when var 0 is 0; TAS 45123-46053 (925 frames)
; ---------------------------------------------------------------------------
Boss_TurretPod_RunTurrets:
	move.l	(g_VM2SavedSP).w,-(a7)
	movem.l	a1-a3/a5,-(a7)
	clr.w	$22(a4)
	move.w	$1E(a6),d0
	jsr	(Boss_TurretPod_RunTurret).l
	move.w	$22(a6),d0
	jsr	(Boss_TurretPod_RunTurret).l
	move.w	$26(a6),d0
	jsr	(Boss_TurretPod_RunTurret).l
	movem.l	(a7)+,a1-a3/a5
	move.l	(a7)+,(g_VM2SavedSP).w
	rts
