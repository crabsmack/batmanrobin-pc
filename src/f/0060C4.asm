; ---------------------------------------------------------------------------
; VM4_Fork - spawns a child a4 object with script a3: pool block on the object list (handler VM4_RunObject), owner $10 and attr $22 inherited, anim/frames/box/depth cleared, radius/heading/x/y = d2-d5, velocities and accelerations 0
; In:  a3 = child script, a6 = parent, d2-d5 = radius/heading/x/y (16.16)
; Out: a5 = child, a6 = parent; Z set on success, Z clear (rts at once) when the pool is full
; Evidence: lea $53B4 / bsr $8A08 (pool alloc + link), exg a5,a6, stores $8 = a3 and the 12 motion longs; called by Op4_Fork/ForkAbs/ForkOrBranch/ForkAbsAndEnd/ForkRestAndEnd
; ---------------------------------------------------------------------------
VM4_Fork:
	movea.l	a6,a5
	lea.l	($53B4).w,a0
	bsr.w	Obj_New
	exg.l	a5,a6
	bne.s	loc_006114
	move.l	a3,$8(a5)
	move.w	$22(a6),$22(a5)
	move.w	$10(a6),$10(a5)
	moveq	#$0,d0
	move.w	d0,$1C(a5)
	move.w	d0,$1E(a5)
	move.w	d0,$20(a5)
	move.w	d0,$14(a5)
	move.w	d0,$24(a5)
	lea.l	$26(a5),a0
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
loc_006114:
	rts
; ---------------------------------------------------------------------------
; Op4_ChildAddOwnVel - a4 op: adds this object's velocities $36/$3A/$3E/$42 (radius, heading, x, y) to the last spawned child's, so the child carries the spawner's motion
; In:  a4 = script (no operands); a5 = child, a6 = this object
; Out: NEXT
; Evidence: four add.l $xx(a6),$xx(a5); script uses $2056C, $28720 (after Op4_ForkAbs: `60AC ... 6116`), $28F50; 20 corpus runs
; ---------------------------------------------------------------------------
Op4_ChildAddOwnVel:
	move.l	$36(a6),d0
	add.l	d0,$36(a5)
	move.l	$3A(a6),d0
	add.l	d0,$3A(a5)
	move.l	$3E(a6),d0
	add.l	d0,$3E(a5)
	move.l	$42(a6),d0
	add.l	d0,$42(a5)
	movea.w	(a4)+,a0
	jmp	(a0)
