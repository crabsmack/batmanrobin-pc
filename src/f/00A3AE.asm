; ---------------------------------------------------------------------------
; Pad_ReadAll - the V-int's full-path pad read: updates g_Pad1/g_Pad2 from ports 1 and 2 and copies both records to g_PadLive1/g_PadLive2
; In:  none (called from VInt_Handler with the Z80 bus held)
; Out: g_Pad1, g_Pad2, g_PadLive1, g_PadLive2; a4/a5/d0/d1 trashed
; Evidence: ports $A10003/$A10005 with a5 = g_Pad1 (Pad_Update advances a5 to g_Pad2); bsr from VInt_Handler between the Z80 bus request and release; 130,055 TAS frames (every full V-int)
; ---------------------------------------------------------------------------
Pad_ReadAll:
	lea.l	(g_Pad1).w,a5
	lea.l	($A10003).l,a4
	bsr.w	Pad_Update
	lea.l	($A10005).l,a4
	bsr.w	Pad_Update
	move.l	(g_Pad1).w,(g_PadLive1).w
	move.l	(g_Pad2).w,(g_PadLive2).w
	rts
