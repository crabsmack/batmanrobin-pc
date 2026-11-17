; ---------------------------------------------------------------------------
; Fx_SpawnAimed - native callback: runs an a4 spawn script at a mirrored offset with the object's own heading
; In:  a3 = motion block (+$C.l heading), a2 = script: dc.w dx, dy, dc.l a4 script
; Out: a2 past the arguments; a1-a6 preserved
; Evidence: d3 = $C(a3) then Fx_SpawnAtOfsWithAngle; script uses $3282A-$328F4 (TAS f70508-f75915)
; ---------------------------------------------------------------------------
Fx_SpawnAimed:
	move.l	$C(a3),d3
	bra.s	Fx_SpawnAtOfsWithAngle
