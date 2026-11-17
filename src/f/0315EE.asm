; ---------------------------------------------------------------------------
; Enemy_MissileDome_SpawnFan - spawns a missile dome that waits $1E0, opens, then launches a fan of three Enemy_Missile_SpawnSlow (headings $250/$200/$1B0, $1E frames straight, $78/$A0/$C8 homing), closes after $5A0 and repeats after $780
; In:  d4/d5 = trigger x/y; a1 = the entry's extra word (timer 1)
; Out: as Spawn_SetTimer1FromEntry; pops one return address when the pool is short
; Evidence: trigger entry Tbl_0307C2 +$64C; body $2FC42 (energy $100, score $64, same hatch frames); three GosubArgs ops at $3161C-$31638 to the launch subroutine $316CE; TAS f71417
; ---------------------------------------------------------------------------
Enemy_MissileDome_SpawnFan:
	lea.l	(Scr2_031600).l,a2
	jsr	(Obj_SpawnEnemyOrQuit).l
	jmp	(Spawn_SetTimer1FromEntry).l
