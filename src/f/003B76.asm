; ---------------------------------------------------------------------------
; Op2_GosubVector2Plus - a2 op: calls vector 2 plus a word displacement as a subroutine, returning to the op after the displacement
; In:  a2 = script: word displacement
; Out: (sp) = a2 + 2; NEXT at $3A(a6) + displacement
; Evidence: pea.l $2(a2) / movea.w (a2),a2 / adda.l $3A(a6),a2; 19 corpus runs; $1B06A `3B76 0004 2D22 001C`
; ---------------------------------------------------------------------------
Op2_GosubVector2Plus:
	pea.l	$2(a2)
	movea.w	(a2),a2
	adda.l	$3A(a6),a2
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_AllocSprBlock - a2 op: allocates an extra sprite block from the object pool, stores its handle in an a6 field and makes it the current a5 (attr g_SceneSprAttr, depth 1, the first $2A bytes cleared); with the pool empty it backs up and yields to retry next frame
; In:  a2 = script: word offset of the handle field in a6
; Out: a5 = the new block, NEXT; or Op2_Yield with a2 on this op
; Evidence: bsr ObjPool_Alloc / move.w a5,(a6,d0.w) / move.w #1,$3A(a5) / g_SceneSprAttr -> $38(a5); failure path subq.l #2,a2 / bra Op2_Yield; 13 corpus runs; $180B6 `3B84 000E 0B16 18C0 0B4A 8000 3BF6` (allocate, set frame/attr, back to the main block)
; ---------------------------------------------------------------------------
Op2_AllocSprBlock:
	bsr.w	ObjPool_Alloc
	bne.s	loc_003BBC
	move.w	(a2)+,d0
	movea.l	a0,a5
	move.w	a5,(a6,d0.w)
	move.w	#$1,$3A(a5)
	move.w	(g_SceneSprAttr).l,$38(a5)
	moveq	#$0,d0
	move.l	d0,(a0)+
	move.l	d0,(a0)+
	move.l	d0,(a0)+
	move.l	d0,(a0)+
	move.l	d0,(a0)+
	move.l	d0,(a0)+
	move.l	d0,(a0)+
	move.l	d0,(a0)+
	move.l	d0,(a0)+
	move.l	d0,(a0)+
	move.w	d0,(a0)+
	movea.w	(a2)+,a0
	jmp	(a0)
loc_003BBC:
	subq.l	#$2,a2
	bra.w	Op2_Yield
