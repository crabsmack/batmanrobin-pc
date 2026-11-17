; ---------------------------------------------------------------------------
; Boss_Hatter_RoadCmdGoto - road schedule command 2 (also the tail of gosub 3 and loop 6): moves the schedule pointer by a 12-bit offset (sign-extended high nibble of byte 0, then byte 1) from the command's start and reads the next command
; In:  a0 = past the command, d4 = byte 0, d1 = byte 1
; Out: a0 = new command address; continues at the command reader
; Evidence: ext.w d4 / asl.w #4 / move.b d1,d4 / lea -2(a0,d4.w); reached from Boss_Hatter_RoadCmdTbl_Goto; F2E8 in the schedule at $03F5BE jumps back $18 bytes to its start; ctarget fuzz-113400-s2
; ---------------------------------------------------------------------------
Boss_Hatter_RoadCmdGoto:
	ext.w	d4
	asl.w	#$4,d4
	move.b	d1,d4
	lea.l	-$2(a0,d4.w),a0
	bra.s	Boss_Hatter_RoadCmdNext
