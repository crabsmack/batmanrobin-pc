; ---------------------------------------------------------------------------
; Title_MenuInit - opens the title menu: blanks the text map and puts the cursor on START
; In:  -
; Out: g_MenuCursor = 0
; Evidence: Op2_CallNativeRel from $4972E (Scr2_049720); TextMap_Clear; TAS f404, scenario-options f404/f1449
; ---------------------------------------------------------------------------
Title_MenuInit:
	jsr	(TextMap_Clear).l
	clr.w	(g_MenuCursor).w
	rts
; ---------------------------------------------------------------------------
; Title_MenuStep - title menu per-frame native: Up/Down move the cursor over START / OPTIONS, the menu text map Tbl_0498AE is drawn and sent to plane A, and any held button restarts the 500-frame idle timeout
; In:  -
; Out: g_MenuCursor; g_SceneTimer = $1F4 when a pad 1 button is held
; Evidence: Menu_MoveCursor d1 = 1 (last item); Tbl_0498AE has menu items 0/1 'START' 'OPTIONS'; Op2_CallNativeRel from $49738, the coroutine that branches on Start; runtime entry (ctarget) in the TAS and scenario-options
; ---------------------------------------------------------------------------
Title_MenuStep:
	moveq	#$1,d1
	jsr	(Menu_MoveCursor).l
	lea.l	Tbl_0498AE(pc),a0
	jsr	(TextMap_Run).l
	jsr	(TextMap_Upload).l
	tst.b	(g_Pad1).w
	beq.s	loc_0499E6
	move.w	#$1F4,(g_SceneTimer).w
loc_0499E6:
	rts
