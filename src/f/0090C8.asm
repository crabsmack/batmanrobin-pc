; ---------------------------------------------------------------------------
; Fx_SpawnAtOfs - native callback: runs an a4 spawn script at the object's x/y plus an offset mirrored by its flip bits, heading 0
; In:  a6 = object, a5 = sprite block, a2 = script: dc.w dx, dy, dc.l a4 script
; Out: a2 past the arguments; a1-a6 preserved
; Evidence: script uses after op $2CBC at $41EB4, $421F6, $45AFE (e.g. dx = 0, dy = -$18 then script $45B8C); 638 TAS frames
; ---------------------------------------------------------------------------
Fx_SpawnAtOfs:
	moveq	#$0,d3
; ---------------------------------------------------------------------------
; Fx_SpawnAtOfsWithAngle - shared body: d4/d5 = object x/y + script dx/dy (negated by a5+$38 bit 11 / bit 12 flip), d2 = 0, then Fx_RunScriptArg
; In:  d3 = heading, a2 = script at dx
; Out: as Fx_SpawnAtOfs
; Evidence: entered from Fx_SpawnAimedN, Fx_SpawnAimed and Fx_SpawnAtOfs
; ---------------------------------------------------------------------------
Fx_SpawnAtOfsWithAngle:
	moveq	#$0,d2
	moveq	#$0,d4
	moveq	#$0,d5
	move.w	$3E(a6),d4
	move.w	$42(a6),d5
	move.w	(a2)+,d0
	btst	#$3,$38(a5)
	beq.s	loc_0090E4
	neg.w	d0
loc_0090E4:
	add.w	d0,d4
	move.w	(a2)+,d0
	btst	#$4,$38(a5)
	beq.s	loc_0090F2
	neg.w	d0
loc_0090F2:
	add.w	d0,d5
	swap	d4
	swap	d5
	bra.w	Fx_RunScriptArg
