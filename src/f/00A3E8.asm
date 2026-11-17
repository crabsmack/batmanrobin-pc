; ---------------------------------------------------------------------------
; Pad_Update - reads one pad and updates its 4-byte record: held, previous, newly pressed, newly released
; In:  a4 = pad data port, a5 = record (g_Pad1 layout)
; Out: record +0 held (Pad_ReadPort), +1 the previous held, +2 held and not before, +3 before and not now; a5 advanced by 4 (to the next pad's record); d0/d1 trashed
; Evidence: byte arithmetic: d1 = old, (a5)+ = new, (a5) = old, pressed = ~old & new, released = ~new & old; callers Pad_ReadAll (twice) and Pad_ReadLive (once, then falls into it for pad 2); every TAS frame
; ---------------------------------------------------------------------------
Pad_Update:
	bsr.w	Pad_ReadPort
	move.b	(a5),d1
	move.b	d0,(a5)+
	move.b	d1,(a5)
	not.b	d1
	and.b	d0,d1
	not.b	d0
	and.b	(a5)+,d0
	move.b	d1,(a5)+
	move.b	d0,(a5)+
	rts
