; ---------------------------------------------------------------------------
; Ending_EpilogueTextStep - native fragment of the epilogue narrator's loop: while a typewriter text is running, continues it (TextMap_Run), then uploads the text map to plane A and re-enters the script
; In:  a2 = script after the Op2_Native op
; Out: back to the a2 script (jsr $7FE)
; Evidence: tst.b g_TextMapTyping / TextMap_Run / TextMap_Upload; runs every frame f19910-21790 (1868 frames) in scenario-level-skip
; ---------------------------------------------------------------------------
Ending_EpilogueTextStep:
	tst.b	(g_TextMapTyping).w
	beq.s	loc_04A5F4
	jsr	(TextMap_Run).l
loc_04A5F4:
	jsr	(TextMap_Upload).l
	jsr	($7FE).w
