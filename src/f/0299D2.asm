; ---------------------------------------------------------------------------
; Enemy_SpinMine_SpawnDown - spawns a spinning mine (Scr2_0299DE, body $28468: energy $40, touch damage) that first flies to a point 20 px below where it was launched, then makes random swoops and homes on the player
; In:  d4/d5 = x/y (the launcher's position)
; Out: as Obj_SpawnEnemy
; Evidence: dc.l in the a2 subroutine $2999E (Spawn_AtOwnPos unless a player box is within $1E px), gosub'd by the scripts $2D1AA/$2D2E2 and the beam ship's attacks $2E2A8/$2E5A6, and by the third turret's script $2F088; frames $3A20-$3A52 rendered: a dark orb that grows and spins a blade; fuzz-41400 / fuzz-64800 only
; ---------------------------------------------------------------------------
Enemy_SpinMine_SpawnDown:
	lea.l	(Scr2_0299DE).l,a2
	jmp	(Obj_SpawnEnemy).l
