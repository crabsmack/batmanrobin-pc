; ---------------------------------------------------------------------------
; Obj_SpawnEnemy - spawns a 4-block script object (enemy, $58 = $A) at d4/d5 running script a2, linked on the main object list
; In:  a2 = a2-VM script, d4/d5 = x/y
; Out: d0 = 0 and a6 = the object on success; Z clear (d0 nonzero) when the pool is short
; Evidence: 199 callers of the form `lea Scr2_x,a2 / jmp Obj_SpawnEnemy` ($11B32, $133DE...); TAS f3000: the enemies' boxes on g_EnemyBoxList belong to $7D2 objects with $58 = $A; 1234 TAS frames
; ---------------------------------------------------------------------------
Obj_SpawnEnemy:
	moveq	#$4,d7
; ---------------------------------------------------------------------------
; Obj_SpawnEnemyN - Obj_SpawnEnemy with a caller-chosen block count d7
; In:  d7 = blocks (main + extras), a2 = script, d4/d5 = x/y
; Out: as Obj_SpawnEnemy
; Evidence: callers $3904E, $39176, $391E4, $3925C (d7 = 5)
; ---------------------------------------------------------------------------
Obj_SpawnEnemyN:
	lea.l	($7D2).w,a0
	bsr.w	Obj_Spawn
	bne.s	loc_008DCE
	move.l	a2,$2A(a4)
	move.w	#$A,$58(a6)
	moveq	#$0,d0
loc_008DCE:
	rts
