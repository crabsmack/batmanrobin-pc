; ---------------------------------------------------------------------------
; Enemy_JetTrooper_SpawnHoverOnJitter - spawns a jetpack trooper (body $423CE) placed with a wide jitter ($12C, $C8) that hovers in a window on the targeted player (args $50,-$50,$50,-$50) at cruise $1AA and turn rate $555, re-aiming every $3C0 ticks
; In:  d4/d5 = position
; Out: jumps to Obj_SpawnEnemy with a2 = its script (d0 = 0 and a6 = the new object; nonzero d0 when the pool is short)
; Evidence: the stub shape in the jet trooper spawner series; Scr2_045400: Enemy_JetTrooper_SpawnHoverOn's script with turn rate $555, accel/cruise $1AA, JitterPos $12C $C8, timer $3C0; the four ROM longs $000453F4 ($01317E, $0131EC, $046AB6, $046BAA) are a4-script words (Op4_Spawn / Op4_Fork operand 0004, then the op $53F4), not references: no caller known; no corpus run reaches it (raw-stubs 2026-10-30)
; ---------------------------------------------------------------------------
Enemy_JetTrooper_SpawnHoverOnJitter:
	lea.l	(Scr2_045400).l,a2
	jmp	(Obj_SpawnEnemy).l
