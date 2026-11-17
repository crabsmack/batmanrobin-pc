; ---------------------------------------------------------------------------
; Enemy_VentTank_SpawnSlow - spawns the lab's vent tank (a tracked machine with five nozzles on top) at floor height $9E, slow cruise $80: it drives at the targeted player, stops, then vents gas puffs from its nozzles for $F00 ticks and rests $780, repeating
; In:  d4/d5 = x/y (trigger entry)
; Out: as Obj_SpawnEnemy
; Evidence: Scr2_04598A: score $C8, energy $300, x accel/cruise $80, coroutine 0 $459DA (gosub $41D50 chase, wait $1E0, accel 0, vent gosub $45AF2, wait $F00, gosub $42E0C (anim $42E24, empty coroutine 1: venting off), wait $780), body $42D74 (population +3, attack box EnemyAtk_HitKnockSpeed $20/$200, treads by Stage4_TreadFrameByX); vents = Fx_SpawnAtOfs (0, -$18) with $45B8C, puffs hit through Enemy_VentTank_PuffHit; Tbl_041000 trigger entries x 3680/3936/4192 (spawn action $12); shot w29_c121230; TAS 121095-121263 (3 spawns)
; ---------------------------------------------------------------------------
Enemy_VentTank_SpawnSlow:
	lea.l	(Scr2_04598A).l,a2
	jmp	(Obj_SpawnEnemy).l
