; ---------------------------------------------------------------------------
; Stage1_BombingStartWave - last step of the bank bombing: starts the wave director whose script the trigger entry gave (kept in $38(a6))
; In:  a6 = bombing director
; Out: via Wave_Start (a5 = wave script)
; Evidence: Op2_CallNativeRelSaved at $020D72 in Scr2_020CF6, then Op2_Kill; TAS f2118
; ---------------------------------------------------------------------------
Stage1_BombingStartWave:
	movea.l	$38(a6),a5
	jmp	(Wave_Start).l
; ---------------------------------------------------------------------------
; Stage1_BombingExplosionRow - bombing native: spawns the a4 effect script Scr4_020E5C at the screen centre: the row of explosions that sweeps the bank's upper floor
; In:  a6 = bombing director
; Out: via Fx_SpawnAtScreenCentre
; Evidence: first native of Scr2_020CF6 ($020D14), 20 frames after the trigger; shots f1760/f1790: explosions running along the upper windows
; ---------------------------------------------------------------------------
Stage1_BombingExplosionRow:
	lea.l	(Scr4_020E5C).l,a4
	jmp	(Fx_SpawnAtScreenCentre).l
; ---------------------------------------------------------------------------
; Stage1_BombingBigBlast - bombing native: spawns the a4 effect script Scr4_020F28 at the screen centre: the large blast at the window where the fire then starts
; In:  a6 = bombing director
; Out: via Fx_SpawnAtScreenCentre
; Evidence: $020D5A in Scr2_020CF6, right after Pal_FlashWhite and Shake_StartBig and before Stage1_BombingStartFire; shot f2116
; ---------------------------------------------------------------------------
Stage1_BombingBigBlast:
	lea.l	(Scr4_020F28).l,a4
	jmp	(Fx_SpawnAtScreenCentre).l
; ---------------------------------------------------------------------------
; Stage1_BombingStartFire - bombing native: starts the building fire at the upper window nearest the screen centre (camera x + $A0)
; In:  -
; Out: via Stage1_StartFireAt (d4 = g_CameraX + $A0)
; Evidence: $020D5E in Scr2_020CF6; the fire burns from f2116 on (png f002400)
; ---------------------------------------------------------------------------
Stage1_BombingStartFire:
	move.w	(g_CameraX).l,d4
	addi.w	#$A0,d4
	jmp	(Stage1_StartFireAt).l
; ---------------------------------------------------------------------------
; Stage1_BombingBlastLeftSign - bombing native: blasts the bank sign under screen x $4D (the left one)
; In:  -
; Out: via Stage1_BlastSignAt (d4 = g_CameraX + $4D)
; Evidence: $020D24 in Scr2_020CF6 after a white flash; png f002400 shows both signs blasted
; ---------------------------------------------------------------------------
Stage1_BombingBlastLeftSign:
	move.w	(g_CameraX).l,d4
	addi.w	#$4D,d4
	bra.w	Stage1_BlastSignAt
; ---------------------------------------------------------------------------
; Stage1_BombingBlastRightSign - bombing native: blasts the bank sign under screen x $F4 (the right one)
; In:  -
; Out: via Stage1_BlastSignAt (d4 = g_CameraX + $F4)
; Evidence: $020D34 in Scr2_020CF6, 100 frames after the left sign
; ---------------------------------------------------------------------------
Stage1_BombingBlastRightSign:
	move.w	(g_CameraX).l,d4
	addi.w	#$F4,d4
	bra.w	Stage1_BlastSignAt
