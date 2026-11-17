; ---------------------------------------------------------------------------
; Stage1_SpawnPoliceCar - parade-truck trigger callback: spawns the police car that drives along behind the truck: starts $50 px left of the screen at y camera + $6C, depth 1, script Scr2_01E5C2 (cruises to screen-x targets)
; In:  -
; Out: d0 = 0, or Z clear when the pool is short
; Evidence: Obj_SpawnScripted, $2A(a4) = $1E5C2; Scr2_01E5C2: SetCruiseX $42A, anim $42, coroutine of Op2_SeekScreenXRangeOrBranch $D2-$DC / $8C-$96 / $17C-$190; trigger entry Tbl_0166EA +$EE, TAS f13561; shot f13860: a black-and-white 1940s police cruiser with a red roof light at the object's position
; ---------------------------------------------------------------------------
Stage1_SpawnPoliceCar:
	jsr	(Obj_SpawnScripted).l
	bne.s	loc_01E5C0
	move.l	#$1E5C2,$2A(a4)
	move.w	(g_CameraX).l,d0
	subi.w	#$50,d0
	move.w	d0,$3E(a6)
	move.w	(g_CameraY).l,d0
	addi.w	#$6C,d0
	move.w	d0,$42(a6)
	move.w	#$1,$3A(a5)
	moveq	#$0,d0
loc_01E5C0:
	rts
