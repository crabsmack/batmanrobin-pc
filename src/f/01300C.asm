; ---------------------------------------------------------------------------
; Drop_RandomHealthA_case6 - jump-table case 6 of Drop_RandomHealthA: bra.s Drop_HealthA3
; In:  as Drop_RandomHealthA
; Out: as Drop_HealthA3
; Evidence: the jmp $13006(pc,d0.w) table of Drop_RandomHealthA, d0 = 6; never run in the corpus (converted 2026-11-17 after a playtest stopped on $13060)
; ---------------------------------------------------------------------------
Drop_RandomHealthA_case6:
	bra.s	Drop_HealthA3
