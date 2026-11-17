; ---------------------------------------------------------------------------
; Op4_AddVelY - a4 op: adds a signed 8.8 word (px/frame) to the object's y velocity $42 (16.16)
; In:  a4 = script: word dvy (8.8)
; Out: NEXT
; Evidence: ext.l / asl.l #8 / add.l to $42(a6); also the tail of the unrun x+y twin at $5782; same code as Op4_AddVelYAlt ($5868); script uses $1A602, $1F13A; 5 corpus runs
; ---------------------------------------------------------------------------
Op4_AddVelY:
	move.w	(a4)+,d0
	ext.l	d0
	asl.l	#$8,d0
	add.l	d0,$42(a6)
	movea.w	(a4)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op4_AddVelRadius - a4 op: adds a signed 8.8 word to the object's radius velocity $36 (16.16)
; In:  a4 = script: word dvr (8.8)
; Out: NEXT
; Evidence: ext.l / asl.l #8 / add.l to $36(a6); same code as Op4_AddVelRadiusAlt ($5876); script use $1F26C; 3 corpus runs
; ---------------------------------------------------------------------------
Op4_AddVelRadius:
	move.w	(a4)+,d0
	ext.l	d0
	asl.l	#$8,d0
	add.l	d0,$36(a6)
	movea.w	(a4)+,a0
	jmp	(a0)
