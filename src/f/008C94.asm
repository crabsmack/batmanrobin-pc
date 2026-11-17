; ---------------------------------------------------------------------------
; Hitbox_FindNextBox - continues an overlap scan from the box after a2 (same rectangle test as Hitbox_FindPlayerBox)
; In:  a2 = last box found, d4-d7 = rectangle
; Out: Z clear, d0 = -1, a2 = next overlapping box; Z set at the end of the list
; Evidence: loops `jsr (a3) / bsr Hitbox_FindNextBox / bne` at $660A and $4E64 visit every overlapping box; $3750C; 84k TAS frames
; ---------------------------------------------------------------------------
Hitbox_FindNextBox:
	movea.w	(a2),a2
	tst.w	(a2)
	beq.s	Hitbox_ScanDone
; ---------------------------------------------------------------------------
; Hitbox_TestBox - the overlap test of the box scans: box a2 against the rectangle d4-d7; a miss goes on with the next box (Hitbox_FindNextBox)
; In:  a2 = box (not the tail sentinel), d4 left, d5 top, d6 right, d7 bottom
; Out: Z clear, d0 = -1, a2 = the first overlapping box from a2 on; Z set at the end of the list
; Evidence: bne.s from Hitbox_FindPlayerBox / Hitbox_FindEnemyBox (the first box); the recompiler makes it a function of its own (P4 entries)
; ---------------------------------------------------------------------------
Hitbox_TestBox:
	cmp.w	$16(a2),d4
	bgt.s	Hitbox_FindNextBox
	cmp.w	$12(a2),d6
	blt.s	Hitbox_FindNextBox
	cmp.w	$18(a2),d5
	bgt.s	Hitbox_FindNextBox
	cmp.w	$14(a2),d7
	blt.s	Hitbox_FindNextBox
	moveq	#-$1,d0
; ---------------------------------------------------------------------------
; Hitbox_ScanDone - the rts the box scans end on (a hit, after moveq #-1,d0, or the end of the list)
; In:  none
; Out: none (the condition codes of the last test or the moveq)
; Evidence: beq.s at the end of the list in Hitbox_FindNextBox; a function of the generated code (P4 entries)
; ---------------------------------------------------------------------------
Hitbox_ScanDone:
	rts
