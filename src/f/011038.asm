; ---------------------------------------------------------------------------
; Player_NewGameSetup - starts a new game's player state: copies the boot settings $F610/$F612/$F61E/$F620 to $F634, g_Credits, $F64A, $F64C, then falls into Player_InitRecords
; In:  none
; Out: as Player_InitRecords
; Evidence: one caller, the title flow $9E02 at $9EFC right after the attract/title script; $F612 = 6 at boot ($9E02) becomes the credit count shown by HudOp_ShowCredits; $F64A/$F64C ($100 at boot) are later read as per-player factors by the a2 op $39AC
; ---------------------------------------------------------------------------
Player_NewGameSetup:
	move.w	(g_UnkStart_F610).w,(g_UnkGame_F634).w
	move.w	(g_StartCredits).w,(g_Credits).w
	move.w	(g_StartEnemyScale).w,(g_EnemyEnergyScale).w
	move.w	(g_StartBossScale).w,(g_BossEnergyScale).w
