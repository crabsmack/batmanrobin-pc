; ---------------------------------------------------------------------------
; Hitbox_FindPlayerBoxOfId - finds the first box on g_PlayerBoxList whose id (+$10) is d3 and that overlaps d4-d6 x d5-d7
; In:  d3 = box id, d4 left, d5 top, d6 right, d7 bottom
; Out: Z clear, d0 = -1, a2 = the box; Z set when none
; Evidence: only caller $3434 (d3 = $3E of the object linked at $10(a4), rectangle = script offsets flipped by $38(a5) around the object); 5.7k TAS frames
; ---------------------------------------------------------------------------
Hitbox_FindPlayerBoxOfId:
	movea.w	(g_PlayerBoxList).w,a2
	tst.w	(a2)
	bne.s	Hitbox_TestBoxOfId
	rts
