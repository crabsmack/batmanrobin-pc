; ---------------------------------------------------------------------------
; Snd_PlaySfx - queues sound effect d0 (1-based) in its g_SndSfxSlots slot unless a higher-priority effect holds that slot; the V-int's Snd_SendSfx hands it to the Z80
; In:  d0 = effect number (0 = none: returns at once)
; Out: slot +0 priority, +1 hold frames, +2.l effect data pointer (Tbl_1E50D6 entry bytes 2, 3 and long 4); d0 trashed, a0/a1 preserved
; Evidence: Tbl_1E50D6 8-byte entries (byte slot offset, byte priority compared with slot +0 by cmp.b/bcs, priority to keep, hold frames, long data into Snd_1E53A6, e.g. entry 1 = 1E FA F5 4B $1E53EE); callers Op2_PlaySound $433E, the a4 op $646E, player and weapon code $103FE/$107E8, $1C5EE, $20866, $41F0E, $499E8; 133 corpus runs
; ---------------------------------------------------------------------------
Snd_PlaySfx:
	subq.w	#$1,d0
	bmi.s	loc_1E5FB8
	movem.l	a0-a1,-(a7)
	lsl.w	#$3,d0
	lea.l	Tbl_1E50D6(pc),a0
	adda.w	d0,a0
	lea.l	(g_SndSfxSlots).l,a1
	moveq	#$0,d0
	move.b	(a0)+,d0
	adda.w	d0,a1
	move.b	(a0)+,d0
	cmp.b	(a1),d0
	bcs.s	loc_1E5FB4
	move.b	(a0)+,(a1)
	move.b	(a0)+,$1(a1)
	move.l	(a0)+,$2(a1)
loc_1E5FB4:
	movem.l	(a7)+,a0-a1
loc_1E5FB8:
	rts
