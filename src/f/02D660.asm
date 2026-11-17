; ---------------------------------------------------------------------------
; Enemy_Biplane_SpawnShuttle - wave spawner: a red biplane (Scr2_02D66C) that flies 180 px in, then shuttles +-12 px left/right while its speed rises from $355 to $6AA, then dashes 600 px left off the screen
; In:  d4/d5 = x/y
; Out: as Obj_SpawnEnemy
; Evidence: 27 entries in Scr5_023C78 (+$FA.. columns of 9 at x $154, y $2026-$2166 step $28, and +$3AC..), all kill-group members (action 1); fuzz runs only
; ---------------------------------------------------------------------------
Enemy_Biplane_SpawnShuttle:
	lea.l	(Scr2_02D66C).l,a2
	jmp	(Obj_SpawnEnemy).l
