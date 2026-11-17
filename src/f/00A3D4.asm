; ---------------------------------------------------------------------------
; Pad_ReadLive - the V-int's busy-path pad read: updates g_PadLive1/g_PadLive2 directly from ports 1 and 2, so the pause code can poll the pads while the main loop is held
; In:  none (called from VInt_BusyPath with the Z80 bus held)
; Out: g_PadLive1, g_PadLive2 (g_Pad1/g_Pad2 untouched); a4/a5/d0/d1 trashed
; Evidence: a5 = g_PadLive1, port $A10003 through Pad_Update, then $A10005 falling through into Pad_Update; only from VInt_BusyPath; 86 TAS frames (frames where the main loop overran)
; ---------------------------------------------------------------------------
Pad_ReadLive:
	lea.l	(g_PadLive1).w,a5
	lea.l	($A10003).l,a4
	bsr.w	Pad_Update
	lea.l	($A10005).l,a4
