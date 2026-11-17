; ---------------------------------------------------------------------------
; Obj_NewMulti - creates a d7-block object: pops d7 pool blocks, makes the first the object (handler a0, linked at the end of the main list), stores the other d7-1 block addresses as handles from +8 and zeroes the rest of the 8 handle words up to +$17
; In:  a0 = handler, d7 = blocks (1-9; 4 for a script object: a3/a4/a5 handles at +8/+$A/+$C)
; Out: d0 = 0 and a6 = the object, a2 = new g_ObjPoolHead; or d0 = -1 through Obj_NewMultiFail when the pool is short
; Evidence: callers Obj_Spawn $8F0E (every Obj_SpawnEnemy*/Boss* spawner) and Stage1_SpawnSignDebris (d7 = 7); computes jmp $896C(pc,-4*d7) into Obj_NewMulti_HandleRun and jmp $896A(pc,2*d7) into Obj_NewMulti_ClearRun; TAS 1907 frames
; ---------------------------------------------------------------------------
Obj_NewMulti:
	sub.b	d7,(g_ObjPoolFree).w
	bcs.s	Obj_NewMultiFail
	movea.w	(g_ObjPoolHead).w,a6
	movea.w	(a6),a2
	move.l	a0,(a6)
	lea.l	(g_ObjListTail).w,a1
	movea.w	$4(a1),a0
	move.w	a6,$4(a1)
	move.w	a6,$6(a0)
	movem.w	a0-a1,$4(a6)
	lea.l	$8(a6),a1
	moveq	#$0,d0
; ---------------------------------------------------------------------------
; Obj_NewMulti_FillHandles - the shared tail of Obj_NewMulti / Obj_NewMultiUnlinked: from the block count computes the jumps into Obj_NewMulti_HandleRun (one handle per extra block) and Obj_NewMulti_ClearRun (zeroes the rest of the 8 handle words)
; In:  d7 = blocks, d0 = 0, a1 = object +8, a2 = the next free block, a6 = the object
; Out: d1 = 2 x blocks, d7 = -4 x blocks; the handles, g_ObjPoolHead = the first block not taken, d0 = 0
; Evidence: add.w d7,d7 / move.w d7,d1 / add.w d7,d7 / neg.w d7 / jmp $896C(pc,d7.w); bra.s from Obj_NewMultiUnlinked ($8998)
; ---------------------------------------------------------------------------
Obj_NewMulti_FillHandles:
	add.w	d7,d7
	move.w	d7,d1
	add.w	d7,d7
	neg.w	d7
	jmp	Obj_NewMulti_ClearRun(pc,d7.w)
