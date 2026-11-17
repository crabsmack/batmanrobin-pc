; ---------------------------------------------------------------------------
; WeaponDrop_SpawnOnScreen - calls the spawner in the script's dc.l at screen position d4/d5 made world-relative by adding g_CameraX/g_CameraY
; In:  d4/d5 = screen x/y, a2 = script at the dc.l spawner
; Out: the spawner's result; a2-a6 preserved, a2 past the long
; Evidence: tail of WeaponDrop_FromTop/FromRight and of the never-run entry $13F92; 15 corpus runs (continue, game-over, level-skip)
; ---------------------------------------------------------------------------
WeaponDrop_SpawnOnScreen:
	movea.l	(a2)+,a0
	movem.l	a2-a6,-(a7)
	add.w	(g_CameraX).w,d4
	add.w	(g_CameraY).w,d5
	jsr	(a0)
	movem.l	(a7)+,a2-a6
	rts
; ---------------------------------------------------------------------------
; WeaponDrop_ItemsOut - director native for the op $2D30 switch: 1 while any weapon or power-up pickup is alive (g_WeaponItemCount), else 0
; In:  none
; Out: d0 = 0 or 1
; Evidence: op $2D30 at $13FCC/$13FF4/$1401C/$1403E/$14060: the director waits while a pickup is out, then checks g_WeaponLvlMax (op $3CEE) before dropping
; ---------------------------------------------------------------------------
WeaponDrop_ItemsOut:
	move.w	(g_WeaponItemCount).w,d0
	beq.s	loc_013FC2
	moveq	#$1,d0
loc_013FC2:
	rts
