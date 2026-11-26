; ---------------------------------------------------------------------------
; Drop_RandomHealthB - spawn action 49: when the random cooldown g_DropCooldown has run out, re-arms it at random and drops 1, 2, 1 or 3 B health pickups (random)
; In:  d4/d5 = position
; Out: g_DropCooldown; as Drop_HealthB1/2/3
; Evidence: entry 49 of Tbl_012EF0; $995A is the random generator; the jump table at $13030 picks Drop_HealthB1/B2/B1/B3; ran 26 times in the TAS (35471-43059, stage 2). Entry 48 (Drop_RandomHealthA) is the same for the A pickups
; ---------------------------------------------------------------------------
Drop_RandomHealthB:
	subq.w	#$1,(g_DropCooldown).w
	bpl.s	loc_01302E
	jsr	(Rand_Next).l
	move.w	d0,d1
	andi.w	#$7,d1
	move.w	d0,(g_DropCooldown).w
	lsr.w	#$8,d0
	andi.w	#$6,d0
	jmp	Drop_RandomHealthB_case0(pc,d0.w)
loc_01302E:
	rts
; ---------------------------------------------------------------------------
; Drop_RandomHealthB_case0 - jump-table case 0 of Drop_RandomHealthB: one B health pickup (bra Drop_HealthB1)
; In:  as Drop_RandomHealthB
; Out: as Drop_HealthB1
; Evidence: the jmp $13030(pc,d0.w) table of Drop_RandomHealthB, d0 = 0 (cases 2/6: Drop_RandomHealthB_case2/case6, bra.s Drop_HealthB2/B3)
; ---------------------------------------------------------------------------
Drop_RandomHealthB_case0:
	bra.s	Drop_HealthB1
