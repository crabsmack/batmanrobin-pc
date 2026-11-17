; ---------------------------------------------------------------------------
; Boss_Stage2_TurretDestroyed - native fragment of the turret script when its energy runs out: ORs its bit (script var 0) into g_Stage2TurretsDown, then resumes at $02621E (score, explosion, wreck animation)
; In:  a2 script context, a4 = turret script block (+$22 var 0 = 1/2/4/8)
; Out: jsr $7FE.w
; Evidence: reached by Op2_TakeDamage's branch at $0261F4; TAS f32655-f33170 (4 runs, one per turret)
; ---------------------------------------------------------------------------
Boss_Stage2_TurretDestroyed:
	move.w	$22(a4),d0
	or.w	d0,(g_Stage2TurretsDown).w
	jsr	($7FE).w
