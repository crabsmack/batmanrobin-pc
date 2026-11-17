; ---------------------------------------------------------------------------
; Enemy_Biplane_SpawnZigzagDown - wave spawner: a red biplane (Scr2_02D892) that flies 160 px in, then zigzags +-20 px with every leg 4 px lower while speeding up ($500-$655), then turns back up and leaves left; paired with Enemy_Biplane_SpawnZigzagUp
; In:  d4/d5 = x/y
; Out: as Obj_SpawnEnemy
; Evidence: Scr5_023C78 +$1A0/+$20E/+$276 (x $154, y $20C4, right before ZigzagUp at the same point); TAS 47924-66203
; ---------------------------------------------------------------------------
Enemy_Biplane_SpawnZigzagDown:
	lea.l	(Scr2_02D892).l,a2
	jmp	(Obj_SpawnEnemy).l
