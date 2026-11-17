; ---------------------------------------------------------------------------
; Op4_ChildAddHeading - a4 op: adds a signed word to the integer part of the child's heading $2A(a5) ($400 per turn)
; In:  a4 = script: word dh; a5 = child
; Out: NEXT
; Evidence: add.w (a4)+ to $2A(a5); 84 a4-script uses, e.g. $37034-$37050 (one spawn, then turn it); 118 corpus runs
; ---------------------------------------------------------------------------
Op4_ChildAddHeading:
	move.w	(a4)+,d0
	add.w	d0,$2A(a5)
	movea.w	(a4)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op4_ChildAddVelX - a4 op: adds a signed 8.8 word (px/frame) to the child's x velocity $3E(a5)
; In:  a4 = script: word dvx (8.8); a5 = child
; Out: NEXT
; Evidence: ext.l / asl.l #8 / add.l to $3E(a5); script uses $26C66, $26C7A; 23 corpus runs
; ---------------------------------------------------------------------------
Op4_ChildAddVelX:
	move.w	(a4)+,d0
	ext.l	d0
	asl.l	#$8,d0
	add.l	d0,$3E(a5)
	movea.w	(a4)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op4_ChildAddVelY - a4 op: adds a signed 8.8 word to the child's y velocity $42(a5)
; In:  a4 = script: word dvy (8.8); a5 = child
; Out: NEXT
; Evidence: ext.l / asl.l #8 / add.l to $42(a5); script uses $1A4B0, $1E228 (after Op4_Spawn); 3 corpus runs
; ---------------------------------------------------------------------------
Op4_ChildAddVelY:
	move.w	(a4)+,d0
	ext.l	d0
	asl.l	#$8,d0
	add.l	d0,$42(a5)
	movea.w	(a4)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op4_ChildAddVelRadius - a4 op: adds a signed 8.8 word to the child's radius velocity $36(a5), so it flies out along its heading
; In:  a4 = script: word dvr (8.8); a5 = child
; Out: NEXT
; Evidence: ext.l / asl.l #8 / add.l to $36(a5); script uses $25B76, $25B94, $25BB2; 30 corpus runs
; ---------------------------------------------------------------------------
Op4_ChildAddVelRadius:
	move.w	(a4)+,d0
	ext.l	d0
	asl.l	#$8,d0
	add.l	d0,$36(a5)
	movea.w	(a4)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op4_ChildAddVelHeading - a4 op: adds a signed 8.8 word to the child's heading speed $3A(a5) (spin around the centre)
; In:  a4 = script: word dvh (8.8); a5 = child
; Out: NEXT
; Evidence: ext.l / asl.l #8 / add.l to $3A(a5); 40 a4-script uses, e.g. $5036E-$50390 in the weapon shot scripts; 117 corpus runs
; ---------------------------------------------------------------------------
Op4_ChildAddVelHeading:
	move.w	(a4)+,d0
	ext.l	d0
	asl.l	#$8,d0
	add.l	d0,$3A(a5)
	movea.w	(a4)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op4_ChildAddAccelX - a4 op: adds a signed 8.8 word to the child's x acceleration $4E(a5)
; In:  a4 = script: word dax (8.8); a5 = child
; Out: NEXT
; Evidence: ext.l / asl.l #8 / add.l to $4E(a5); 20 corpus runs
; ---------------------------------------------------------------------------
Op4_ChildAddAccelX:
	move.w	(a4)+,d0
	ext.l	d0
	asl.l	#$8,d0
	add.l	d0,$4E(a5)
	movea.w	(a4)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op4_ChildAddAccelY - a4 op: adds a signed 8.8 word to the child's y acceleration $52(a5)
; In:  a4 = script: word day (8.8); a5 = child
; Out: NEXT
; Evidence: ext.l / asl.l #8 / add.l to $52(a5); script uses $2057E, $20592, $3B74E; 15 corpus runs
; ---------------------------------------------------------------------------
Op4_ChildAddAccelY:
	move.w	(a4)+,d0
	ext.l	d0
	asl.l	#$8,d0
	add.l	d0,$52(a5)
	movea.w	(a4)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op4_ChildAddAccelRadius - a4 op: adds a signed 8.8 word to the child's radius acceleration $46(a5)
; In:  a4 = script: word dar (8.8); a5 = child
; Out: NEXT
; Evidence: ext.l / asl.l #8 / add.l to $46(a5); script use $1E488; 6 corpus runs
; ---------------------------------------------------------------------------
Op4_ChildAddAccelRadius:
	move.w	(a4)+,d0
	ext.l	d0
	asl.l	#$8,d0
	add.l	d0,$46(a5)
	movea.w	(a4)+,a0
	jmp	(a0)
