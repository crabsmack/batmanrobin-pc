; ---------------------------------------------------------------------------
; Drop_HealthA9 - spawn action 40: drops nine A-style health pickups (a4 script Scr4_0130B0)
; In:  d4/d5 = position
; Out: as Fx_RunScriptNoParent
; Evidence: Tbl_012EF0 entry 40, between Drop_HealthA5 (39) and Drop_HealthARow (41); lea Scr4_0130B0,a4 / bra.w Drop_RunFxScript; never run in the corpus (converted 2026-11-17 after a playtest stopped on $13060)
; ---------------------------------------------------------------------------
Drop_HealthA9:
	lea.l	(Scr4_0130B0).l,a4
	bra.w	Drop_RunFxScript
