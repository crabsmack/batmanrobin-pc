; ---------------------------------------------------------------------------
; ToyRing_StartBreakUp - starts breaking a toy ring into its figures: releases the figures' sprite frames and loads the walk round the ring into the script args: g_VM2Arg0 = first angle (spin +$14 - step/2), g_VM2Arg1 = angle step +$18, g_VM2Arg2/g_VM2Arg3 = the ring's current x/y radii
; In:  a6 = ring object, $E(a6) = ring block (+0 radius, +$8 squash, +$14 spin, +$18 step)
; Out: g_VM2Arg0-3 set; frames released (ToyRing_ReleaseFrames)
; Evidence: bsr ToyRing_ReleaseFrames; radii = r - squash/2 and r + squash as in ToyRing_Update; Op2_CallNativeRel before the ToyRing_NextFigurePos loops in Scr2_03926C ($0395F8, $039616, $039630, $03964A, $039664); TAS 25 frames
; ---------------------------------------------------------------------------
ToyRing_StartBreakUp:
	bsr.w	ToyRing_ReleaseFrames
	movea.w	$E(a6),a0
	move.l	$18(a0),d0
	move.l	d0,(g_VM2Arg1).w
	asr.l	#$1,d0
	neg.l	d0
	add.l	$14(a0),d0
	move.l	d0,(g_VM2Arg0).w
	move.l	(a0),d0
	move.l	d0,d1
	move.l	$8(a0),d2
	add.l	d2,d1
	asr.l	#$1,d2
	sub.l	d2,d0
	move.l	d0,(g_VM2Arg2).w
	move.l	d1,(g_VM2Arg3).w
	rts
; ---------------------------------------------------------------------------
; ToyRing_NextFigurePos - switch native of the ring break-up loop: while figures are left (a4 +$24 count - 1), puts the anchor $46/$4A(a6) at the next figure's place on the ring (x + Arg2 * cos, y + Arg3 * sin of Arg0) and steps the angle by Arg1
; In:  a6 = ring object, g_VM2Arg0-3 from ToyRing_StartBreakUp
; Out: d0 = 0 next position in the anchor (the script spawns a figure there with Spawn_AtAnchor or an explosion with Op2_FxPosAnchor), 1 = all done (count cleared)
; Evidence: subq #1,$24(a4); Tbl_059B22 cos (+$100) / sin; Op2_CallNativeRelSwitch at $0395FC-$039668 followed by Op2_CallNative $8D52 (Spawn_AtAnchor) with figure spawners $038CF4/$038D48/$038BBC/$038B80, or Op2_FxSpawnRegs at the death path $039664
; ---------------------------------------------------------------------------
ToyRing_NextFigurePos:
	subq.w	#$1,$24(a4)
	bpl.s	loc_0376CE
	clr.w	$24(a4)
	moveq	#$1,d0
	rts
loc_0376CE:
	lea.l	(Tbl_059B22).l,a0
	move.w	(g_VM2Arg0).w,d0
	andi.w	#$3FE,d0
	adda.w	d0,a0
	move.w	(g_VM2Arg2).w,d0
	muls.w	$100(a0),d0
	add.l	$3E(a6),d0
	move.l	d0,$46(a6)
	move.w	(g_VM2Arg3).w,d0
	muls.w	(a0),d0
	add.l	$42(a6),d0
	move.l	d0,$4A(a6)
	move.l	(g_VM2Arg1).w,d0
	add.l	d0,(g_VM2Arg0).w
	moveq	#$0,d0
	rts
