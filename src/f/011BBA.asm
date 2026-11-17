; ---------------------------------------------------------------------------
; BossMeter_Exists - native case callback: returns case 1 when the boss energy meter object exists, else case 0
; In:  none
; Out: d0 = 0 no meter, 1 meter (case index for op $2D40)
; Evidence: two op $2D40 pointers in the coroutine at $030FB8; ran 480 frames in the TAS (75519-76134)
; ---------------------------------------------------------------------------
BossMeter_Exists:
	move.w	(g_BossMeterObj).w,d0
	beq.s	loc_011BC2
	moveq	#$1,d0
loc_011BC2:
	rts
; ---------------------------------------------------------------------------
; BossMeter_AnimRing - advances the meter ring's 16-frame animation and copies that frame's 8 tiles (2 rows of 4, column-major) into the meter tile buffer g_BossMeterTiles
; In:  a4 = meter script block (+$22 animation offset, steps of 8 mod $80)
; Out: g_BossMeterTiles rewritten; a2/a3 preserved
; Evidence: op $2CB2 call in Scr2_011B3E ($11B62); reads Tbl_057892 (tile numbers) and Tbl_057992 (tiles); executed while a boss meter is up (TAS, scenario-level-skip)
; ---------------------------------------------------------------------------
BossMeter_AnimRing:
	move.w	$22(a4),d0
	addq.w	#$8,d0
	andi.w	#$78,d0
	move.w	d0,$22(a4)
	movem.l	a2-a3,-(a7)
	lea.l	(Tbl_057892).l,a0
	lea.l	(Tbl_057992).l,a1
	lea.l	(g_BossMeterTiles).l,a2
	adda.w	d0,a0
	moveq	#$1,d6
loc_011BEC:
	moveq	#$3,d7
loc_011BEE:
	move.w	(a0)+,d0
	lsl.w	#$5,d0
	lea.l	(a1,d0.w),a3
	move.l	(a3)+,(a2)+
	move.l	(a3)+,(a2)+
	move.l	(a3)+,(a2)+
	move.l	(a3)+,(a2)+
	move.l	(a3)+,(a2)+
	move.l	(a3)+,(a2)+
	move.l	(a3)+,(a2)+
	move.l	(a3)+,(a2)+
	lea.l	$20(a2),a2
	dbra	d7,loc_011BEE
	lea.l	-$E0(a2),a2
	lea.l	$78(a0),a0
	dbra	d6,loc_011BEC
	movem.l	(a7)+,a2-a3
	rts
; ---------------------------------------------------------------------------
; BossMeter_DrawPercent - draws the boss's remaining energy as a two-digit percentage (energy x 99 / full, at least 1 while alive) over the ring tiles
; In:  a4 = meter script block (+$25 bit 1: draw nothing at 0 energy)
; Out: digits drawn into g_BossMeterTiles+$50 (tens) and +$90 (ones) by BossMeter_DrawDigit
; Evidence: op $2CB2 call in Scr2_011B3E ($11B6E); g_BossEnergy $16C0 of $2200 at TAS f4800 gives 66, the number shown in the ring
; ---------------------------------------------------------------------------
BossMeter_DrawPercent:
	move.w	(g_BossEnergy).w,d0
	bne.s	loc_011C30
	btst	#$1,$25(a4)
	beq.s	loc_011C42
	rts
loc_011C30:
	mulu.w	#$63,d0
	divu.w	(g_BossEnergyMax).w,d0
	ext.l	d0
	bne.s	loc_011C3E
	moveq	#$1,d0
loc_011C3E:
	divu.w	#$A,d0
loc_011C42:
	move.l	a2,-(a7)
	lea.l	(g_BossMeterTiles+$50).l,a2
	bsr.w	BossMeter_DrawDigit
	lea.l	(g_BossMeterTiles+$90).l,a2
	swap	d0
	bsr.w	BossMeter_DrawDigit
	movea.l	(a7)+,a2
	rts
