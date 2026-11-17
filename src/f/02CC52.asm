; ---------------------------------------------------------------------------
; Enemy_Gunship_SpawnB - wave spawner: the second grey gunship (Scr2_02CC5E, body $2800A) that cycles through 4 player-relative windows ($1680 ticks each: below-right, above-right, above-left, below-left); its attack fires fans of 4 homing missiles (headings $230/$215/$1EB/$1D0) twice, then 4 single missiles heading $200 $780 ticks apart, and loops
; In:  d4/d5 = x/y
; Out: as Obj_SpawnEnemy
; Evidence: Scr5_021D3A +$490; Op2_SetVector2HereGotoArgs4 windows into $27D34; the fan subroutines $2CD8E-$2CE7C use Spawn_AtOwnOfs with Enemy_Missile_SpawnGunshipA-D; f65900 shows the grey ship at the right edge; TAS 65685-66611
; ---------------------------------------------------------------------------
Enemy_Gunship_SpawnB:
	lea.l	(Scr2_02CC5E).l,a2
	jmp	(Obj_SpawnEnemy).l
