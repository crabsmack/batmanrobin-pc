; ---------------------------------------------------------------------------
; Hitbox_TestAttackBoxAt - tests one attack box at position d1/d2: mirrors its rectangle by the sprite's flip bits, then by its kind word looks for overlapping boxes on g_PlayerBoxList (0 = first, 4 = all) or g_EnemyBoxList (8 = first) and strikes them
; In:  a0 = attack box (+6 kind, +8..+$E rect relative); d1/d2 = position; a5 = sprite block (flips at $38), a6 = owner
; Out: hits via Hitbox_Strike/StrikeAll; registers clobbered
; Evidence: movem.w $6(a0),d3-d7 / neg+exg on btst #3/#4,$38(a5) / jmp $4E22(pc,d3.w) (bra $4E32, $4E3A, $4E42; the never-run $4E2E = enemy list, all); callers $4DEC, Op2_TestAttackBoxAtDrawn
; ---------------------------------------------------------------------------
Hitbox_TestAttackBoxAt:
	movem.w	$6(a0),d3-d7
	btst	#$3,$38(a5)
	beq.s	loc_004E08
	neg.w	d4
	neg.w	d6
	exg.l	d4,d6
loc_004E08:
	btst	#$4,$38(a5)
	beq.s	loc_004E16
	neg.w	d5
	neg.w	d7
	exg.l	d5,d7
loc_004E16:
	add.w	d1,d4
	add.w	d1,d6
	add.w	d2,d5
	add.w	d2,d7
	jmp	Hitbox_KindFirstPlayer(pc,d3.w)
; ---------------------------------------------------------------------------
; Hitbox_KindFirstPlayer - entry 0 of Hitbox_TestAttackBoxAt's kind jump table: attack-box kind 0 goes to Hitbox_HitFirstPlayerBox
; In:  d3 = 0 (the kind); d4-d7 = the rectangle; a0 = attack box
; Out: as Hitbox_HitFirstPlayerBox
; Evidence: bra.w Hitbox_HitFirstPlayerBox; reached by jmp $4E22(pc,d3.w) (call-site log)
; ---------------------------------------------------------------------------
Hitbox_KindFirstPlayer:
	bra.w	Hitbox_HitFirstPlayerBox
; ---------------------------------------------------------------------------
; Hitbox_KindAllPlayers - entry 4 of Hitbox_TestAttackBoxAt's kind jump table: attack-box kind 4 goes to Hitbox_HitAllPlayerBoxes
; In:  d3 = 4; d4-d7 = the rectangle; a0 = attack box
; Out: as Hitbox_HitAllPlayerBoxes
; Evidence: bra.w Hitbox_HitAllPlayerBoxes; reached by jmp $4E22(pc,d3.w) with d3 = 4 (call-site log)
; ---------------------------------------------------------------------------
Hitbox_KindAllPlayers:
	bra.w	Hitbox_HitAllPlayerBoxes
; ---------------------------------------------------------------------------
; Hitbox_KindFirstEnemy - entry 8 of Hitbox_TestAttackBoxAt's kind jump table: attack-box kind 8 (the players' attacks) goes to Hitbox_HitFirstEnemyBox
; In:  d3 = 8; d4-d7 = the rectangle; a0 = attack box
; Out: as Hitbox_HitFirstEnemyBox
; Evidence: bra.w Hitbox_HitFirstEnemyBox; reached by jmp $4E22(pc,d3.w) with d3 = 8 (call-site log)
; ---------------------------------------------------------------------------
Hitbox_KindFirstEnemy:
	bra.w	Hitbox_HitFirstEnemyBox
