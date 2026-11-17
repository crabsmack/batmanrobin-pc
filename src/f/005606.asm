; ---------------------------------------------------------------------------
; Op4_Spawn - a4 op: spawns a child a4 object ($613A: pool block on the object list, handler VM4_RunObject, owner $10 and attr $22 inherited, radius/heading/x/y from d2-d5 plus an optional parameter table) and continues with a5 = the child
; In:  a4 = script: word failure offset (0 = end the run when the pool is full), word child-script offset, word parameter-table offset (0 = none); d2-d5 = radius, heading, x, y (16.16)
; Out: NEXT with a5 = child; pool full: rts or branch by the failure offset
; Evidence: bsr $613A then NEXT; the most used a4 op (229 a4-script uses, e.g. the shot scripts $108CE-$10964: `5606 0000 0006 0000 53F4`); 142 corpus runs
; ---------------------------------------------------------------------------
Op4_Spawn:
	bsr.w	VM4_SpawnChild
	movea.w	(a4)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op4_SpawnAddVel - a4 op: as Op4_Spawn, then adds this object's velocities $36/$3A/$3E/$42 (radius, heading, x, y) to the child's, so the child inherits the spawner's (or g_SpawnCtx's parent) motion
; In:  a4 = script: word failure offset, word child-script offset, word parameter-table offset
; Out: NEXT with a5 = child
; Evidence: bsr $613A then four add.l $36-$42(a6) to (a5); on g_SpawnCtx those longs are the parent's velocity (Fx_RunScript); script uses $1A45E, $1A552, $1ABDC; 43 corpus runs
; ---------------------------------------------------------------------------
Op4_SpawnAddVel:
	bsr.w	VM4_SpawnChild
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
; ---------------------------------------------------------------------------
; Op4_AddMotionX - a4 op: adds three longs to the object's x motion: x $2E, x velocity $3E and x acceleration $4E (all 16.16)
; In:  a4 = script: long dx, long dvx, long dax
; Out: NEXT
; Evidence: three add.l (a4)+ to $2E/$3E/$4E(a6); the motion op $6286 integrates accel -> velocity -> position; script uses $46BD0, $46BF2; 2 corpus runs
; ---------------------------------------------------------------------------
Op4_AddMotionX:
	move.l	(a4)+,d0
	add.l	d0,$2E(a6)
	move.l	(a4)+,d0
	add.l	d0,$3E(a6)
	move.l	(a4)+,d0
	add.l	d0,$4E(a6)
	movea.w	(a4)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op4_AddMotionY - a4 op: adds three longs to the object's y motion: y $32, y velocity $42 and y acceleration $52 (16.16)
; In:  a4 = script: long dy, long dvy, long day
; Out: NEXT
; Evidence: three add.l (a4)+ to $32/$42/$52(a6); script use $46D00; 4 corpus runs
; ---------------------------------------------------------------------------
Op4_AddMotionY:
	move.l	(a4)+,d0
	add.l	d0,$32(a6)
	move.l	(a4)+,d0
	add.l	d0,$42(a6)
	move.l	(a4)+,d0
	add.l	d0,$52(a6)
	movea.w	(a4)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op4_AddMotionRadius - a4 op: adds three longs to the object's radius motion: radius $26, its velocity $36 and acceleration $46 (16.16; the drawn point is the radius along the heading)
; In:  a4 = script: long dr, long dvr, long dar
; Out: NEXT
; Evidence: three add.l (a4)+ to $26/$36/$46(a6); script uses $14C14, $1AB5E, $26BA6; 151 corpus runs
; ---------------------------------------------------------------------------
Op4_AddMotionRadius:
	move.l	(a4)+,d0
	add.l	d0,$26(a6)
	move.l	(a4)+,d0
	add.l	d0,$36(a6)
	move.l	(a4)+,d0
	add.l	d0,$46(a6)
	movea.w	(a4)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op4_AddMotionHeading - a4 op: adds three longs to the object's heading motion: heading $2A (0-$3FF per turn in the high word), its turn speed $3A and turn acceleration $4A
; In:  a4 = script: long dh, long dvh, long dah
; Out: NEXT
; Evidence: three add.l (a4)+ to $2A/$3A/$4A(a6); heading indexes Tbl_059B22 (andi #$3FE) in the motion ops; script use $14C22; 1 corpus run (the TAS)
; ---------------------------------------------------------------------------
Op4_AddMotionHeading:
	move.l	(a4)+,d0
	add.l	d0,$2A(a6)
	move.l	(a4)+,d0
	add.l	d0,$3A(a6)
	move.l	(a4)+,d0
	add.l	d0,$4A(a6)
	movea.w	(a4)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op4_ChildAddMotionX - a4 op: adds three longs to the last spawned child's (a5) x, x velocity and x acceleration ($2E/$3E/$4E)
; In:  a4 = script: long dx, long dvx, long dax; a5 = child
; Out: NEXT
; Evidence: three add.l (a4)+ to $2E/$3E/$4E(a5); a5 = the object made by the preceding spawn op; 17 corpus runs
; ---------------------------------------------------------------------------
Op4_ChildAddMotionX:
	move.l	(a4)+,d0
	add.l	d0,$2E(a5)
	move.l	(a4)+,d0
	add.l	d0,$3E(a5)
	move.l	(a4)+,d0
	add.l	d0,$4E(a5)
	movea.w	(a4)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op4_ChildAddMotionY - a4 op: adds three longs to the child's (a5) y, y velocity and y acceleration ($32/$42/$52)
; In:  a4 = script: long dy, long dvy, long day; a5 = child
; Out: NEXT
; Evidence: three add.l (a4)+ to $32/$42/$52(a5); script uses $1AA68, $1F288, $25AC8 (after a spawn op); 51 corpus runs
; ---------------------------------------------------------------------------
Op4_ChildAddMotionY:
	move.l	(a4)+,d0
	add.l	d0,$32(a5)
	move.l	(a4)+,d0
	add.l	d0,$42(a5)
	move.l	(a4)+,d0
	add.l	d0,$52(a5)
	movea.w	(a4)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op4_ChildAddMotionRadius - a4 op: adds three longs to the child's (a5) radius, radius velocity and radius acceleration ($26/$36/$46)
; In:  a4 = script: long dr, long dvr, long dar; a5 = child
; Out: NEXT
; Evidence: three add.l (a4)+ to $26/$36/$46(a5); script uses $1AB34 (after Op4_SpawnAddVel), $1E560; 49 corpus runs
; ---------------------------------------------------------------------------
Op4_ChildAddMotionRadius:
	move.l	(a4)+,d0
	add.l	d0,$26(a5)
	move.l	(a4)+,d0
	add.l	d0,$36(a5)
	move.l	(a4)+,d0
	add.l	d0,$46(a5)
	movea.w	(a4)+,a0
	jmp	(a0)
