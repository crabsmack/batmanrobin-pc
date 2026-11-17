; ---------------------------------------------------------------------------
; SpawnAction_GroupMemberGone - spawn action 2, and Obj_Destroy's call for an object that still carries action 1 (removed, e.g. off screen, without its kill counted): counts the member as gone (+8) and frees the group block once that reaches the member target (+0); no reward
; In:  d1 = group block or 0 (0 = nothing)
; Out: rts
; Evidence: Tbl_012EF0 entry 2; bsr from Obj_Destroy when $19(a6) == 1 with d1 = $1A(a6); addq.w #1,8(a0) / cmp (a0) / ble ObjPool_Free; also the tail of SpawnAction_GroupMemberReward
; ---------------------------------------------------------------------------
SpawnAction_GroupMemberGone:
	tst.w	d1
	bne.s	loc_006FA0
	rts
loc_006FA0:
	movea.w	d1,a0
; ---------------------------------------------------------------------------
; SpawnAction_GroupCountGone - counts one kill-group member gone (+8) and frees the group block once that reaches the member target (+0)
; In:  a0 = group block
; Out: rts, or ObjPool_Free (tail) when every member is gone
; Evidence: addq.w #1,8(a0) / move.w (a0),d0 / cmp.w 8(a0),d0 / ble.w ObjPool_Free / rts; the shared tail of SpawnAction_GroupMemberReward and SpawnAction_GroupMemberGone; 22 corpus runs
; ---------------------------------------------------------------------------
SpawnAction_GroupCountGone:
	addq.w	#$1,$8(a0)
	move.w	(a0),d0
	cmp.w	$8(a0),d0
	ble.w	ObjPool_Free
	rts
; ---------------------------------------------------------------------------
; Op5_Spawn - a5 op: spawns one enemy from a spawn entry (routine $6FEA), then reloads the timer from the interval $2A, switches to the after-quota script when the quota is reached, and yields
; In:  a5 = script: word offset to a spawn entry (word x, word y: $E000-range = screen-relative, $2000-range = world; long spawn action byte + spawner routine)
; Out: yields ($1E = the next op)
; Evidence: bsr $6FEA / move.l a5,$1E / bra $7052; 694 script uses, e.g. $16942 (`6FB2 0052 6AC4 00F0`); entry $16994 `0154 E018 0001 B6FA` (x $154 right of the screen, action 1, spawner $1B6FA); 87 corpus runs
; ---------------------------------------------------------------------------
Op5_Spawn:
	bsr.s	VM5_SpawnEntry
	move.l	a5,$1E(a6)
	bra.w	VM5_AfterSpawn
; ---------------------------------------------------------------------------
; Op5_SpawnMulti - a5 op: spawns n+1 enemies at once from a list of spawn-entry offsets, then as Op5_Spawn reloads the timer, checks the quota and yields
; In:  a5 = script: word n (count - 1), n+1 word entry offsets
; Out: yields
; Evidence: dbra loop over $6FEA; 156 script uses, e.g. $1695A (`6FBC 0002 0038 003E 004C`: 3 enemies); 36 corpus runs
; ---------------------------------------------------------------------------
Op5_SpawnMulti:
	move.w	(a5)+,d0
loc_006FBE:
	move.w	d0,-(a7)
	bsr.w	VM5_SpawnEntry
	move.w	(a7)+,d0
	dbra	d0,loc_006FBE
	move.l	a5,$1E(a6)
	bra.w	VM5_AfterSpawn
; ---------------------------------------------------------------------------
; Op5_SpawnRandom - a5 op: spawns one enemy from a spawn entry picked at random from n, then as Op5_Spawn reloads the timer, checks the quota and yields
; In:  a5 = script: word n, n word entry offsets
; Out: yields ($1E = after the table)
; Evidence: $1E = a5 + 2n, Rand_Below(n) picks the offset for $6FEA; 295 script uses, e.g. $169C4 (`6FD2 0003 0016 001C 0022`); 74 corpus runs
; ---------------------------------------------------------------------------
Op5_SpawnRandom:
	move.w	(a5)+,d0
	lea.l	(a5,d0.w),a0
	adda.w	d0,a0
	move.l	a0,$1E(a6)
	bsr.w	Rand_Below
	add.w	d0,d0
	adda.w	d0,a5
	bsr.s	VM5_SpawnEntry
	bra.s	VM5_AfterSpawn
