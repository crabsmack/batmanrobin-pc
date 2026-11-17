; ---------------------------------------------------------------------------
; Op4_BounceYRandom - a4 op: bounces the object: y velocity $42 = -(v/2 + a random [0, 1/4) of v/2's integer part), a reversed, roughly halved, slightly randomised rebound
; In:  a4 = script (no operands)
; Out: NEXT
; Evidence: Rand_Next & $3FFF mulu the high word of $42/2, plus $42/2, negated; script uses $1A9BA (`63E2 53D4` after Op4_TerrainMoveDefault), $358B8; 6 corpus runs
; ---------------------------------------------------------------------------
Op4_BounceYRandom:
	bsr.w	Rand_Next
	andi.w	#$3FFF,d0
	move.l	$42(a6),d1
	asr.l	#$1,d1
	swap	d1
	mulu.w	d1,d0
	swap	d1
	add.l	d1,d0
	neg.l	d0
	move.l	d0,$42(a6)
	movea.w	(a4)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op4_SetAnimRateRandom - a4 op: sets the object's animation ticker: accumulator $1C and rate $1D both = base + a random number below range (randomised animation speed)
; In:  a4 = script: byte range, byte base
; Out: NEXT
; Evidence: move.b (a4)+ / Rand_Below / add.b (a4)+ / stored to $1C and $1D; script use $306CE (`6402 4040`); 13 corpus runs
; ---------------------------------------------------------------------------
Op4_SetAnimRateRandom:
	moveq	#$0,d0
	move.b	(a4)+,d0
	bsr.w	Rand_Below
	add.b	(a4)+,d0
	move.b	d0,$1C(a6)
	move.b	d0,$1D(a6)
	movea.w	(a4)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op4_SetAnim - a4 op: starts an animation: ticker word $1C (accumulator byte, rate byte), anim-script pointer $C; sets the resume pc $8 to the next op so later frames restart there
; In:  a4 = script: word ticker ($FF40 = step now, then every 4th frame), long anim-script address
; Out: NEXT
; Evidence: move.w (a4)+,$1C / move.l (a4)+,$C / move.l a4,$8; 161 script uses, e.g. $108D8 (`6418 FF40 0001 08E4` then `634E 67EE`, the anim script $108E4 = Op4_AnimFrameAttr / Op4_AnimFrame ... Op4_Kill); 151 corpus runs
; ---------------------------------------------------------------------------
Op4_SetAnim:
	move.w	(a4)+,$1C(a6)
	move.l	(a4)+,$C(a6)
	move.l	a4,$8(a6)
	movea.w	(a4)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op4_SetFrame - a4 op: shows one fixed sprite frame: stops the animation ticker ($1C = 0), sets the wanted frame $1E and the resume pc $8 to the next op
; In:  a4 = script: word frame (byte offset into the frame table)
; Out: NEXT
; Evidence: clr.w $1C / move.w (a4)+,$1E / move.l a4,$8; script uses $1A5A6 (`6428 3232`), $1B5F4; 43 corpus runs
; ---------------------------------------------------------------------------
Op4_SetFrame:
	clr.w	$1C(a6)
	move.w	(a4)+,$1E(a6)
	move.l	a4,$8(a6)
	movea.w	(a4)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op4_ChildSetAnim - a4 op: starts an animation on the last spawned child: its ticker word $1C(a5) and anim-script pointer $C(a5)
; In:  a4 = script: word ticker, long anim-script address; a5 = child
; Out: NEXT
; Evidence: move.w (a4)+,$1C(a5) / move.l (a4)+,$C(a5); script uses $1E270 (`6438 FF40 0001 E3FE`); 47 corpus runs
; ---------------------------------------------------------------------------
Op4_ChildSetAnim:
	move.w	(a4)+,$1C(a5)
	move.l	(a4)+,$C(a5)
	movea.w	(a4)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op4_ChildSetFrame - a4 op: gives the last spawned child one fixed frame: clears its ticker $1C(a5) and sets its wanted frame $1E(a5)
; In:  a4 = script: word frame; a5 = child
; Out: NEXT
; Evidence: clr.w $1C(a5) / move.w (a4)+,$1E(a5); script uses $1ED76 (`6444 FD62` then Op4_Spawn); 14 corpus runs
; ---------------------------------------------------------------------------
Op4_ChildSetFrame:
	clr.w	$1C(a5)
	move.w	(a4)+,$1E(a5)
	movea.w	(a4)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op4_SetDepth - a4 op: sets the object's display depth $24 (0 = in front of all sprites, 1 = behind all, else depth-sorted with that key)
; In:  a4 = script: word depth
; Out: NEXT
; Evidence: move.w (a4)+,$24(a6); the draw tail picks Spr_AddFront / Spr_AddBack / Spr_AddSorted by it; script uses $1465A (`6450 0001`), $1466A (`6450 0000`); 42 corpus runs
; ---------------------------------------------------------------------------
Op4_SetDepth:
	move.w	(a4)+,$24(a6)
	movea.w	(a4)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op4_ChildSetDepth - a4 op: sets the last spawned child's display depth $24(a5) (0 front, 1 back, else sorted)
; In:  a4 = script: word depth; a5 = child
; Out: NEXT
; Evidence: move.w (a4)+,$24(a5); script uses $4718C, $471AA (`6458 0002`); 2 corpus runs
; ---------------------------------------------------------------------------
Op4_ChildSetDepth:
	move.w	(a4)+,$24(a5)
	movea.w	(a4)+,a0
	jmp	(a0)
