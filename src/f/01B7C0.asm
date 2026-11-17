; ---------------------------------------------------------------------------
; Enemy_Clown_Spawn - spawns the standard clown (palette line 3, energy $80) that drops in from above; its prologue heads the shared clown body
; In:  d4/d5 = x/y (Tbl_0166EA +$B2/+$21E, Tbl_0174F0 +$6B2/+$6BA)
; Out: as Obj_SpawnEnemy
; Evidence: Scr2_01B7CC = ModSprAttr $98/$60, SetEnergy $80, coroutine 0 $1BAD6 (fall in); every clown variant runs the body in this span; TAS f13431-f18590
; ---------------------------------------------------------------------------
Enemy_Clown_Spawn:
	lea.l	(Scr2_01B7CC).l,a2
	jmp	(Obj_SpawnEnemy).l
