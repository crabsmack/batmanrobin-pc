; ---------------------------------------------------------------------------
; Enemy_Copter_SpawnRed - wave-script spawner of the small red propeller drone (Scr2_01F496: energy $10, speed $300): it flies forward for the delay word taken from the wave script (loop counter 0), then homes on a player and explodes on contact
; In:  d4/d5 = x/y, a1 = wave-script operand (word delay)
; Out: d0 = 0 (Obj_SpawnScriptedOrQuit returns to the caller's caller when the pool is short); a1 past the word
; Evidence: $14(a4) = the word: Op2_LoopCounter0 at $1F534 counts it before BalancePlayerTarget and the CruiseToPlayer coroutine; Op2_BranchTouchBox -> native $10742, FxSpawnHereAbs, Kill; anim $13A; Tbl_0166EA +$CA2.. (10-byte wave entries); shot f7020 (a column of red drones, tagged)
; ---------------------------------------------------------------------------
Enemy_Copter_SpawnRed:
	jsr	(Obj_SpawnScriptedOrQuit).l
	move.l	#$1F496,$2A(a4)
	move.w	(a1)+,$14(a4)
	moveq	#$0,d0
	rts
