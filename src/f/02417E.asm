; ---------------------------------------------------------------------------
; Stage2_LiftArriveTop - turns the lift section into the stage 2 boss arena when the lift reaches the top: swaps the V-int object to Stage2_ArenaVIntScroll, sets arena mode (g_SceneVars = 1), the tilting-floor terrain routines and the arena's pivot and floor values, then starts the boss
; In:  -
; Out: $F5C2 object handler = Stage2_ArenaVIntScroll; g_TerrainFn/Alt = Coll_Stage2TiltFloor/Rest; $F59E/$F5A6 = $A8 (pivot x), $F5AA = $C4 (floor y), $F5B4 = -$46, $F5B8 = -$12C; jmp $025BE2
; Evidence: only caller Stage2_LiftUpdate; TAS f32065 (one frame), the same frame $025BE2 first runs; png f032400 shows the arena under the boss machine (Two-Face boss: the continue scenario idles there)
; ---------------------------------------------------------------------------
Stage2_LiftArriveTop:
	addq.b	#$1,(g_VIntOff).w
	movea.w	(g_SceneVars2+$24).w,a0
	move.l	#$2456A,(a0)
	move.w	#$1,(g_SceneVars).w
	subq.b	#$1,(g_VIntOff).w
	move.l	#$247C6,(g_TerrainFn).w
	move.l	#$247F8,(g_TerrainFnAlt).w
	move.w	#$FFBA,(g_SceneVars2+$16).w
	move.w	#$FED4,(g_SceneVars2+$1A).w
	move.w	#$A8,(g_SceneVars2+$8).w
	move.w	#$A8,(g_SceneVars2).w
	move.w	#$A8,(g_SceneVars2+$8).w
	move.w	#$C4,(g_SceneVars2+$C).w
	jmp	(Boss_Stage2_Spawn).l
; ---------------------------------------------------------------------------
; Stage2_ArenaRequestTilt - boss-script native: when the arena is live ($F5F6) and the floor is not already turning ($F5B0 = 0), arms the next floor tilt in 4 frames ($F5C0) and resets the debris timer $F5F5
; In:  -
; Out: $F5C0 = 4 when accepted
; Evidence: dc.l at $026B58 and $026C52 in the boss's a4 script Scr4_026A46; Stage2_UpdateScroll counts $F5C0 down (rumbling) and then flips the tilt target; executed in the continue/game-over scenarios and fuzz-25200..32400
; ---------------------------------------------------------------------------
Stage2_ArenaRequestTilt:
	tst.b	(g_Stage2TiltAllowed).w
	beq.s	loc_0241DC
	tst.l	(g_SceneVars2+$12).w
	beq.s	loc_0241DE
loc_0241DC:
	rts
loc_0241DE:
	move.w	#$4,(g_SceneVars2+$22).w
	clr.b	(g_Stage2DebrisTimer).w
	rts
; ---------------------------------------------------------------------------
; Stage2_LiftInstallVInt - puts the lift's scroll handler Stage2_LiftVIntScroll on the V-int object list and keeps that object in $F5C2 (Stage2_LiftArriveTop later swaps its handler)
; In:  -
; Out: $F5C2 = the V-int object
; Evidence: VIntObj_New with a0 = Stage2_LiftVIntScroll; called once by the scene $32F14 at its start (TAS f21125)
; ---------------------------------------------------------------------------
Stage2_LiftInstallVInt:
	lea.l	(Stage2_LiftVIntScroll).l,a0
	jsr	(VIntObj_New).l
	move.w	a6,(g_SceneVars2+$24).w
	rts
; ---------------------------------------------------------------------------
; Stage2_UpdateScroll - per-frame scroll of the lift scene: view y = g_MapScrollY + shake; lift mode scrolls layer A and the shaft columns; arena mode runs the floor tilt (warning rumble, slope easing toward +-$1800 with rumbling and random debris effects, a big shake on arrival) and eases the pivot x, then draws the tilt
; In:  -
; Out: $E012 = view y; layer A or the arena scroll buffers updated
; Evidence: called every frame by the scene loop ($32FD4); g_SceneVars = 0 lift / 1 arena; $F5C0 countdown -> Player_SetBits300All and target slope g_SceneVars+8 = $1800 (pivot $118) or $E800 (pivot $28); Scr4_024360 (8 a4 scripts) spawned at y camera+$E0; TAS f21158-f35416; png f033600 shows the floor tilted
; ---------------------------------------------------------------------------
Stage2_UpdateScroll:
	move.l	(g_MapScrollY).l,d0
	add.l	(g_ShakeY).w,d0
	move.l	d0,(g_Stage2ViewY).w
	move.w	(g_SceneVars).w,d0
	bne.s	loc_02422C
	lea.l	(g_LayerA).l,a6
	move.w	(g_Stage2ViewY).w,d0
	sub.w	$2(a6),d0
	move.w	d0,$6(a6)
	jsr	(Layer_ScrollStep).l
	bra.w	Stage2_LiftColumnScroll
