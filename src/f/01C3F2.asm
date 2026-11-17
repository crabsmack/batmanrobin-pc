; ---------------------------------------------------------------------------
; Boss_JokerBalloon_VIntSplit - V-int object of the Joker balloon boss: scrolls plane A so its balloon picture sits at the boss's position above screen line (y - g_ViewY), and arms an H-int on that line that puts the stage's own plane-A scroll back for the rest of the screen
; In:  a6 = the V-int object (+$18/+$1A = balloon x/y from Boss_JokerBalloon_SyncSplit), a0/a1 = VDP control/data
; Out: VDP reg 10 = line - 1, H-int on (reg 0 = $14) with the g_HIntCode jmp stub -> Boss_JokerBalloon_HIntRestore; balloon V scroll $1A0 - line and H scroll x - g_ViewX - $7F written to the VDP (busy V-int) or swapped into g_VScrollA/g_HScrollA, the stage values saved in g_SceneVars/+2; reg 0 = $04 (H-int off) and nothing else when line <= 0
; Evidence: installed as the handler of a V-int list object by Boss_JokerBalloon_StartSplit; VSRAM write $40000010, HScroll $7C000002 (VRAM $FC00); TAS f18913-f21099 (2187 frames), the Joker balloon fight at the end of the parade truck (shots f19200/f19260)
; ---------------------------------------------------------------------------
Boss_JokerBalloon_VIntSplit:
	move.w	$1A(a6),d0
	sub.w	(g_ViewY).w,d0
	neg.w	d0
	addi.w	#$A0,d0
	cmpi.w	#$A0,d0
	blt.s	loc_01C40C
	move.w	#$8004,(a0)
	rts
loc_01C40C:
	move.w	#$8A9F,d1
	sub.w	d0,d1
	addi.w	#$100,d0
	move.w	$18(a6),d2
	sub.w	(g_ViewX).w,d2
	subi.w	#$7F,d2
	tst.b	(g_FrameBusy).w
	beq.s	loc_01C43A
	move.l	#$40000010,(a0)
	move.w	d0,(a1)
	move.l	#$7C000002,(a0)
	move.w	d2,(a1)
	bra.s	loc_01C44E
loc_01C43A:
	move.w	(g_HScrollA).w,(g_SceneVars).w
	move.w	(g_VScrollA).w,(g_SceneVars+$2).w
	move.w	d2,(g_HScrollA).w
	move.w	d0,(g_VScrollA).w
loc_01C44E:
	move.w	d1,(a0)
	move.w	#$4EF9,(g_HIntCode).l
	move.l	#$1C468,(g_HIntOperand).l
	move.w	#$8014,(a0)
	rts
; ---------------------------------------------------------------------------
; Boss_JokerBalloon_HIntRestore - H-int handler of the Joker balloon split: at the split line writes the saved stage plane-A scroll (g_SceneVars+2 V, g_SceneVars H) back to VSRAM 0 and VRAM $FC00 and turns further H-ints off (reg 10 = $FF)
; In:  H-int (via the jmp stub at g_HIntCode)
; Out: rte
; Evidence: target long $1C468 stored at g_HIntOperand by Boss_JokerBalloon_VIntSplit; same VDP commands as its busy-V-int branch
; ---------------------------------------------------------------------------
Boss_JokerBalloon_HIntRestore:
	move.l	#$40000010,($C00004).l
	move.w	(g_SceneVars+$2).w,($C00000).l
	move.l	#$7C000002,($C00004).l
	move.w	(g_SceneVars).w,($C00000).l
	move.w	#$8AFF,($C00004).l
	rte
; ---------------------------------------------------------------------------
; Boss_JokerBalloon_SyncSplit - balloon-boss native: copies the boss's x/y ($3E/$42) to its scroll-split V-int object ($1E(a6) +$18/+$1A) once a frame
; In:  a6 = the balloon boss (+$1E = the V-int object)
; Out: -
; Evidence: op $2CB2 (CallNativeRel) at $1C5D8 in the boss's frame loop (after RunAnim and Draw); 3 corpus runs incl. the TAS
; ---------------------------------------------------------------------------
Boss_JokerBalloon_SyncSplit:
	movea.w	$1E(a6),a0
	move.w	$3E(a6),$18(a0)
	move.w	$42(a6),$1A(a0)
	rts
; ---------------------------------------------------------------------------
; Boss_JokerBalloon_HideSplit - balloon-boss native: sets the split object's y to 0, so its next V-int turns the H-int split off and leaves plane A to the stage
; In:  a6 = the balloon boss (+$1E = the V-int object)
; Out: -
; Evidence: op $2CB2 at $1D8B2 in Scr2_01D470 (the boss's later script); Boss_JokerBalloon_VIntSplit exits early when line <= 0; TAS only
; ---------------------------------------------------------------------------
Boss_JokerBalloon_HideSplit:
	movea.w	$1E(a6),a0
	clr.w	$1A(a0)
	rts
; ---------------------------------------------------------------------------
; Boss_JokerBalloon_StartSplit - balloon-boss native: allocates a V-int list object, gives it the handler Boss_JokerBalloon_VIntSplit with y 0 and keeps it in $1E(a6)
; In:  a6 = the balloon boss
; Out: d0 = 0, or 1 when the object pool is empty (Z clear: the script retries)
; Evidence: VIntObj_NewIdle (pool block linked on the V-int list at $DE9C/$DEA4, handler $8BEC replaced by $1C3F2); op $2CB2 at $1C50C at the head of Scr2_01C4E6; TAS only
; ---------------------------------------------------------------------------
Boss_JokerBalloon_StartSplit:
	move.l	a6,-(a7)
	jsr	(VIntObj_NewIdle).l
	movea.l	a6,a0
	movea.l	(a7)+,a6
	bne.s	loc_01C4D2
	move.w	a0,$1E(a6)
	clr.w	$1A(a0)
	move.l	#$1C3F2,(a0)
	moveq	#$0,d0
	rts
loc_01C4D2:
	moveq	#$1,d0
	rts
; ---------------------------------------------------------------------------
; Boss_JokerBalloon_Spawn - trigger callback of the parade truck: spawns the Joker balloon boss (boss class, 7 pool blocks) running Scr2_01C4E6
; In:  d4/d5 = x/y (trigger entry Tbl_0166EA +$24C)
; Out: as Obj_SpawnBossN
; Evidence: d7 = 7 then Obj_SpawnBossN; the object at $B72C runs Scr2_01C4E6 from f18840 and loses energy 7328 -> 0 by f20460 while the balloon with the Joker in its gondola is on screen (shot f19260)
; ---------------------------------------------------------------------------
Boss_JokerBalloon_Spawn:
	lea.l	(Scr2_01C4E6).l,a2
	move.w	#$7,d7
	jmp	(Obj_SpawnBossN).l
