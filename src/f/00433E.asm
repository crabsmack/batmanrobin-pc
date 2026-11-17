; ---------------------------------------------------------------------------
; Op2_PlaySound - a2 op: plays sound effect d0 = operand through the driver request Snd_PlaySfx (8-byte entries of Tbl_1E50D6 with a priority check)
; In:  a2 = script: word sound id (0 = none)
; Out: NEXT
; Evidence: move.w (a2)+,d0 / jsr Snd_PlaySfx (subq #1 / bmi rts; lsl #3 into Tbl_1E50D6; cmp.b priority); 133 corpus runs; $13436 `433E 0041`
; ---------------------------------------------------------------------------
Op2_PlaySound:
	move.w	(a2)+,d0
	jsr	(Snd_PlaySfx).l
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_PlayMusic - a2 op: switches the music to a song through Sound_PlayMusic, preserving the VM registers
; In:  a2 = script: word song
; Out: NEXT
; Evidence: movem.l a1-a6 around jsr Sound_PlayMusic; 3 corpus runs; $18080 `434A 0001`
; ---------------------------------------------------------------------------
Op2_PlayMusic:
	move.w	(a2)+,d0
	movem.l	a1-a6,-(a7)
	jsr	(Sound_PlayMusic).l
	movem.l	(a7)+,a1-a6
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_BranchPopulationNz - a2 op: branches while any weighted script object is alive (g_ObjPopulation != 0)
; In:  a2 = script: word offset
; Out: branch when the population is not 0, else NEXT after the offset
; Evidence: tst.w g_ObjPopulation / bne Op2_Goto; 5 corpus runs; $1E96A `435E FFCE`
; ---------------------------------------------------------------------------
Op2_BranchPopulationNz:
	tst.w	(g_ObjPopulation).w
	bne.w	Op2_Goto
	addq.l	#$2,a2
	movea.w	(a2)+,a0
	jmp	(a0)
