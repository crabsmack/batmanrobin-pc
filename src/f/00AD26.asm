; ---------------------------------------------------------------------------
; Tint_Step - moves g_TintLevel 2 toward g_TintTarget once every g_TintDelay+1 calls and returns it
; In:  g_TintLevel, g_TintTarget, g_TintDelay, g_TintDelayCount
; Out: d0.b = g_TintLevel (flags from the final move: Z when 0); d1 trashed
; Evidence: same countdown/reload scheme as Fade_Step's g_FadeDelayCount; callers Tint_StepLine0/1/3 and the stage code $48C36 / $48E80 (which also writes the negated level to lines 0-2); TAS 12,372 frames
; ---------------------------------------------------------------------------
Tint_Step:
	moveq	#-$2,d1
	move.b	(g_TintLevel).w,d0
	cmp.b	(g_TintTarget).w,d0
	beq.s	loc_00AD44
	bpl.s	loc_00AD36
	neg.b	d1
loc_00AD36:
	subq.b	#$1,(g_TintDelayCount).w
	bpl.s	loc_00AD44
	move.b	(g_TintDelay).w,(g_TintDelayCount).w
	add.b	d1,d0
loc_00AD44:
	move.b	d0,(g_TintLevel).w
	rts
; ---------------------------------------------------------------------------
; Pal_CopyBright - copies colours with one brightness offset applied to all three channels through the fade clamp tables
; In:  a0 = source CRAM words, a1 = destination, d0.w = brightness offset (even, -$E black .. +$E white), d7 = colours - 1
; Out: a0/a1 advanced; d0-d2/d6/d7/a2-a4 trashed
; Evidence: a2/a3/a4 = Tbl_00A6A8/Tbl_00A728/Tbl_00A7A8 + d0 (the red/green/blue clamp tables Fade_Step uses), channel nibbles index them; only caller Title_PaletteFadeChannel $049816 (jmp; it sets a reload flag first and takes a1 from its table); entered by jmp, so func_frames misses it, but the exec coverage of idle has $AD4A (the title fade-in)
; ---------------------------------------------------------------------------
Pal_CopyBright:
	lea.l	Tbl_00A6A8(pc),a2
	lea.l	Tbl_00A728(pc),a3
	lea.l	Tbl_00A7A8(pc),a4
	adda.w	d0,a2
	adda.w	d0,a3
	adda.w	d0,a4
	move.w	#$F,d6
loc_00AD60:
	move.w	(a0)+,d0
	move.w	d0,d1
	lsr.l	#$4,d1
	move.l	d1,d2
	lsr.l	#$4,d2
	and.l	d6,d0
	and.l	d6,d1
	and.l	d6,d2
	move.w	(a2,d0.w),d0
	or.w	(a3,d1.w),d0
	or.w	(a4,d2.w),d0
	move.w	d0,(a1)+
	dbra	d7,loc_00AD60
	rts
