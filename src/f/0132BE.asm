; ---------------------------------------------------------------------------
; Item_LaunchFromHit - shared tail of the FromHit item spawners: gives the new item an x velocity of half the knockback of the box just struck, so it flies off in the hit's direction
; In:  a3 = the new item's motion block (left by the spawn); g_StruckBox
; Out: +$18(a3) = box +$22 knockback / 2; d0 = 0 (success)
; Evidence: jmp'd to by the 11 FromHit spawners (spawn actions 4, 7, ..., 34) after Obj_SpawnEnemyOrQuit; box +$22 = knockback set by PlayerAtk_ApplyHit by the attacker's facing; a3 +$18 = x velocity (NOTES a3 motion block); 51 TAS frames (6969-118546)
; ---------------------------------------------------------------------------
Item_LaunchFromHit:
	movea.l	(g_StruckBox).w,a0
	move.l	$22(a0),d0
	asr.l	#$1,d0
	move.l	d0,$18(a3)
	moveq	#$0,d0
	rts