; ---------------------------------------------------------------------------
; Op2_LoadA5Handle - a2 op: makes the sprite block whose handle is in an a6 field the current a5, so the sprite and animation ops act on it
; In:  a2 = script: word offset of the handle field in a6
; Out: NEXT with a5 = that block (Op2_ResetA5 restores $C(a6))
; Evidence: movea.w (a6,d0.w),a5; 27 corpus runs; $18120 `3BC2 000E 3E58 3BF6` (run the extra block's animation, then back)
; ---------------------------------------------------------------------------
Op2_LoadA5Handle:
	move.w	(a2)+,d0
	movea.w	(a6,d0.w),a5
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_ResetSprBlock - a2 op: reinitialises the extra sprite block in an a6 field: attribute = g_SceneSprAttr and its first $2A bytes (frames, count, animation state) cleared
; In:  a2 = script: word offset of the handle field in a6
; Out: NEXT (a5 unchanged)
; Evidence: same clear as Op2_AllocSprBlock on (a6,d0.w); 1 corpus run; $1C56A `3BCC 000E`
; ---------------------------------------------------------------------------
Op2_ResetSprBlock:
	move.w	(a2)+,d0
	movea.w	(a6,d0.w),a0
	move.w	(g_SceneSprAttr).l,$38(a0)
	moveq	#$0,d0
	move.l	d0,(a0)+
	move.l	d0,(a0)+
	move.l	d0,(a0)+
	move.l	d0,(a0)+
	move.l	d0,(a0)+
	move.l	d0,(a0)+
	move.l	d0,(a0)+
	move.l	d0,(a0)+
	move.l	d0,(a0)+
	move.l	d0,(a0)+
	move.w	d0,(a0)+
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_ResetA5 - a2 op: makes the object's own sprite block ($C(a6)) the current a5 again
; In:  a2 = script (no operand)
; Out: NEXT with a5 = $C(a6)
; Evidence: movea.w $C(a6),a5 (the handle VM2_RunObject loads); 21 corpus runs; follows Op2_LoadA5Handle/Op2_AllocSprBlock in $180C2, $18126
; ---------------------------------------------------------------------------
Op2_ResetA5:
	movea.w	$C(a6),a5
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_ReleaseFrames - a2 op: releases every sprite frame (VRAM art) the current sprite block a5 holds and zeroes its frame count
; In:  a2 = script (no operand); a5 = sprite block
; Out: NEXT
; Evidence: bsr Obj_ReleaseFrames; 2 corpus runs; $291C2 `3BFE 0EA6`, $3D0EE `3BFE 3C06 0010`
; ---------------------------------------------------------------------------
Op2_ReleaseFrames:
	bsr.w	Obj_ReleaseFrames
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_ReleaseBlockFrames - a2 op: releases the sprite frames of the extra sprite block in an a6 field (a5 is kept)
; In:  a2 = script: word offset of the handle field in a6
; Out: NEXT
; Evidence: a5 saved, movea.w (a6,d0.w),a5 / bsr Obj_ReleaseFrames / a5 restored; 6 corpus runs; $19A3E `3C06 000E`
; ---------------------------------------------------------------------------
Op2_ReleaseBlockFrames:
	move.l	a5,-(a7)
	move.w	(a2)+,d0
	movea.w	(a6,d0.w),a5
	bsr.w	Obj_ReleaseFrames
	movea.l	(a7)+,a5
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_SetBlockAnim - a2 op: starts an animation on the extra sprite block in an a6 field (as Op2_SetAnim does for a5): animation script $2E = relative target, timer $1E cleared, done flag cleared
; In:  a2 = script: word offset to the animation script, word offset of the handle field in a6
; Out: NEXT
; Evidence: move.l d0,$2E(a0) / clr.w $1E(a0) / bclr #0,$18(a0) on (a6,d1.w); 5 corpus runs, 132 script uses (e.g. $180C8 `3C18 1C6A 000E`)
; ---------------------------------------------------------------------------
Op2_SetBlockAnim:
	movea.l	a2,a0
	adda.w	(a2)+,a0
	move.l	a0,d0
	move.w	(a2)+,d1
	movea.w	(a6,d1.w),a0
	move.l	d0,$2E(a0)
	clr.w	$1E(a0)
	bclr	#$0,$18(a0)
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_JitterPos - a2 op: moves the object by a random amount: x += Rand_Next * wx and y += Rand_Next * wy in 16.16 (up to half the word in pixels either way)
; In:  a2 = script: word wx, word wy
; Out: $3E/$42(a6) changed; NEXT
; Evidence: bsr Rand_Next / muls.w (a2)+,d0 / add.l d0,$3E(a6), same for $42; 6 corpus runs; $3C610 `3C36 012C 0000`
; ---------------------------------------------------------------------------
Op2_JitterPos:
	bsr.w	Rand_Next
	muls.w	(a2)+,d0
	add.l	d0,$3E(a6)
	bsr.w	Rand_Next
	muls.w	(a2)+,d0
	add.l	d0,$42(a6)
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_PushPos - a2 op: pushes the object's x and y (16.16 longs) on the 68000 stack for Op2_PopPos later in the same run
; In:  a2 = script (no operand)
; Out: two longs on sp; NEXT
; Evidence: move.l $3E(a6),-(a7) / move.l $42(a6),-(a7); $1D930 `3C4E 3C36 0004 0004 2D10 EC90 3C5A` (save, jitter, gosub, restore); 3 corpus runs
; ---------------------------------------------------------------------------
Op2_PushPos:
	move.l	$3E(a6),-(a7)
	move.l	$42(a6),-(a7)
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_PopPos - a2 op: restores the object's x and y pushed by Op2_PushPos
; In:  a2 = script (no operand); sp = the two longs
; Out: $3E/$42(a6) restored; NEXT
; Evidence: move.l (a7)+,$42(a6) / move.l (a7)+,$3E(a6); 3 corpus runs; $48620 `3C5A 0820`
; ---------------------------------------------------------------------------
Op2_PopPos:
	move.l	(a7)+,$42(a6)
	move.l	(a7)+,$3E(a6)
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_AimDirAtPlayer - a2 op: points the sprite's aim direction at the targeted player: $1A(a5) = 4 x the direction (of n) from the object to the player record's position (+16 px down), via Mot_DirToPlayer
; In:  a2 = script: word n directions; a4 +$10 = targeted player record (0 = none: the operand is skipped)
; Out: $1A(a5) = direction * 4; NEXT
; Evidence: beq Op2_SkipWord when $10(a4) = 0; jsr $14F78 (octant slope maths, result 0..n-1) / add d2,d2 x2 / move.w d2,$1A(a5); read by Op2_SwitchAimDir; 14 corpus runs; $1C0DA `3C66 0008`
; ---------------------------------------------------------------------------
Op2_AimDirAtPlayer:
	move.w	$10(a4),d0
	beq.w	Op2_SkipWord
	movea.w	d0,a0
	move.w	$3E(a6),d2
	move.w	$42(a6),d3
	move.w	(a2)+,d4
	jsr	(Mot_DirToPlayer).l
	add.w	d2,d2
	add.w	d2,d2
	move.w	d2,$1A(a5)
	movea.w	(a2)+,a0
	jmp	(a0)
