; ---------------------------------------------------------------------------
; Drop_HealthARow - spawn action 41: releases five A-style health pickups one after another (delay $40 between, a different spawn argument each)
; In:  d4/d5 = position
; Out: as Fx_RunScriptNoParent
; Evidence: Tbl_012EF0 entry 41; Scr4_013144 = $576A waits and five $5606 forks into the A pickup past its launch setup ($13268, args $34/$28/$1C/$10/$04); 21 TAS runs (7132-126242)
; ---------------------------------------------------------------------------
Drop_HealthARow:
	lea.l	(Scr4_013144).l,a4
	bra.w	Drop_RunFxScript
; ---------------------------------------------------------------------------
; Drop_HealthB1 - spawn action 42: drops one B-style health pickup (Scr4_01313A)
; In:  d4/d5 = position
; Out: as Fx_RunScriptNoParent
; Evidence: Tbl_012EF0 entry 42; Scr4_01313A forks one child at $13220 (different launch and animation ops from the A pickup), which also ends in sound $12 + Drop_CollectHealth; target of Drop_RandomHealthB
; ---------------------------------------------------------------------------
Drop_HealthB1:
	lea.l	(Scr4_01313A).l,a4
	bra.w	Drop_RunFxScript
; ---------------------------------------------------------------------------
; Drop_HealthB2 - spawn action 43: drops two B-style health pickups (Scr4_013132)
; In:  d4/d5 = position
; Out: as Fx_RunScriptNoParent
; Evidence: Tbl_012EF0 entry 43; two forks of the $13220 pickup; executed in the TAS (11 corpus runs)
; ---------------------------------------------------------------------------
Drop_HealthB2:
	lea.l	(Scr4_013132).l,a4
	bra.w	Drop_RunFxScript
; ---------------------------------------------------------------------------
; Drop_HealthB3 - spawn action 44: drops three B-style health pickups (Scr4_01312A)
; In:  d4/d5 = position
; Out: as Fx_RunScriptNoParent
; Evidence: Tbl_012EF0 entry 44; three forks of the $13220 pickup; executed in the TAS (12 corpus runs)
; ---------------------------------------------------------------------------
Drop_HealthB3:
	lea.l	(Scr4_01312A).l,a4
	bra.w	Drop_RunFxScript
; ---------------------------------------------------------------------------
; Drop_HealthB5 - spawn action 45: drops five B-style health pickups (Scr4_01311A)
; In:  d4/d5 = position
; Out: as Fx_RunScriptNoParent
; Evidence: Tbl_012EF0 entry 45; five forks of the $13220 pickup; executed in the TAS (13 corpus runs)
; ---------------------------------------------------------------------------
Drop_HealthB5:
	lea.l	(Scr4_01311A).l,a4
	bra.w	Drop_RunFxScript
; ---------------------------------------------------------------------------
; Drop_HealthB9 - spawn action 46: drops nine B-style health pickups (Scr4_0130FA)
; In:  d4/d5 = position
; Out: as Fx_RunScriptNoParent
; Evidence: Tbl_012EF0 entry 46; nine forks of the $13220 pickup; executed in the TAS and one fuzz run
; ---------------------------------------------------------------------------
Drop_HealthB9:
	lea.l	(Scr4_0130FA).l,a4
	bra.w	Drop_RunFxScript
; ---------------------------------------------------------------------------
; Drop_HealthBRow - spawn action 47: after a $380 wait releases five B-style health pickups one after another
; In:  d4/d5 = position
; Out: as Fx_RunScriptNoParent
; Evidence: Tbl_012EF0 entry 47; Scr4_0131B2 = $576A waits and five forks into the B pickup past its launch setup ($13232); executed once, in the TAS
; ---------------------------------------------------------------------------
Drop_HealthBRow:
	lea.l	(Scr4_0131B2).l,a4
	bra.w	Drop_RunFxScript
