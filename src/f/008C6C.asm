; ---------------------------------------------------------------------------
; Hitbox_Unlink - unlinks collision box a0 from whichever box list it is on (prev.next = next, next.prev = prev)
; In:  a0 = box node (+0 next, +2 prev)
; Out: a0 kept; a1 preserved (parked in usp)
; Evidence: callers free boxes after it: $3858 (box list rebuild), $5406 / $6156 (a4 object end) then ObjPool_Free; $4F4E, $53D4, $372C0
; ---------------------------------------------------------------------------
Hitbox_Unlink:
	move.l	a1,usp
	movea.w	$2(a0),a1
	move.w	(a0),(a1)
	movea.w	(a0),a1
	move.w	$2(a0),$2(a1)
	move.l	usp,a1
	rts
