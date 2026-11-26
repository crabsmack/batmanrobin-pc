; ---------------------------------------------------------------------------
; Drop_RandomHealthA - spawn action 48: when the random cooldown g_DropCooldown has run out, re-arms it at random and drops 1, 2, 1 or 3 A health pickups (random)
; In:  d4/d5 = position
; Out: g_DropCooldown; as Drop_HealthA1/2/3
; Evidence: entry 48 of Tbl_012EF0; Drop_RandomHealthB's code with the A pickups (the jmp $13006(pc,d0.w) cases bra.s Drop_HealthA1/A2/A1/A3); never run in the corpus (converted 2026-11-17 after a playtest stopped on $13060)
; ---------------------------------------------------------------------------
Drop_RandomHealthA:
	subq.w	#$1,(g_DropCooldown).w
	bpl.s	loc_013004
	jsr	(Rand_Next).l
	move.w	d0,d1
	andi.w	#$7,d1
	move.w	d0,(g_DropCooldown).w
	lsr.w	#$8,d0
	andi.w	#$6,d0
	jmp	Drop_RandomHealthA_case0(pc,d0.w)
loc_013004:
	rts
