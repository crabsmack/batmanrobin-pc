; ---------------------------------------------------------------------------
; Boss_Hatter_LittleHatFollow - a4 native (op $53C4) in the little hat's frame loop: keeps it at its owner's x and deletes it (Op4_Kill) once it has fallen 24 px below the owner's y, else continues to the draw op
; In:  a6 = the little hat (+$12 owner), d5 = its drawn y (Op4_MoveXY)
; Out: d4 = owner x
; Evidence: native after Op4_MoveXY in Scr4_03D908; jsr $53C6 into Op4_Draw at $03D932; TAS f113672-f113706 (35 frames)
; ---------------------------------------------------------------------------
Boss_Hatter_LittleHatFollow:
	movea.w	$12(a6),a0
	move.w	$3E(a0),d4
	move.w	$42(a0),d0
	addi.w	#$18,d0
	cmp.w	d0,d5
	blt.s	loc_03D92E
	jmp	(Op4_Kill).l
loc_03D92E:
	jsr	($53C6).w
