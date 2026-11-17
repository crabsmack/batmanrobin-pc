; ---------------------------------------------------------------------------
; Obj_NewMulti_HandleRun - unrolled run of 4-byte units (move.w a2,(a1)+ / movea.w (a2),a2) that writes the next pool block's address into the object's next handle word and walks the free list; this label is the 8-block entry
; In:  a1 = next handle word (object +8), a2 = next free block, d1 = 2 * blocks
; Out: falls to jmp $896A(pc,d1.w) at $8968 into Obj_NewMulti_ClearRun
; Evidence: entered by jmp $896C(pc,d7.w) at $8944 (Obj_NewMulti, d7 = -4 * blocks): entry = $896C - 4n, one unit per extra block; ctarget $895C in 140 runs (4 blocks), $8958 29, $8954 10, $8950 4, $894C 1 (TAS f10511); the unit before it at $8948 (9 blocks) is raw, never run; dkit.toml computed_entry_runs $894C-$896C stride 4
; ---------------------------------------------------------------------------
Obj_NewMulti_HandleRun:
	move.w	a2,(a1)+
	movea.w	(a2),a2
	move.w	a2,(a1)+
	movea.w	(a2),a2
	move.w	a2,(a1)+
	movea.w	(a2),a2
	move.w	a2,(a1)+
	movea.w	(a2),a2
	move.w	a2,(a1)+
	movea.w	(a2),a2
	move.w	a2,(a1)+
	movea.w	(a2),a2
	move.w	a2,(a1)+
	movea.w	(a2),a2
	jmp	Obj_NewMulti_ClearRun-2(pc,d1.w)
