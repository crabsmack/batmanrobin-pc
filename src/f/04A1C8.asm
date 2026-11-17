; ---------------------------------------------------------------------------
; BatmobileIntro_Animate - per-frame native of the shared stage-opening object (the Batmobile racing through the night): every 4th frame rotates four 3-colour groups of palette line 0 (colours 2-13: the speed streaks), and on 2 of every 6 frames shows plane A 320 px to the right and 1 px lower (the car's bump)
; In:  a6 = the opening object (+$2A palette timer 3..0, +$2C bump timer 5..0)
; Out: g_HScrollA = 0 or -$140, g_VScrollA = 0 or 1, g_Pal0Reload set after a rotation; d0-d2/d7/a0 trashed
; Evidence: Scr2_04A1A8 (spawned by BatmobileIntro_Run $4A180) loops StepCoroutine0 / StepVelocity / ApplyMove / Op2_CallNativeRel +6 -> here; lea $FFE0D6 (= g_PalSource+4), movem (d0,d1,d2) -> (d2,d0,d1) x4; btst #2 of $2D(a6); runs 140 frames per stage start (scenario-two-player f609-748; png w32 run f680/f700: the Batmobile on blue streaks)
; ---------------------------------------------------------------------------
BatmobileIntro_Animate:
	subq.w	#$1,$2A(a6)
	bcc.s	loc_04A1EE
	move.w	#$3,$2A(a6)
	st.b	(g_Pal0Reload).w
	lea.l	($FFFFE0D6).l,a0
	moveq	#$3,d7
loc_04A1E0:
	movem.w	(a0),d0-d2
	move.w	d2,(a0)+
	move.w	d0,(a0)+
	move.w	d1,(a0)+
	dbra	d7,loc_04A1E0
loc_04A1EE:
	moveq	#$0,d0
	moveq	#$0,d1
	subq.w	#$1,$2C(a6)
	bcc.s	loc_04A1FE
	move.w	#$5,$2C(a6)
loc_04A1FE:
	btst	#$2,$2D(a6)
	beq.s	loc_04A20C
	move.w	#$FEC0,d0
	moveq	#$1,d1
loc_04A20C:
	move.w	d0,(g_HScrollA).w
	move.w	d1,(g_VScrollA).w
	rts
