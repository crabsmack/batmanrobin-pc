; ---------------------------------------------------------------------------
; Enemy_Barrel_Spawn - spawns a barrel (Scr2_0253DC) at d4/d5 for Enemy_BarrelThug_ThrowBarrel
; In:  d4/d5 = position
; Out: a6 = barrel, d0 = 0 (nonzero when the pool is full)
; Evidence: only caller Enemy_BarrelThug_ThrowBarrel ($02539C); RAM f28320: objects in Scr2_0253DC at x 228 y 180 (the barrel on the floor in shot f28320)
; ---------------------------------------------------------------------------
Enemy_Barrel_Spawn:
	lea.l	(Scr2_0253DC).l,a2
	jmp	(Obj_SpawnEnemy).l
