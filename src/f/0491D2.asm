; ---------------------------------------------------------------------------
; Continue_HasCredits - credits-left exit of the continue screen's credit test (Continue_BranchHasCredits $491BC): resumes Scr2_0491A0 at $491D6, which starts the slow countdown (coroutine 0, $492CE: one monitor digit every 60 frames) and the Start check (coroutine 1, $491F0), then bobs and scrolls the picture every frame
; In:  a2 script context (Op2_Native at $491BA)
; Out: jsr $7FE.w
; Evidence: bne target of the tst.w g_Credits at $491BC; $491C6 (no credits) instead starts the fast countdown $49258 ($140 per digit) and the NO CREDITS REMAINING text $49234, never read in the corpus; scenario-continue / scenario-game-over f41999
; ---------------------------------------------------------------------------
Continue_HasCredits:
	jsr	($7FE).w
