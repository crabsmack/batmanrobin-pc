; ---------------------------------------------------------------------------
; Hitbox_FindPlayerBox - finds the first box on g_PlayerBoxList overlapping the rectangle d4-d6 x d5-d7
; In:  d4 left, d5 top, d6 right, d7 bottom (world pixels)
; Out: Z clear, d0 = -1, a2 = the box when one overlaps; Z set when none (a2 = sentinel)
; Evidence: box rect +$12 left, +$14 top, +$16 right, +$18 bottom (written by $4EF6 and $66E2); callers $375A, $4E32, $65B4, $41F0E (that one adds $10 to box+$20 damage on a hit); 76k TAS frames
; ---------------------------------------------------------------------------
Hitbox_FindPlayerBox:
	movea.w	(g_PlayerBoxList).w,a2
	tst.w	(a2)
	bne.s	Hitbox_TestBox
	rts
