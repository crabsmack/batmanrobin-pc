; ---------------------------------------------------------------------------
; BossMeter_AddPart - native callback for a multi-part boss: adds this part's energy $52 to the meter's full and current energy
; In:  a6 = boss part object (+$52 energy)
; Out: g_BossEnergyMax and g_BossEnergy increased
; Evidence: op $2CBC pointers in scripts $0261BC and $02918C; ran twice in the TAS (32140, 45123) after BossMeter_Show
; ---------------------------------------------------------------------------
BossMeter_AddPart:
	move.w	$52(a6),d0
	add.w	d0,(g_BossEnergyMax).w
	add.w	d0,(g_BossEnergy).w
	rts
; ---------------------------------------------------------------------------
; BossMeter_Defeated - native callback when a boss dies: zeroes the meter's energy, switches the meter object to its exit script Scr2_011B8E and ends BOSS TIME (HUD_UnlockJoin)
; In:  none
; Out: g_BossEnergy = 0; coroutine slot 0 ($38 of the meter's a4) = Scr2_011B8E
; Evidence: 11 op $2CBC pointers in boss scripts, e.g. $0199FA right after Hitbox_WipeEnemies; executed in the TAS and scenario-ending; Scr2_011B8E waits, then calls BossMeter_Free and Op2_Kill
; ---------------------------------------------------------------------------
BossMeter_Defeated:
	clr.w	(g_BossEnergy).w
	move.w	(g_BossMeterObj).w,d0
	beq.s	loc_011B30
	movea.w	d0,a0
	movea.w	$A(a0),a0
	move.l	#$11B8E,$38(a0)
	bra.w	HUD_UnlockJoin
loc_011B30:
	rts
