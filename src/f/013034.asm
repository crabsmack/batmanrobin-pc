; ---------------------------------------------------------------------------
; Drop_RandomHealthB_case4 - jump-table case 4 of Drop_RandomHealthB: one B health pickup (bra Drop_HealthB1)
; In:  as Drop_RandomHealthB
; Out: as Drop_HealthB1
; Evidence: the `jmp $13030(pc,d0.w)` table of Drop_RandomHealthB, d0 = 4; executed in the TAS
; ---------------------------------------------------------------------------
Drop_RandomHealthB_case4:
	bra.s	Drop_HealthB1
