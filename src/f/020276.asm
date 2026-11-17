; ---------------------------------------------------------------------------
; Stage1_BalloonUseHeightsA - tail of Stage1_SpawnBalloonLauncherB: stores the balloon height-pair table Tbl_020354 in the new launcher's $1E and reports success
; In:  a6 = the launcher just spawned
; Out: $1E(a6) = Tbl_020354, d0 = 0
; Evidence: jmp from $020090; $1E(a6) is the table cursor read by Stage1_LaunchBalloonA/B
; ---------------------------------------------------------------------------
Stage1_BalloonUseHeightsA:
	move.l	#$20354,$1E(a6)
	moveq	#$0,d0
	rts
; ---------------------------------------------------------------------------
; Stage1_BalloonUseHeightsB - tail of Stage1_SpawnBalloonLauncherA: stores the balloon height-pair table Tbl_02036E in the new launcher's $1E and reports success
; In:  a6 = the launcher just spawned
; Out: $1E(a6) = Tbl_02036E, d0 = 0
; Evidence: jmp from $01FFB2; RAM f13800: launcher $1E = $0002037A (3 pairs into Tbl_02036E). A never-run third variant at $02028E (raw) sets $020388
; ---------------------------------------------------------------------------
Stage1_BalloonUseHeightsB:
	move.l	#$2036E,$1E(a6)
	moveq	#$0,d0
	rts
