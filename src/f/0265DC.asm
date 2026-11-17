; ---------------------------------------------------------------------------
; Enemy_LadderThug_InitLadder - shared tail of the ladder spawners: clears the new object's ladder block (+$10(a6): swing angle, angular speed) and starts the swing at angle 6.0
; In:  a6 = new object
; Out: d0 = 0
; Evidence: jmp from Enemy_LadderThug_SpawnA/B/C and Boss_Stage2_SpawnEndLadder; ObjPool_Clear; Enemy_LadderThug_SwingLadder reads that block; TAS f24981-f34742 (8 spawns)
; ---------------------------------------------------------------------------
Enemy_LadderThug_InitLadder:
	movea.w	$10(a6),a0
	jsr	(ObjPool_Clear).l
	dc.w	$217C,$0006,$0000,$0000	; insn: move.l #$60000, $0(a0)
	moveq	#$0,d0
	rts
