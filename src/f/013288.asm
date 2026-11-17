; ---------------------------------------------------------------------------
; Drop_RunFxScript - shared tail of the Drop_ actions: runs a4 script a4 as a parentless effect at d4/d5 with radius and heading 0
; In:  a4 = a4-VM script, d4/d5 = x/y pixels
; Out: as Fx_RunScriptNoParent (d4/d5 swapped to 16.16)
; Evidence: branched to by the 12 Drop_HealthA*/B* entries and the unrun $13060; d2/d3 = 0 are the radius/heading registers of Fx_RunScript (NOTES a4 effect spawns)
; ---------------------------------------------------------------------------
Drop_RunFxScript:
	moveq	#$0,d2
	moveq	#$0,d3
	swap	d4
	swap	d5
	jmp	(Fx_RunScriptNoParent).l
; ---------------------------------------------------------------------------
; Drop_CollectHealth - a4 native of the health pickups: gives the player who touched it 16 health ($10, capped at $100 = full)
; In:  a2 = the touching collision box (inferred from the touch op before it)
; Out: player record +4 health raised; the native ends early (Item_BoxPlayer pops) when the box is not a player's
; Evidence: op $5438 pointers at $01324C (B pickup) and $013282 (A pickup), right after sound $12; executed in 37 corpus runs incl. the TAS; +4 = health ($100 full) per NOTES Player records
; ---------------------------------------------------------------------------
Drop_CollectHealth:
	movea.l	a2,a1
	bsr.w	Item_BoxPlayer
	addi.w	#$10,$4(a0)
	cmpi.w	#$100,$4(a0)
	ble.s	loc_0132B0
	move.w	#$100,$4(a0)
loc_0132B0:
	rts
