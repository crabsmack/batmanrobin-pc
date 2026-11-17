; ---------------------------------------------------------------------------
; Op2_StopCoroutine0 - a2 op: stops coroutine slot 0 by pointing $38(a4) back at Scr2_002D28 (a script that just returns), the slot's idle value from Obj_InitSpawned
; In:  a2 = script (no operand); a4 = object's a4 block
; Out: NEXT
; Evidence: move.l #$2D28,$38(a4); the word at $2D28 is the return op $2D2A; the start ops follow at $2334; 51 corpus runs incl. the TAS
; ---------------------------------------------------------------------------
Op2_StopCoroutine0:
	move.l	#$2D28,$38(a4)
	movea.w	(a2)+,a0
	jmp	(a0)
