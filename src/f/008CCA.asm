; ---------------------------------------------------------------------------
; Hitbox_FindNextBoxOfId - continues a Hitbox_FindPlayerBoxOfId scan from the box after a2
; In:  a2 = last box, d3 = id, d4-d7 = rectangle
; Out: Z clear, d0 = -1, a2 = next matching box; Z set at the end
; Evidence: reached from Hitbox_FindPlayerBoxOfId's loop; same compares plus `cmp.w $10(a2),d3`
; ---------------------------------------------------------------------------
Hitbox_FindNextBoxOfId:
	movea.w	(a2),a2
	tst.w	(a2)
	beq.s	Hitbox_ScanOfIdDone
; ---------------------------------------------------------------------------
; Hitbox_TestBoxOfId - the id-and-overlap test of the id scans: box a2's id against d3, then its rectangle against d4-d7; a miss goes on with the next box
; In:  a2 = box, d3 = id, d4 left, d5 top, d6 right, d7 bottom
; Out: Z clear, d0 = -1, a2 = the first matching box from a2 on; Z set at the end of the list
; Evidence: bne.s from Hitbox_FindPlayerBoxOfId; a function of the generated code (P4 entries)
; ---------------------------------------------------------------------------
Hitbox_TestBoxOfId:
	cmp.w	$10(a2),d3
	bne.s	Hitbox_FindNextBoxOfId
	cmp.w	$16(a2),d4
	bgt.s	Hitbox_FindNextBoxOfId
	cmp.w	$12(a2),d6
	blt.s	Hitbox_FindNextBoxOfId
	cmp.w	$18(a2),d5
	bgt.s	Hitbox_FindNextBoxOfId
	cmp.w	$14(a2),d7
	blt.s	Hitbox_FindNextBoxOfId
	moveq	#-$1,d0
; ---------------------------------------------------------------------------
; Hitbox_ScanOfIdDone - the rts the id scans end on
; In:  none
; Out: none (the condition codes of the last test or the moveq)
; Evidence: beq.s at the end of the list in Hitbox_FindNextBoxOfId; a function of the generated code (P4 entries)
; ---------------------------------------------------------------------------
Hitbox_ScanOfIdDone:
	rts
; ---------------------------------------------------------------------------
; Hitbox_DamageAllEnemies - adds d0 damage to every box on g_EnemyBoxList and marks each one struck by attacker d1
; In:  d0 = damage, d1 = attacker id
; Out: a0 = the list sentinel
; Evidence: per box: bset #0,$3C (the struck flag Obj_TestStruck tests), clr $22, add d0 to $20 (damage the owner's script subtracts from its energy, $39FA), $1A = d1; callers Hitbox_WipeEnemies and $1375E, both with d0 = $200
; ---------------------------------------------------------------------------
Hitbox_DamageAllEnemies:
	movea.w	(g_EnemyBoxList).w,a0
loc_008CF6:
	tst.w	(a0)
	beq.s	loc_008D10
	bset	#$0,$3C(a0)
	clr.w	$22(a0)
	add.w	d0,$20(a0)
	move.w	d1,$1A(a0)
	movea.w	(a0),a0
	bra.s	loc_008CF6
loc_008D10:
	rts
; ---------------------------------------------------------------------------
; Spawn_AtScreenCentre - native callback: calls the routine named by the script's dc.l with d4/d5 = the screen centre (camera + $A0, $70)
; In:  a2 = script after the call op: dc.l routine, then its arguments
; Out: a2 = past the arguments the routine consumed (it gets them in a1); a3-a6 preserved
; Evidence: 26 script uses after op $2CBC ($46F18...); the routines are enemy spawners such as $43ACC (`lea script,a2 / jmp Obj_SpawnEnemy`)
; ---------------------------------------------------------------------------
Spawn_AtScreenCentre:
	movea.l	(a2)+,a0
	move.w	(g_CameraX).w,d4
	move.w	(g_CameraY).w,d5
	addi.w	#$A0,d4
	addi.w	#$70,d5
	movea.l	a2,a1
	movem.l	a3-a6,-(a7)
	jsr	(a0)
	movea.l	a1,a2
	movem.l	(a7)+,a3-a6
	rts
