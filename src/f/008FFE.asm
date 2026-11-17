; ---------------------------------------------------------------------------
; Hitbox_WipeEnemies - native callback: deals $200 damage to every enemy box and pauses spawning for 15 frames (screen clear)
; In:  none
; Out: via Hitbox_DamageAllEnemies (d1 = 0 attacker)
; Evidence: sets g_ScreenClearTimer = $F (spawn walker $12E02 and on-screen tests skip while it runs); script uses after op $2CBC at $199F4, $1D854, $29344, $3D1C8, $462A4; 4 TAS frames
; ---------------------------------------------------------------------------
Hitbox_WipeEnemies:
	move.w	#$F,(g_ScreenClearTimer).w
	move.w	#$200,d0
	move.w	#$0,d1
	bra.w	Hitbox_DamageAllEnemies
; ---------------------------------------------------------------------------
; Fx_SpawnAtOfsAngle - native callback: runs an a4 spawn script at the object's x/y plus an unmirrored offset, with the heading given in the script
; In:  a6 = object, a3 = its motion block, a5 = sprite block, a2 = script: dc.w heading, dx, dy, dc.l a4 script
; Out: a2 past the arguments; a1-a6 preserved
; Evidence: d3 = heading << 16 (the new a4 object's $2A heading), d4/d5 = ($3E + dx, $42 + dy) << 16, d2 = 0, then Fx_RunScriptArg; 114 script uses after op $2CBC ($29CBC...)
; ---------------------------------------------------------------------------
Fx_SpawnAtOfsAngle:
	moveq	#$0,d3
	move.w	(a2)+,d3
	swap	d3
	moveq	#$0,d2
	moveq	#$0,d4
	moveq	#$0,d5
	move.w	$3E(a6),d4
	move.w	$42(a6),d5
	add.w	(a2)+,d4
	add.w	(a2)+,d5
	swap	d4
	swap	d5
	bra.w	Fx_RunScriptArg
