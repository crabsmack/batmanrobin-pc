; ---------------------------------------------------------------------------
; Continue_BranchHasCredits - native fragment of the continue controller: with credits left resumes the script at Scr2_0491D6 (the countdown with Start), with none at $0491C6 (a script tail no run reaches)
; In:  g_Credits; entered by Op2_Native ($07FC) at $0491BA
; Out: jsr $7FE into the chosen script part
; Evidence: tst.w g_Credits / bne to the second jsr $7FE.w; $0491C6-$0491D1 is never read in the corpus (the scenarios always had credits); scenario-continue/game-over f41999
; ---------------------------------------------------------------------------
Continue_BranchHasCredits:
	tst.w	(g_Credits).w
	bne.s	Continue_HasCredits
	jsr	($7FE).w