; ---------------------------------------------------------------------------
; Spawn_AtCameraOfs - native callback: calls the script's dc.l routine with d4/d5 = camera position + the two offset words that follow it
; In:  a2 = script: dc.l routine, dc.w dx, dy, then the routine's arguments
; Out: a2 past the arguments; a3-a6 preserved
; Evidence: script uses after op $2CBC at $1E7F4, $1E956, $3CC70
; ---------------------------------------------------------------------------
Spawn_AtCameraOfs:
	movea.l	(a2)+,a0
	move.w	(a2)+,d4
	move.w	(a2)+,d5
	add.w	(g_CameraX).w,d4
	add.w	(g_CameraY).w,d5
	movea.l	a2,a1
	movem.l	a3-a6,-(a7)
	jsr	(a0)
	movea.l	a1,a2
	movem.l	(a7)+,a3-a6
	rts
; ---------------------------------------------------------------------------
; Spawn_AtAnchor - native callback: calls the script's dc.l routine with d4/d5 = the object's anchor point $46/$4A
; In:  a6 = object, a2 = script: dc.l routine + its arguments
; Out: a2 past the arguments; a3-a6 preserved
; Evidence: $46/$4A are the copy of x/y made by op $1282 and moved by op $12A6; 22 script uses in $38886-$38A58, e.g. routine $38CA4 (spawns an enemy with Obj_SpawnEnemy)
; ---------------------------------------------------------------------------
Spawn_AtAnchor:
	move.w	$46(a6),d4
	move.w	$4A(a6),d5
	bra.s	Spawn_CallAtD4D5
; ---------------------------------------------------------------------------
; Spawn_AtDrawPos - native callback: calls the script's dc.l routine with d4/d5 = the object's drawn position $4E/$50
; In:  a6 = object, a2 = script: dc.l routine + its arguments
; Out: a2 past the arguments; a3-a6 preserved
; Evidence: $4E/$50 = x and y minus height ($1A1F2, $1EB2C) and the base of its collision boxes ($4EB8); 11 script uses at $38918-$389E6
; ---------------------------------------------------------------------------
Spawn_AtDrawPos:
	move.w	$4E(a6),d4
	move.w	$50(a6),d5
	bra.s	Spawn_CallAtD4D5
; ---------------------------------------------------------------------------
; Spawn_AtOwnPos - native callback: calls the script's dc.l routine with d4/d5 = the object's x/y ($3E/$42)
; In:  a6 = object, a2 = script: dc.l routine + its arguments
; Out: a2 past the arguments; a3-a6 preserved
; Evidence: 21 script uses ($299AE, $2F12A, $3FB48...), e.g. routine $299D2 (Obj_SpawnEnemy with script $299DE)
; ---------------------------------------------------------------------------
Spawn_AtOwnPos:
	move.w	$3E(a6),d4
	move.w	$42(a6),d5
; ---------------------------------------------------------------------------
; Spawn_CallAtD4D5 - shared tail: calls the script's dc.l routine with the position in d4/d5 and the script pointer in a1, then resumes after what it consumed
; In:  d4/d5 = position, a2 = script at the dc.l routine
; Out: a2 = a1 as the routine left it; a3-a6 preserved
; Evidence: entered by Spawn_AtAnchor / Spawn_AtDrawPos / Spawn_AtOwnPos
; ---------------------------------------------------------------------------
Spawn_CallAtD4D5:
	movea.l	(a2)+,a0
	movea.l	a2,a1
	movem.l	a3-a6,-(a7)
	jsr	(a0)
	movea.l	a1,a2
	movem.l	(a7)+,a3-a6
	rts
; ---------------------------------------------------------------------------
; Spawn_AtOwnOfs - native callback: calls the script's dc.l routine at the object's x/y plus a dx/dy offset mirrored by the sprite's flip bits
; In:  a6 = object, a5 = its sprite block ($38 attr: bit 11 h-flip negates dx, bit 12 v-flip negates dy), a2 = script: dc.l routine, dc.w dx, dy, arguments
; Out: a2 past the arguments; a3-a6 preserved
; Evidence: 86 script uses ($29DBE, $2CD9E...), e.g. routine $29DCC (Obj_SpawnEnemyOrQuit); NOTES: $3D934 entered by its jsr (a0) at $8DAC
; ---------------------------------------------------------------------------
Spawn_AtOwnOfs:
	movea.l	(a2)+,a0
	move.w	$3E(a6),d4
	move.w	$42(a6),d5
	move.w	(a2)+,d0
	btst	#$3,$38(a5)
	beq.s	loc_008D96
	neg.w	d0
loc_008D96:
	add.w	d0,d4
	move.w	(a2)+,d0
	btst	#$4,$38(a5)
	beq.s	loc_008DA4
	neg.w	d0
loc_008DA4:
	add.w	d0,d5
	movea.l	a2,a1
	movem.l	a3-a6,-(a7)
	jsr	(a0)
	movea.l	a1,a2
	movem.l	(a7)+,a3-a6
	rts
