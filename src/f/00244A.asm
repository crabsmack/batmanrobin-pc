; ---------------------------------------------------------------------------
; Op2_SpawnChild - a2 op: spawns a child object once: when the a6 field named by the operand is 0, calls the spawner with d4/d5 = own x/y and stores the new object's a6 there; a field already set, or a failed spawn, just continues
; In:  a2 = script: word a6 field offset, long spawner (e.g. $399C6, an inline-argument spawn through $37134; out a6 = new object, Z set = ok, as Obj_SpawnEnemy)
; Out: field = child a6 (word); NEXT
; Evidence: tst.w (a6,d0.w) / jsr (a0) / movea.l a6,a0 / move.w a0,(a6,d0.w); dispatched in 26 runs; $39686 `244A 002A 000399C6` then the child ops 2BF6/2AA6 on the same field $2A
; ---------------------------------------------------------------------------
Op2_SpawnChild:
	move.w	$3E(a6),d4
	move.w	$42(a6),d5
	move.w	(a2)+,d0
	movea.l	(a2)+,a0
	tst.w	(a6,d0.w)
	bne.w	VM2_Next
	movem.l	d0/a2-a6,-(a7)
	jsr	(a0)
	movea.l	a6,a0
	movem.l	(a7)+,d0/a2-a6
	bne.w	VM2_Next
	move.w	a0,(a6,d0.w)
	movea.w	(a2)+,a0
	jmp	(a0)
