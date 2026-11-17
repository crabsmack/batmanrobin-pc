; ---------------------------------------------------------------------------
; Op4_AddY - a4 op: adds a signed pixel word to the integer part of the object's y $32 (16.16)
; In:  a4 = script: word dy
; Out: NEXT
; Evidence: add.w (a4)+ to $32(a6) (the high word of the long); script uses $13252 (-$14 in Drop child A), $20E6C, $46952; 26 corpus runs
; ---------------------------------------------------------------------------
Op4_AddY:
	move.w	(a4)+,d0
	add.w	d0,$32(a6)
	movea.w	(a4)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op4_AddRadius - a4 op: adds a signed word to the integer part of the object's radius $26
; In:  a4 = script: word dr
; Out: NEXT
; Evidence: add.w (a4)+ to $26(a6); radius scales the (cos, sin)(heading) offset of the drawn point; 21 corpus runs
; ---------------------------------------------------------------------------
Op4_AddRadius:
	move.w	(a4)+,d0
	add.w	d0,$26(a6)
	movea.w	(a4)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op4_AddHeading - a4 op: adds a signed word to the integer part of the object's heading $2A ($400 per turn)
; In:  a4 = script: word dh
; Out: NEXT
; Evidence: add.w (a4)+ to $2A(a6); script uses $26AAC, $29816, $36022; 27 corpus runs
; ---------------------------------------------------------------------------
Op4_AddHeading:
	move.w	(a4)+,d0
	add.w	d0,$2A(a6)
	movea.w	(a4)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op4_AddVelX - a4 op: adds a signed 8.8 word (px/frame) to the object's x velocity $3E (16.16)
; In:  a4 = script: word dvx (8.8)
; Out: NEXT
; Evidence: ext.l / asl.l #8 / add.l to $3E(a6); 26 corpus runs
; ---------------------------------------------------------------------------
Op4_AddVelX:
	move.w	(a4)+,d0
	ext.l	d0
	asl.l	#$8,d0
	add.l	d0,$3E(a6)
	movea.w	(a4)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op4_AddVelYAlt - a4 op: second copy of the y-velocity add: a signed 8.8 word added to $42(a6)
; In:  a4 = script: word dvy (8.8)
; Out: NEXT
; Evidence: byte-for-byte the code of Op4_AddVelY ($578C), in the second add family ($583C-$58AD); script uses $26D4E (`5868 0200`), $26D7C; 3 corpus runs (fuzz only)
; ---------------------------------------------------------------------------
Op4_AddVelYAlt:
	move.w	(a4)+,d0
	ext.l	d0
	asl.l	#$8,d0
	add.l	d0,$42(a6)
	movea.w	(a4)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op4_AddVelRadiusAlt - a4 op: second copy of the radius-velocity add: a signed 8.8 word added to $36(a6)
; In:  a4 = script: word dvr (8.8)
; Out: NEXT
; Evidence: same code as Op4_AddVelRadius ($579A); script uses $29812, $298DE, $298F8; 38 corpus runs
; ---------------------------------------------------------------------------
Op4_AddVelRadiusAlt:
	move.w	(a4)+,d0
	ext.l	d0
	asl.l	#$8,d0
	add.l	d0,$36(a6)
	movea.w	(a4)+,a0
	jmp	(a0)
