; ---------------------------------------------------------------------------
; Obj_SpawnEnemyOrQuit - spawns a 4-block enemy script object like Obj_SpawnEnemy, but returns to the caller's caller when the pool is short
; In:  a2 = script, d4/d5 = x/y
; Out: a6 = the object, d0 = 0; on failure pops one return address
; Evidence: 62 callers ($133F2, $1FFA6, $20082, $29DCC...): `jsr Obj_SpawnEnemyOrQuit` then code that sets up the new object; 481 TAS frames
; ---------------------------------------------------------------------------
Obj_SpawnEnemyOrQuit:
	moveq	#$4,d7
