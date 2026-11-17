; ---------------------------------------------------------------------------
; Arc_PlanToArgs - a4 native: plans a gravity arc for the current a4 object to the target point given in the script
; In:  a5 = object, a4 = script: dc.w target x, y, rise height, rise frames
; Out: a4 past the four words; object $26 gravity, $42 y speed, $3E x speed, $56/$58 start x/y
; Evidence: 58 script uses after the a4 native-call op $5438 ($2790E...); followed by Arc_Step calls; d0-d1 target, rest as Arc_Plan
; ---------------------------------------------------------------------------
Arc_PlanToArgs:
	move.w	(a4)+,d0
	move.w	(a4)+,d1
	bra.s	Arc_Plan
; ---------------------------------------------------------------------------
; Arc_PlanToMark - a4 native: plans a gravity arc for the current a4 object to the point g_ArcTargetX/Y
; In:  a5 = object, g_ArcTargetX/Y (stored by a2 op $5378), a4 = script: dc.w rise height, rise frames
; Out: as Arc_PlanToArgs
; Evidence: script uses at $1AB84, $1B528, $1F0EA, $42CF8, each followed by `$5438 Arc_Step`
; ---------------------------------------------------------------------------
Arc_PlanToMark:
	move.w	(g_ArcTargetX).w,d0
	move.w	(g_ArcTargetY).w,d1
; ---------------------------------------------------------------------------
; Arc_Plan - solves gravity, initial upward speed and x speed so the object rises d2 pixels in d3 frames and lands on the target
; In:  d0/d1 = target x/y, a5 = object ($2E/$32 x/y), a4 = script at the height and frame words
; Out: $26 = gravity, $42 = -(gravity * frames), $3E = x speed (16.16)
; Evidence: gravity = height << 16 / (frames * (frames + 1) / 2); fall time from the target's y distance; x speed = dx << 16 / total frames; d2-d5 preserved
; ---------------------------------------------------------------------------
Arc_Plan:
	movem.l	d2-d5,-(a7)
	movea.l	a5,a6
	move.w	(a4)+,d2
	move.w	(a4)+,d3
	move.w	d3,d4
	addq.w	#$1,d4
	asr.w	#$1,d4
	mulu.w	d3,d4
	moveq	#$0,d5
	move.w	d2,d5
	swap	d5
	divu.w	d4,d5
	ext.l	d5
	move.l	d5,$26(a6)
	move.l	d5,d6
	mulu.w	d3,d6
	move.l	d6,$42(a6)
	lsr.l	#$8,d6
	sub.w	$32(a6),d1
	bpl.s	loc_0092B4
	neg.w	d1
	ext.l	d1
	lsl.l	#$8,d1
	divu.w	d6,d1
	mulu.w	d1,d5
	add.l	d5,$42(a6)
	bra.s	loc_0092BA
loc_0092B4:
	ext.l	d1
	lsl.l	#$8,d1
	divu.w	d6,d1
loc_0092BA:
	neg.l	$42(a6)
	add.w	d3,d3
	add.w	d1,d3
	asr.w	#$2,d1
	sub.w	d1,d3
	sub.w	$2E(a6),d0
	ext.l	d0
	asl.l	#$8,d0
	divs.w	d3,d0
	ext.l	d0
	asl.l	#$8,d0
	move.l	d0,$3E(a6)
	move.w	$2E(a6),$56(a6)
	move.w	$32(a6),$58(a6)
	movem.l	(a7)+,d2-d5
	rts
; ---------------------------------------------------------------------------
; Arc_Step - a4 native: moves the object one frame along its arc and returns its x/y
; In:  a6 = object
; Out: d4/d5 = new x/y words; $2E += $3E, $42 += $26, $32 += $42
; Evidence: script uses after op $5438 at $1AB98, $1B53C, $26AF6, $3DA0A, $42D10
; ---------------------------------------------------------------------------
Arc_Step:
	move.l	$3E(a6),d4
	add.l	d4,$2E(a6)
	move.w	$2E(a6),d4
	move.l	$26(a6),d5
	add.l	$42(a6),d5
	move.l	d5,$42(a6)
	add.l	d5,$32(a6)
	move.w	$32(a6),d5
	rts
; ---------------------------------------------------------------------------
; Wave_EndAll - native callback: starts a new wave generation, so every running wave-director object ends on its next frame
; In:  none
; Out: d0 = 0
; Evidence: g_WaveSerial++ and g_WaveGen = it; the wave handler $6A14 ends objects whose serial $A is below g_WaveGen; script uses after op $2CBC ($1C824, $1CFA2, $20CEE, $3B2EC)
; ---------------------------------------------------------------------------
Wave_EndAll:
	addq.w	#$1,(g_WaveSerial).w
	move.w	(g_WaveSerial).w,(g_WaveGen).w
	moveq	#$0,d0
	rts
; ---------------------------------------------------------------------------
; Wave_StartScriptArg - native callback: starts a wave-director object running the a5 script given by the script's dc.l
; In:  a2 = script at the dc.l, d4/d5 = position
; Out: a2 past the dc.l; a1-a6 preserved
; Evidence: 38 script uses after op $2CBC ($1C7E6...); calls Wave_Start
; ---------------------------------------------------------------------------
Wave_StartScriptArg:
	movea.l	(a2)+,a0
	movem.l	a1-a6,-(a7)
	movea.l	a0,a5
	bsr.w	Wave_Start
	movem.l	(a7)+,a1-a6
	rts
; ---------------------------------------------------------------------------
; Wave_StartFromEntry - level trigger callback: starts a wave-director object with the a5 script the entry points to
; In:  a1 = trigger entry at its script pointer, d4/d5 = position
; Out: as Wave_Start
; Evidence: 120 uses in the stage trigger tables ($16046..., Tbl_015F5A and others read at pc $932C), entries `dc.l Wave_StartFromEntry, script`
; ---------------------------------------------------------------------------
Wave_StartFromEntry:
	movea.l	(a1),a5
