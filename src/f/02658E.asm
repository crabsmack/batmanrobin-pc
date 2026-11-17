; ---------------------------------------------------------------------------
; Boss_Stage2_SpawnEndLadder - boss native at the end of the defeat: spawns a 6-block rope-ladder object Scr2_0265A4 (the ladder-thug body $0265F2, hung from the wreck) whose coroutine waits $B40 frames and then starts the section exit (Section_StartExitN, timer $530)
; In:  d4/d5 = position
; Out: d0 = 0 (quits the caller when the pool is full)
; Evidence: op $2CC4 at $025DFA; tail Enemy_LadderThug_InitLadder; TAS f34742 (one frame); the exit to the flying section follows at f35416
; ---------------------------------------------------------------------------
Boss_Stage2_SpawnEndLadder:
	lea.l	(Scr2_0265A4).l,a2
	move.w	#$6,d7
	jsr	(Obj_SpawnEnemyNOrQuit).l
	jmp	(Enemy_LadderThug_InitLadder).l
