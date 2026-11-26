; ---------------------------------------------------------------------------
; Drop_HealthA1 - spawn action 36: drops one A-style health pickup (a4 script Scr4_0130F0)
; In:  d4/d5 = position
; Out: as Fx_RunScriptNoParent
; Evidence: Tbl_012EF0 entry 36; Scr4_0130F0 forks one child at $13252, whose script plays sound $12 and calls Drop_CollectHealth (+$10 health) when collected (collection by touch inferred); 39 TAS runs (7834-31499)
; ---------------------------------------------------------------------------
Drop_HealthA1:
	lea.l	(Scr4_0130F0).l,a4
	bra.w	Drop_RunFxScript
; ---------------------------------------------------------------------------
; Drop_HealthA2 - spawn action 37: drops two A-style health pickups (Scr4_0130E8)
; In:  d4/d5 = position
; Out: as Fx_RunScriptNoParent
; Evidence: Tbl_012EF0 entry 37; Scr4_0130E8 = two $5606 forks of the $13252 pickup; executed in the TAS and scenario-ending
; ---------------------------------------------------------------------------
Drop_HealthA2:
	lea.l	(Scr4_0130E8).l,a4
	bra.w	Drop_RunFxScript
; ---------------------------------------------------------------------------
; Drop_HealthA3 - spawn action 38: drops three A-style health pickups (Scr4_0130E0)
; In:  d4/d5 = position
; Out: as Fx_RunScriptNoParent
; Evidence: Tbl_012EF0 entry 38; three forks of the $13252 pickup; executed in the TAS
; ---------------------------------------------------------------------------
Drop_HealthA3:
	lea.l	(Scr4_0130E0).l,a4
	bra.w	Drop_RunFxScript
; ---------------------------------------------------------------------------
; Drop_HealthA5 - spawn action 39: drops five A-style health pickups (Scr4_0130D0)
; In:  d4/d5 = position
; Out: as Fx_RunScriptNoParent
; Evidence: Tbl_012EF0 entry 39; five forks of the $13252 pickup; executed in the TAS. Entry 40 is nine (Drop_HealthA9, Scr4_0130B0)
; ---------------------------------------------------------------------------
Drop_HealthA5:
	lea.l	(Scr4_0130D0).l,a4
	bra.w	Drop_RunFxScript
