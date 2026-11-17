; ---------------------------------------------------------------------------
; Enemy_Brute_SpawnPhased - spawns the brute (palette line 3, energy $D0) whose behaviour vector 1 is switched on timers ($F0, $F0, $5A0 frames) by its own loop at $1AF4C
; In:  d4/d5 = x/y (trigger Tbl_0163BA)
; Out: as Obj_SpawnEnemy
; Evidence: Scr2_01AF1A: vector-2 +8 goto = $1AF4C (SetVector1HereGoto / SetTimer0 $F0, $F0, $5A0, loops); its prologue heads the shared brute body; Gem Expo trigger table Tbl_0163BA; TAS f6766-f18529 (13 frames)
; ---------------------------------------------------------------------------
Enemy_Brute_SpawnPhased:
	lea.l	(Scr2_01AF1A).l,a2
	jmp	(Obj_SpawnEnemy).l
