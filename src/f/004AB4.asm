; ---------------------------------------------------------------------------
; Op2_DrawSprAtDrawnBack - draw-list op: draws one frame at the drawn position $4E/$50(a6) behind every sorted sprite (Spr_AddBack)
; In:  a2 = draw list: word frame; a1 = frame slot
; Out: a1 + 2; NEXT
; Evidence: $4E/$50(a6), bsr Spr_AddBack; 1 run; $3D544 `4AB4 4754 4AB4 475E 44F2`
; ---------------------------------------------------------------------------
Op2_DrawSprAtDrawnBack:
	move.w	$38(a5),d6
	move.w	$4E(a6),d4
	move.w	$50(a6),d5
	move.w	(a2)+,d1
	move.w	(a1),d2
	bsr.w	Spr_ChangeFrame
	move.w	d1,(a1)+
	bsr.w	Spr_AddBack
	movea.w	(a2)+,a0
	jmp	(a0)
