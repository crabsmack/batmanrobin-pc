; ---------------------------------------------------------------------------
; Ending_CellTextStep - native fragment of the Freeze-cell narrator's loop: continues a running typewriter text (TextMap_Run), uploads the text map and re-enters the script
; In:  a2 = script after the Op2_Native op
; Out: back to the a2 script (jsr $7FE)
; Evidence: same code as Ending_EpilogueTextStep; tst.b g_TextMapTyping; every frame f15109-19900 (4760 frames) in scenario-level-skip
; ---------------------------------------------------------------------------
Ending_CellTextStep:
	tst.b	(g_TextMapTyping).w
	beq.s	loc_04A740
	jsr	(TextMap_Run).l
loc_04A740:
	jsr	(TextMap_Upload).l
	jsr	($7FE).w
