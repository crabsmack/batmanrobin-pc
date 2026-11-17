; ---------------------------------------------------------------------------
; Enemy_Gunship_SpawnA - wave spawner: the grey gunship (Scr2_02C6F2, body $2800A: energy $A00, contact attack box with knockback) that holds 240-260 px right of the targeted player for 2 x $2580 ticks, closes in onto it (+-$14) for $3840, repeats, then dashes 300 px left at cruise $D55 and starts over; launches homing missiles: 2 rounds of 10 fast single ones (subroutines $2CF1A-$2CFAE), then 4 fans of 4 (Gunship B's $2CD8E-$2CDE8), then 5 rounds of 4 single ones heading $200
; In:  d4/d5 = x/y
; Out: as Obj_SpawnEnemy
; Evidence: Tbl_023A84 +$B0 (drop action $2D) and Scr5_023B38 +$14; Op2_GosubArgs entry $2D04 (3 args: homing time, straight time, heading) into subroutines that call Spawn_AtOwnOfs with Enemy_Missile_SpawnGunship* unless a player box is within $1E px; shot at f52500: a large grey ship firing at the right edge; TAS 52346-57737
; ---------------------------------------------------------------------------
Enemy_Gunship_SpawnA:
	lea.l	(Scr2_02C6F2).l,a2
	jmp	(Obj_SpawnEnemy).l
