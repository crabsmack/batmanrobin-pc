; ---------------------------------------------------------------------------
; WeaponDrop_Command - gives the weapon-drop director (g_WeaponDropper) a command script in its coroutine slot 0, spawning the director (boss-class object running Scr2_013F2A) when none exists
; In:  a1 = pointer to the command script long (trigger entry or a2 operand)
; Out: $38(a4) of the director = the script; g_WeaponDropper = the director; d0 = 0
; Evidence: trigger-table callback (dc.l $13F02 then the script) at $163A4/$166E2/$16924/$21438/$21D46/$21DA4/$2355C (stage trigger tables); Scr2_013F2A steps coroutine 0 every frame ($235A, Op2_YieldGoto); 17 TAS frames (4518-114869)
; ---------------------------------------------------------------------------
WeaponDrop_Command:
	move.w	(g_WeaponDropper).w,d0
	beq.s	loc_013F1A
	movea.w	d0,a6
	movea.w	$A(a6),a4
loc_013F0E:
	move.l	(a1)+,$38(a4)
	move.w	a6,(g_WeaponDropper).w
	moveq	#$0,d0
	rts
loc_013F1A:
	lea.l	(Scr2_013F2A).l,a2
	jsr	(Obj_SpawnBossOrQuit).l
	jmp	loc_013F0E(pc)
