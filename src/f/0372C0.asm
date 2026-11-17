; ---------------------------------------------------------------------------
; ToyChain_RemoveLastLink - removes the newest link of a chain: sets var 0 bit 4 (link lost, read by ToyChain_SteerAtPlayer), stores the link's position as the anchor $46/$4A(a6), releases its frame, unlinks its box and frees the block
; In:  a6 = chain object, a4 +$24 = link count
; Out: d0 = 0 removed, 1 = no link left; anchor = the removed link's x/y (Spawn_AtAnchor then puts a single figure there)
; Evidence: ori #$10,$22(a4); $44/$46 of the link -> $46/$4A(a6); Spr_ReleaseFrame, Hitbox_Unlink, ObjPool_Free; Op2_CallNativeRelSwitch in the shedding routines $03887C-$038912 (followed by Op2_CallNative Spawn_AtAnchor with a figure spawner such as ToyFigure_Spawn) and in the hit path $0379F8; TAS 504 frames
; ---------------------------------------------------------------------------
ToyChain_RemoveLastLink:
	move.w	$24(a4),d0
	subq.w	#$1,d0
	bmi.s	loc_03730A
	move.w	d0,$24(a4)
	ori.w	#$10,$22(a4)
	add.w	d0,d0
	movea.w	$E(a6),a1
	movea.w	$34(a1,d0.w),a1
	move.w	$44(a1),$46(a6)
	move.w	$46(a1),$4A(a6)
	move.w	$42(a1),d2
	jsr	(Spr_ReleaseFrame).l
	movea.l	a1,a0
	tst.w	$2(a0)
	beq.s	loc_037300
	jsr	(Hitbox_Unlink).l
loc_037300:
	jsr	(ObjPool_Free).l
	moveq	#$0,d0
	rts
loc_03730A:
	moveq	#$1,d0
	rts
