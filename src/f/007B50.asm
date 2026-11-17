; ---------------------------------------------------------------------------
; Spr_AddFront - appends a sprite frame to the unsorted front list, which Spr_BuildSAT emits before the sorted list (in front of all sorted sprites)
; In:  a0 = frame-table entry, d4 = x, d5 = y, d6 = attribute word
; Out: a 12-byte entry at $9DDE-12; $9DDE advances by $12; nothing when g_SprNodesFree runs out
; Evidence: Spr_BuildSAT links the last front entry (-$1E(a0)) to the sorted list head and starts its walk at $9E04 when $9DDE != $9E16; callers $46CC, $6776, $1412E, $37708, $41F0E
; ---------------------------------------------------------------------------
Spr_AddFront:
	subq.b	#$1,(g_SprNodesFree).w
	bmi.s	loc_007B6C
	move.l	a0,d0
	movea.w	(g_SprFrontEnd).w,a0
	move.w	a0,d1
	move.l	d0,-(a0)
	movem.w	d1/d4-d6,-(a0)
	addi.w	#$12,(g_SprFrontEnd).w
	rts
loc_007B6C:
	clr.b	(g_SprNodesFree).w
	rts
