; ---------------------------------------------------------------------------
; Enemy_ArenaDebris_SpawnThud - spawner of arena debris (body $03B5EC) that hits the floor once with a thud (coroutine 0 $03B684: random left/right cruise, random screen shake, sound $F, sprite shake), bounces off and drops through (terrain box A disabled)
; In:  d4/d5 = position (wave spawn entry)
; Out: jumps to Obj_SpawnEnemy with a2 = Scr2_03C62A
; Evidence: dc.l $03C61E in Scr5_03C4A8 (+$1C, +$3C, +$5C, +$7C) and Scr2_03C34C+$158; TAS f88125-f89327 (17 spawns)
; ---------------------------------------------------------------------------
Enemy_ArenaDebris_SpawnThud:
	lea.l	(Scr2_03C62A).l,a2
	jmp	(Obj_SpawnEnemy).l
