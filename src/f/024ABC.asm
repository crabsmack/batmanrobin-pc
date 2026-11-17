; ---------------------------------------------------------------------------
; Flight_CopyBandLines - corridor helper: copies d7+1 lines of the middle wall: H-scroll from the band table (high words of the longs at a4) and a constant V-scroll d2, cut at the remaining line budget (then it also ends Flight_DrawCorridor by dropping its return address)
; In:  a0 = H lines, a1 = V lines, a4 = band longs, d2 = V value, d7 = lines - 1
; Out: a0/a1/a4 advanced; g_SceneVars+$10 reduced
; Evidence: bsr from Flight_DrawCorridor ($0249FE); addq.l #4,a7 when the budget runs out; TAS f35433-f47954
; ---------------------------------------------------------------------------
Flight_CopyBandLines:
	sub.w	d7,(g_SceneVars+$10).w
	bgt.s	loc_024AD0
	add.w	(g_SceneVars+$10).w,d7
	addq.l	#$4,a7
	bra.s	loc_024AD0
loc_024ACA:
	move.w	(a4)+,(a0)+
	move.w	d2,(a1)+
	addq.l	#$2,a4
loc_024AD0:
	dbra	d7,loc_024ACA
	rts
; ---------------------------------------------------------------------------
; Enemy_BarrelThug_SpawnA - lift trigger callback: spawns the barrel-carrying thug Scr2_024AE2 (score $64, energy $50/$C0, drop action $25; falls in with op $1CDC, first move after timer $1E0; killed, it drops its barrel)
; In:  d4/d5 = trigger x/y
; Out: d0 = 0 when spawned
; Evidence: dc.l entries in Tbl_02105A ($0212A4 ...) and wave script Tbl_021B26 ($021CF2); shared body $024CC8 (coroutines, Enemy_BarrelThug_ThrowBarrel); shot f28320: green-suited thug holding a barrel overhead on the girder (object at x 224 y 53 in the body)
; ---------------------------------------------------------------------------
Enemy_BarrelThug_SpawnA:
	lea.l	(Scr2_024AE2).l,a2
	jmp	(Obj_SpawnEnemy).l
