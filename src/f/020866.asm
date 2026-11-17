; ---------------------------------------------------------------------------
; Stage1_BlastSignChained - street trigger callback: blasts the bank sign nearest the trigger x (Stage1_BlastSignAt), then returns into the trigger entry's second callback long
; In:  d4 = trigger x, a1 = after the callback long in the trigger entry (Trigger_Fire)
; Out: continues at the extra callback (rts to the pushed long, here $01ACE0) with d0 = 0
; Evidence: move.l (a1),-(a7) pushes the entry's next long; entries $01622E/$016252/... in Tbl_015F5A are size $12 with $00020866 then $0001ACE0; TAS f2896-f4284 (9 frames)
; ---------------------------------------------------------------------------
Stage1_BlastSignChained:
	move.l	(a1),-(a7)
; ---------------------------------------------------------------------------
; Stage1_BlastSignAt - blasts the bank sign slot nearest world x d4: sets its wanted state to 8 (blasted) for Stage1_VIntFacade, throws debris and plays sound $13
; In:  d4 = world x
; Out: d0 = 0; g_SceneVars+6+slot = 8
; Evidence: facade x = d4*35/32 - g_Stage1Segment*$168 split at 0/$A8/$150 into slots 0-3 (as Stage1_UpdateStreetScroll); Stage1_SpawnSignDebris at y $96; Snd_PlaySfx #$13; entered from Stage1_BombingBlastLeftSign/RightSign (bra)
; ---------------------------------------------------------------------------
Stage1_BlastSignAt:
	move.w	d4,d0
	move.w	d0,d1
	asr.w	#$3,d1
	add.w	d1,d0
	asr.w	#$2,d1
	sub.w	d1,d0
	move.w	(g_Stage1Segment).l,d1
	mulu.w	#$168,d1
	sub.w	d1,d0
	moveq	#$0,d1
	tst.w	d0
	ble.s	loc_020898
	moveq	#$1,d1
	cmpi.w	#$A8,d0
	ble.s	loc_020898
	moveq	#$2,d1
	cmpi.w	#$150,d0
	ble.s	loc_020898
	moveq	#$3,d1
loc_020898:
	lea.l	(g_SceneVars+$6).l,a0
	move.b	#$8,(a0,d1.w)
	move.w	#$96,d5
	move.w	#$2000,d6
	bsr.w	Stage1_SpawnSignDebris
	move.w	#$13,d0
	jsr	(Snd_PlaySfx).l
	moveq	#$0,d0
	rts
