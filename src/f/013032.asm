; ---------------------------------------------------------------------------
; Drop_RandomHealthB_case2 - jump-table case 2 of Drop_RandomHealthB: bra.s Drop_HealthB2
; In:  as Drop_RandomHealthB
; Out: as Drop_HealthB2
; Evidence: the jmp $13030(pc,d0.w) table of Drop_RandomHealthB, d0 = 2 (the TAS's 26 runs took cases 0 and 4); never run in the corpus (converted 2026-11-17 after a playtest stopped on $13060)
; ---------------------------------------------------------------------------
Drop_RandomHealthB_case2:
	bra.s	Drop_HealthB2
