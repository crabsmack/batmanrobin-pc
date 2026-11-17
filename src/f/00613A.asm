; ---------------------------------------------------------------------------
; VM4_SpawnChild - spawn core of Op4_Spawn*: allocates a child a4 object and sets its script and motion from d2-d5 plus an optional 48-byte parameter table; on a full pool drops the caller's return and ends the run or takes the failure branch
; In:  a4 = op operands: word failure offset (0 = end the run), word child-script offset, word table offset (0 = none); d2-d5 = radius/heading/x/y; a6 = parent
; Out: a5 = child (a4 past the 3 words) and rts to the op; on failure no return to the op
; Evidence: $8A08 alloc with handler $53B4; the table holds (value, velocity, acceleration) longs for x, y, radius and heading at +0/+$C/+$18/+$24, the values added to d4/d5/d2/d3; callers Op4_Spawn, Op4_SpawnAddVel, Op4_SpawnRunChild; VM4_ReinitSelf is the Op4_ReplaceSelf entry
; ---------------------------------------------------------------------------
VM4_SpawnChild:
	movea.l	a6,a5
	lea.l	($53B4).w,a0
	bsr.w	Obj_New
	exg.l	a5,a6
	beq.s	VM4_SpawnChild_Init
	addq.l	#$4,a7
	move.w	(a4),d0
	beq.s	loc_006154
	adda.w	d0,a4
	movea.w	(a4)+,a0
	jmp	(a0)
loc_006154:
	rts
; ---------------------------------------------------------------------------
; VM4_ReinitSelf - Op4_ReplaceSelf's entry into VM4_SpawnChild: frees the object's collision box $14(a6) if any and re-initialises the object itself (a5 = a6) from the spawn operands
; In:  a4 = Op4_ReplaceSelf's spawn operands; a6 = object
; Out: a5 = a6 re-initialised; rts
; Evidence: bra.w from Op4_ReplaceSelf ($557E); Hitbox_Unlink + ObjPool_Free on the box, then the common $6182 tail
; ---------------------------------------------------------------------------
VM4_ReinitSelf:
	move.w	$14(a6),d0
	beq.s	VM4_SpawnChild_InitMotion
	movea.w	d0,a0
	bsr.w	Hitbox_Unlink
	bsr.w	ObjPool_Free
	movea.l	a6,a5
	bra.s	VM4_SpawnChild_InitMotion
; ---------------------------------------------------------------------------
; VM4_SpawnChild_Init - VM4_SpawnChild after a successful allocation: clears the new child's animation and frame words, copies the parent's attributes and owner id, then VM4_SpawnChild_InitMotion
; In:  a5 = the new child, a6 = the parent, a4 = the spawn operands, d2-d5
; Out: falls into VM4_SpawnChild_InitMotion
; Evidence: beq.s here from VM4_SpawnChild after Obj_New; clr.w $1C/$1E/$20(a5) / move.w $22(a6),$22(a5) / move.w $10(a6),$10(a5)
; ---------------------------------------------------------------------------
VM4_SpawnChild_Init:
	clr.w	$1C(a5)
	clr.w	$1E(a5)
	clr.w	$20(a5)
	move.w	$22(a6),$22(a5)
	move.w	$10(a6),$10(a5)
; ---------------------------------------------------------------------------
; VM4_SpawnChild_InitMotion - the shared tail of VM4_SpawnChild and VM4_ReinitSelf: clears the box and depth, sets the script from the child-script offset and the motion from d2-d5 plus the optional parameter table
; In:  a5 = the object to set up, a4 = the spawn operands (failure, child-script, table offsets), d2-d5
; Out: a4 past the operands, a3 = the script or the table, a0 past the 12 motion longs, d0/d1, the last move.l's flags; rts
; Evidence: moveq #0,d0 / move.w d0,$14(a5) / move.w d0,$24(a5) / move.l (a4)+,d1 / lea -2(a4,d1.w),a3 / move.l a3,$8(a5); bra.s from VM4_ReinitSelf, fall-through from VM4_SpawnChild_Init
; ---------------------------------------------------------------------------
VM4_SpawnChild_InitMotion:
	moveq	#$0,d0
	move.w	d0,$14(a5)
	move.w	d0,$24(a5)
	lea.l	$26(a5),a0
	move.l	(a4)+,d1
	lea.l	-$2(a4,d1.w),a3
	move.l	a3,$8(a5)
	move.w	(a4)+,d1
	bne.s	loc_0061B8
	move.l	d2,(a0)+
	move.l	d3,(a0)+
	move.l	d4,(a0)+
	move.l	d5,(a0)+
	move.l	d0,(a0)+
	move.l	d0,(a0)+
	move.l	d0,(a0)+
	move.l	d0,(a0)+
	move.l	d0,(a0)+
	move.l	d0,(a0)+
	move.l	d0,(a0)+
	move.l	d0,(a0)+
	rts
loc_0061B8:
	lea.l	-$2(a4,d1.w),a3
	move.l	$18(a3),d0
	add.l	d2,d0
	move.l	d0,(a0)+
	move.l	$24(a3),d0
	add.l	d3,d0
	move.l	d0,(a0)+
	move.l	(a3),d0
	add.l	d4,d0
	move.l	d0,(a0)+
	move.l	$C(a3),d0
	add.l	d5,d0
	move.l	d0,(a0)+
	move.l	$1C(a3),(a0)+
	move.l	$28(a3),(a0)+
	move.l	$4(a3),(a0)+
	move.l	$10(a3),(a0)+
	move.l	$20(a3),(a0)+
	move.l	$2C(a3),(a0)+
	move.l	$8(a3),(a0)+
	move.l	$14(a3),(a0)+
	rts
; ---------------------------------------------------------------------------
; Op4_SetXYFromDraw - a4 op: sets the integer words of the object's x $2E / y $32 to the drawn x/y words d4/d5 (keeps a terrain-corrected position)
; In:  a4 = script (no operands); d4/d5 low words = position
; Out: NEXT
; Evidence: move.w d4,$2E(a6) / move.w d5,$32(a6); script use $26D68 (after Op4_AltTerrainMove `6576 0002 61FC`); 3 corpus runs
; ---------------------------------------------------------------------------
Op4_SetXYFromDraw:
	move.w	d4,$2E(a6)
	move.w	d5,$32(a6)
	movea.w	(a4)+,a0
	jmp	(a0)
