; ---------------------------------------------------------------------------
; Obj_NewMulti_ClearRun - unrolled move.w d0,(a1)+ run that zeroes the handle words Obj_NewMulti_HandleRun left over (9 - blocks of them, so +8..+$17 are always written)
; In:  a1 = first unused handle word, d0 = 0
; Out: continues into Obj_NewMulti_Clear4 and the common tail (g_ObjPoolHead = a2, d0 = 0)
; Evidence: entered by jmp $896A(pc,d1.w) at $8968 with d1 = 2 * blocks: 1 block enters here ($896C, 8 words), 4 blocks at $8972; ctarget entries $8972-$897A in 1-140 runs
; ---------------------------------------------------------------------------
Obj_NewMulti_ClearRun:
	move.w	d0,(a1)+
	move.w	d0,(a1)+
	move.w	d0,(a1)+
