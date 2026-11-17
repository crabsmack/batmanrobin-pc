; ---------------------------------------------------------------------------
; Spr_DrawDirect - draws one sprite frame straight into the SAT buffer, bypassing the display list, using the adjustable clip window
; In:  a0 = frame-table entry, d4 = x, d5 = y, d6 = attribute word
; Out: SAT entries appended; g_SATPtr and g_SprCountLo updated
; Evidence: callers $11A1C and $15940 (HUD digits: frame = BCD nibble * 10 + 10, d6 = $8000 high priority) call Spr_ChangeFrame first; ran on 2303 TAS frames
; ---------------------------------------------------------------------------
Spr_DrawDirect:
	movea.w	(g_SATPtr).w,a1
	move.w	(g_SprCount).w,d2
	moveq	#$0,d3
	bsr.w	Spr_EmitClipped
	move.b	d2,(g_SprCountLo).w
	move.w	a1,(g_SATPtr).w
	rts
