; ---------------------------------------------------------------------------
; Drop_RandomHealthA_case2 - jump-table case 2 of Drop_RandomHealthA: bra.s Drop_HealthA2
; In:  as Drop_RandomHealthA
; Out: as Drop_HealthA2
; Evidence: the jmp $13006(pc,d0.w) table of Drop_RandomHealthA, d0 = 2; never run in the corpus (converted 2026-11-17 after a playtest stopped on $13060)
; ---------------------------------------------------------------------------
Drop_RandomHealthA_case2:
	bra.s	Drop_HealthA2
