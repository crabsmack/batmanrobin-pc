; ---------------------------------------------------------------------------
; Op5_SetOrigin - a5 op: sets the wave's spawn origin offset $26/$28 (x, y words)
; In:  a5 = script: word x, word y
; Out: NEXT
; Evidence: move.w (a5)+,$26 / $28; script uses $23CFC (`6DF4 0154 0094`), $23F02; 9 corpus runs
; ---------------------------------------------------------------------------
Op5_SetOrigin:
	move.w	(a5)+,$26(a6)
; ---------------------------------------------------------------------------
; Op5_SetOriginY - a5 op: sets the spawn origin y offset $28
; In:  a5 = script: word y
; Out: NEXT
; Evidence: dispatched entry inside Op5_SetOrigin (computed-jump target in 4 corpus runs); script uses $228AE (`6DF8 0114`), $22BAA
; ---------------------------------------------------------------------------
Op5_SetOriginY:
	move.w	(a5)+,$28(a6)
	movea.w	(a5)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op5_SetOriginX - a5 op: sets the spawn origin x offset $26
; In:  a5 = script: word x
; Out: NEXT
; Evidence: move.w (a5)+,$26; script uses $22108 (`6E00 0154`), $222B6; 9 corpus runs
; ---------------------------------------------------------------------------
Op5_SetOriginX:
	move.w	(a5)+,$26(a6)
	movea.w	(a5)+,a0
	jmp	(a0)
