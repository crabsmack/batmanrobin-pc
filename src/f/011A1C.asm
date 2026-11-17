; ---------------------------------------------------------------------------
; HUD_UpdateGoSign - per-frame GO! sign: slides it in and brakes, clears g_CameraLockX when it stops, waits for the camera to scroll, holds, slides it off right and releases its sprite; draws it animated while active
; In:  none (g_GoSignState: 0 off, -1 moving, >0 hold timer)
; Out: g_GoSign* updated; GO! frame ($2C88 + 8-frame animation by g_GoSignAnim) drawn with Spr_DrawDirect at x+$20, y $90
; Evidence: called by HUD_Update every frame (TAS 532-130772); GO! visible in TAS f1460 after Camera_LockHere arenas; the hold timer $47 counts down by 2 to -1, which re-enters the moving branch with a positive speed until it reaches 4.0 and Spr_ReleaseFrame
; ---------------------------------------------------------------------------
HUD_UpdateGoSign:
	move.w	(g_GoSignState).w,d0
	beq.w	loc_011AC8
	bmi.s	loc_011A2E
	subq.w	#$2,d0
	move.w	d0,(g_GoSignState).w
	bra.s	loc_011A82
loc_011A2E:
	move.l	(g_GoSignSpeed).w,d0
	bne.s	loc_011A42
	move.w	(g_GoSignCameraX).w,d0
	cmp.w	(g_CameraX).w,d0
	bcc.s	loc_011A82
	bsr.s	HUD_GoSignLeave
	bra.s	loc_011A82
loc_011A42:
	bpl.s	loc_011A5A
	addi.l	#$1A00,d0
	bmi.s	loc_011A7A
	moveq	#$0,d0
	tst.b	(g_GoSignKeepLock).w
	bne.s	loc_011A7A
	move.l	d0,(g_CameraLockX).w
	bra.s	loc_011A7A
loc_011A5A:
	addi.l	#$1A00,d0
	cmpi.l	#$40000,d0
	blt.s	loc_011A7A
	clr.w	(g_GoSignState).w
	move.w	(g_GoSignFrame).w,d2
	clr.w	(g_GoSignFrame).w
	jmp	(Spr_ReleaseFrame).l
loc_011A7A:
	move.l	d0,(g_GoSignSpeed).w
	add.l	d0,(g_GoSignX).w
loc_011A82:
	move.w	#$2C88,d1
	move.w	(g_GoSignAnim).w,d0
	addq.w	#$1,d0
	cmpi.w	#$20,d0
	blt.s	loc_011A96
	subi.w	#$20,d0
loc_011A96:
	move.w	d0,(g_GoSignAnim).w
	lsr.w	#$2,d0
	mulu.w	#$A,d0
	add.w	d0,d1
	move.w	(g_GoSignFrame).w,d2
	jsr	(Spr_ChangeFrame).l
	move.w	d1,(g_GoSignFrame).w
	beq.s	loc_011AC8
	move.w	(g_GoSignX).w,d4
	addi.w	#$20,d4
	move.w	#$90,d5
	move.w	#$8000,d6
	jmp	(Spr_DrawDirect).l
loc_011AC8:
	rts
; ---------------------------------------------------------------------------
; BossMeter_Show - native callback for a boss: sets the boss energy meter's full and current energy from the boss's $52 and starts the meter (restarts its script, or spawns it)
; In:  a6 = boss object (+$52 energy)
; Out: g_BossEnergyMax = g_BossEnergy = $52(a6); d0 = 0 (or the spawn result)
; Evidence: pointed to by 12 boss scripts through op $2CBC ($0180A6, $01C528, $01EE04, $025C4C, ...); executed in the TAS and scenario-level-skip; TAS f4800 shows the ring meter at top centre during the stage 1 balloon boss, with g_BossEnergy falling $2200 -> 0 over frames 4560-6000
; ---------------------------------------------------------------------------
BossMeter_Show:
	move.w	$52(a6),d0
	move.w	d0,(g_BossEnergyMax).w
	move.w	d0,(g_BossEnergy).w
	move.w	(g_BossMeterObj).w,d0
	beq.s	BossMeter_Spawn
	movea.w	d0,a0
	movea.w	$A(a0),a0
	move.l	#$11B3E,$2A(a0)
	moveq	#$0,d0
	rts
