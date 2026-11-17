; ---------------------------------------------------------------------------
; Player_InitRecords - resets both player records for a new game: picks which one is in play from g_StartPad, gives the Robin flag from g_P1IsRobin, sets pad numbers and default score/health/lives/weapon
; In:  g_StartPad, g_P1IsRobin
; Out: g_Player1/g_Player2: +$3E = pad 1/2, +$25 flags (bit 0 in play, bit 2 right-hand HUD side for P2, bit 3 Robin, bit 7 panel in text mode) and the fields set by $101A8 (score 0, health $100, lives from $F618, weapon type/level from $F614/$F616)
; Evidence: callers $9EFC (fall-through) and $9F36 (title flow); RAM: robin scenario (g_P1IsRobin=$FF) g_Player1+$25 = $19, Batman TAS +$25 = $31 and g_Player2 $84/$8C; HUD at TAS frame 3000 shows the score, ring and 2 lives that match +0/+4/+6
; ---------------------------------------------------------------------------
Player_InitRecords:
	move.b	#$1,d0
	move.b	#$84,d1
	cmpi.b	#$2,(g_StartPad).w
	bne.s	loc_011068
	move.b	#$0,d0
	move.b	#$85,d1
loc_011068:
	tst.b	(g_P1IsRobin).w
	beq.s	loc_011074
	ori.b	#$8,d0
	bra.s	loc_011078
loc_011074:
	ori.b	#$8,d1
loc_011078:
	lea.l	(g_Player1).l,a0
	move.w	#$1,$3E(a0)
	move.b	d0,$25(a0)
	lea.l	(g_Player2).l,a1
	move.w	#$2,$3E(a1)
	move.b	d1,$25(a1)
	bsr.w	Player_InitRecord
	movea.l	a1,a0
	bra.w	Player_InitRecord
