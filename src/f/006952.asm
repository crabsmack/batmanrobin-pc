; ---------------------------------------------------------------------------
; Op4_SetShotFrames - a4 op (shots): sets the object's frame ring $C (cyclic list of {next-node long, sprite-data long}) and its resume pc $8 to the next op
; In:  a4 = script: long frame-ring address
; Out: NEXT
; Evidence: move.l (a4)+,$C / move.l a4,$8; player shot scripts $50524 (`6952 0005 0612 6990`); Op4_ShotStep walks the ring; 148 corpus runs
; ---------------------------------------------------------------------------
Op4_SetShotFrames:
	move.l	(a4)+,$C(a6)
	move.l	a4,$8(a6)
	movea.w	(a4)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op4_SetVelFromHeading - a4 op: sets the object's x/y velocity to speed * (cos, sin)(heading) (8.8 speed) and steps x/y back by it once, so the first move lands on the start point
; In:  a4 = script: word speed (8.8 px/frame)
; Out: NEXT
; Evidence: Tbl_059B22 by $2A, muls speed, asr 8 -> $3E/$42, subtracted from $2E/$32; shot scripts $50520 (`695E 0E00`: 14 px/frame); 148 corpus runs
; ---------------------------------------------------------------------------
Op4_SetVelFromHeading:
	lea.l	(Tbl_059B22).l,a0
	move.w	$2A(a6),d0
	andi.w	#$3FE,d0
	adda.w	d0,a0
	move.w	(a4)+,d0
	move.w	d0,d1
	muls.w	$100(a0),d0
	muls.w	(a0),d1
	asr.l	#$8,d0
	asr.l	#$8,d1
	move.l	d0,$3E(a6)
	move.l	d1,$42(a6)
	sub.l	d0,$2E(a6)
	sub.l	d1,$32(a6)
	movea.w	(a4)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op4_ShotStep - a4 op (shots): moves x/y by the velocity, then as Op4_ShotStepAt hit-tests the enemy boxes on its phase (branch on a hit) or kills itself off screen, and draws the next ring frame
; In:  a4 = script: word half-width, word half-height, word hit branch offset
; Out: the hit branch, or ends the run (drawn or deleted)
; Evidence: adds $3E/$42 to $2E/$32, falls into Op4_ShotStepAt; player shot scripts $5052A (`6990 0006 0006 009E`), $5055E; 148 corpus runs
; ---------------------------------------------------------------------------
Op4_ShotStep:
	move.l	$3E(a6),d0
	add.l	d0,$2E(a6)
	move.l	$42(a6),d0
	add.l	d0,$32(a6)
	move.w	$2E(a6),d4
	move.w	$32(a6),d5
; ---------------------------------------------------------------------------
; Op4_ShotStepAt - a4 op (shots): at d4/d5: on the object's frame phase branches if an enemy box overlaps +-w/+-h; on the other frames deletes the object when off camera ($140 x $E0); then pushes the next frame of its ring $C straight onto the front sprite list (no art cache) and ends the frame
; In:  a4 = script: word half-width, word half-height, word hit branch offset; d4/d5 = x/y words
; Out: the hit branch, or ends the run
; Evidence: dispatched entry inside Op4_ShotStep (computed-jump target in 117 corpus runs); g_FrameParity vs $18, Hitbox_FindEnemyBox, $8ABE (unlink + free), g_SprFrontEnd / g_SprNodesFree; uses $5054A (`62E2 69A8 0006 0006 007E`), $5057E
; ---------------------------------------------------------------------------
Op4_ShotStepAt:
	move.w	d4,d2
	move.w	d5,d3
	move.b	(g_FrameParity).w,d0
	cmp.b	$18(a6),d0
	bne.s	loc_0069CE
	move.w	(a4)+,d6
	move.w	(a4)+,d7
	sub.w	d6,d4
	add.w	d2,d6
	sub.w	d7,d5
	add.w	d3,d7
	bsr.w	Hitbox_FindEnemyBox
	beq.s	loc_0069E6
	adda.w	(a4),a4
	movea.w	(a4)+,a0
	jmp	(a0)
loc_0069CE:
	sub.w	(g_CameraX).w,d4
	cmpi.w	#$140,d4
	bcc.w	Obj_UnlinkFree
	sub.w	(g_CameraY).w,d5
	cmpi.w	#$E0,d5
	bcc.w	Obj_UnlinkFree
loc_0069E6:
	movea.l	$C(a6),a0
	move.l	(a0)+,$C(a6)
	movea.l	(a0),a0
	subq.b	#$1,(g_SprNodesFree).w
	bmi.s	loc_006A0E
	movea.w	(g_SprFrontEnd).w,a1
	addi.w	#$12,(g_SprFrontEnd).w
	move.w	a1,d0
	move.l	a0,-(a1)
	move.w	$22(a6),d4
	movem.w	d0/d2-d4,-(a1)
	rts
loc_006A0E:
	clr.b	(g_SprNodesFree).w
	rts
; ---------------------------------------------------------------------------
; VM5_RunObject - object handler of a wave director (a5 VM): ends it when a newer wave generation started; while g_ObjPopulation is below its cap counts its timer $2E down by g_WaveTimerStep and, when it runs out, resumes its a5 script at $1E
; In:  a6 = wave object (+$A serial, +$12/+$14 population cap one/two players, +$1E resume pc, +$2E timer)
; Out: rts (waiting), or the script runs until an op yields (saved sp g_VM5SavedSP); VM5_EndWave when g_WaveGen > its serial
; Evidence: installed by Wave_Start ($8A08 with this address); two players = bit 0 of both records' +$25 flags ($F675, $F6B5); 117 corpus runs
; ---------------------------------------------------------------------------
VM5_RunObject:
	move.w	(g_WaveGen).w,d0
	cmp.w	$A(a6),d0
	bgt.w	VM5_EndWave
	move.w	$12(a6),d0
	btst	#$0,(g_P1Flags).w
	beq.s	loc_006A38
	btst	#$0,(g_P2Flags).w
	beq.s	loc_006A38
	move.w	$14(a6),d0
loc_006A38:
	cmp.w	(g_ObjPopulation).w,d0
	ble.s	loc_006A4C
	move.l	$2E(a6),d0
	sub.l	(g_WaveTimerStep).w,d0
	ble.s	loc_006A4E
	move.l	d0,$2E(a6)
loc_006A4C:
	rts
loc_006A4E:
	move.l	a7,(g_VM5SavedSP).w
	movea.l	$1E(a6),a5
; ---------------------------------------------------------------------------
; VM5_Next - a5-VM NEXT: fetches the next op word from a5 and jumps to that handler (the not-waiting path of the Op5_WaitPopulation ops)
; In:  a5 = script, a6 = wave object
; Out: the next op
; Evidence: movea.w (a5)+,a0 / jmp (a0); bge targets in Op5_WaitPopulation and Op5_WaitPopulation0-6
; ---------------------------------------------------------------------------
VM5_Next:
	movea.w	(a5)+,a0
	jmp	(a0)