loc_02422C:
	tst.w	(g_SceneVars2+$22).w
	beq.s	loc_02426E
	subq.w	#$1,(g_SceneVars2+$22).w
	bne.s	loc_024268
	jsr	(Player_SetBits300All).l
	clr.l	(g_SceneVars2+$4).w
	clr.l	(g_SceneVars+$2).w
	tst.w	(g_SceneVars+$8).w
	bpl.s	loc_02425A
	move.w	#$1800,(g_SceneVars+$8).w
	move.w	#$118,(g_SceneVars2).w
	bra.s	loc_02426E
loc_02425A:
	move.w	#$E800,(g_SceneVars+$8).w
	move.w	#$28,(g_SceneVars2).w
	bra.s	loc_02426E
loc_024268:
	jsr	(Shake_Rumble).l
loc_02426E:
	move.w	(g_SceneVars+$6).w,d2
	move.w	(g_SceneVars+$8).w,d3
	cmp.w	d2,d3
	beq.w	loc_024324
	move.w	(g_SceneVars+$2).w,d1
	tst.w	d3
	bpl.s	loc_02428C
	subq.w	#$2,d1
	cmp.w	d3,d2
	bgt.s	loc_0242AA
	bra.s	loc_024292
loc_02428C:
	addq.w	#$2,d1
	cmp.w	d3,d2
	blt.s	loc_0242AA
loc_024292:
	clr.l	(g_SceneVars2+$12).w
	move.w	d3,(g_SceneVars+$6).w
	jsr	(Shake_StartBig).l
	jsr	(Player_ClearBits300All).l
	bra.w	loc_024324
loc_0242AA:
	add.w	d1,d2
	move.w	d1,(g_SceneVars+$2).w
	move.w	d2,(g_SceneVars+$6).w
	jsr	(Shake_Rumble).l
	addi.b	#$24,(g_Stage2DebrisTimer).w
	bcc.s	loc_024304
	move.w	#$20,d0
	jsr	(Rand_Below).l
	andi.w	#$1C,d0
	lea.l	(Scr4_024360).l,a4
	movea.l	(a4,d0.w),a4
	move.w	#$A8,d4
	sub.w	(g_SceneVars2).w,d4
	addi.w	#$A8,d4
	move.w	#$50,d0
	jsr	(Rand_Signed).l
	add.w	d0,d4
	swap	d4
	move.l	(g_CameraY).w,d5
	addi.l	#$E00000,d5
	jsr	(Fx_RunScriptNoParent).l
loc_024304:
	move.l	(g_SceneVars2+$12).w,d0
	addi.l	#$2000,d0
	cmpi.l	#$30000,d0
	ble.s	loc_02431C
	move.l	#$30000,d0
loc_02431C:
	move.l	d0,(g_SceneVars2+$12).w
	add.l	d0,(g_SceneVars2+$E).w
loc_024324:
	move.l	#$1200,d0
	move.l	(g_SceneVars2+$4).w,d1
	move.l	(g_SceneVars2+$8).w,d2
	move.l	(g_SceneVars2).w,d3
	cmp.l	d3,d2
	beq.w	loc_02435C
	bmi.s	loc_024348
	sub.l	d0,d1
	add.l	d1,d2
	cmp.l	d3,d2
	bpl.s	loc_024354
	bra.s	loc_024350
loc_024348:
	add.l	d0,d1
	add.l	d1,d2
	cmp.l	d3,d2
	bmi.s	loc_024354
loc_024350:
	moveq	#$0,d1
	move.l	d3,d2
loc_024354:
	move.l	d1,(g_SceneVars2+$4).w
	move.l	d2,(g_SceneVars2+$8).w
loc_02435C:
	bra.w	Stage2_ArenaDrawTilt
