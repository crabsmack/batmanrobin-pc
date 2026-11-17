; ---------------------------------------------------------------------------
; Op5_AddOriginX - a5 op: adds a signed word to the spawn origin x offset $26 (spawns marching along a row)
; In:  a5 = script: word dx
; Out: NEXT
; Evidence: add.w d0,$26; script uses $3C4B6 (`6E18 001E 6CB0 FFF2`: +30 px per loop pass), $3C4D6 (`FFE2`); 3 corpus runs
; ---------------------------------------------------------------------------
Op5_AddOriginX:
	move.w	(a5)+,d0
	add.w	d0,$26(a6)
	movea.w	(a5)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op5_OriginAtScreenCentre - a5 op: sets the spawn origin offset $26/$28 to the current screen centre in world coordinates (g_CameraX + $A0, g_CameraY + $70)
; In:  a5 = script (no operands)
; Out: NEXT
; Evidence: g_CameraX + $A0 -> $26, g_CameraY + $70 -> $28; script uses $22210, $228AC; 7 corpus runs
; ---------------------------------------------------------------------------
Op5_OriginAtScreenCentre:
	move.w	(g_CameraX).w,d0
	addi.w	#$A0,d0
	move.w	d0,$26(a6)
	move.w	(g_CameraY).w,d0
	addi.w	#$70,d0
	move.w	d0,$28(a6)
	movea.w	(a5)+,a0
	jmp	(a0)
