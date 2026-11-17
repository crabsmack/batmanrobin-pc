; ---------------------------------------------------------------------------
; Obj_ReleaseFrames - releases every sprite frame an object holds: calls Spr_ReleaseFrame for each of the $14(a5) frame words at the start of the sprite block a5, clearing each, then zeroes the count
; In:  a5 = the object's sprite block (words: frame per sprite from +0, count at +$14)
; Out: a1 past the list; d2/d7/a0 clobbered
; Evidence: callers Obj_Destroy, $3B76 (twice) and $102D6 (player reset); same loop as the frame drop at $4508; 2.2k TAS frames
; ---------------------------------------------------------------------------
Obj_ReleaseFrames:
	lea.l	(a5),a1
	move.w	$14(a5),d7
	dbra	d7,loc_0009D0
	rts
loc_0009D0:
	move.w	(a1),d2
	bsr.w	Spr_ReleaseFrame
	clr.w	(a1)+
	dbra	d7,loc_0009D0
	clr.w	$14(a5)
	rts
; ---------------------------------------------------------------------------
; Op2_BranchRandom - a2 op: random branch: d0 = random word (Rand_Next); branches when d0 >= threshold
; In:  a2 = script: words threshold, offset
; Out: branch by offset, else NEXT after it (taken with probability 1 - threshold/65536)
; Evidence: Rand_Next is the RNG over $F5FC/$F5FE; executed in the TAS
; ---------------------------------------------------------------------------
Op2_BranchRandom:
	jsr	(Rand_Next).l
	cmp.w	(a2)+,d0
	bcc.w	Op2_Goto
	addq.l	#$2,a2
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_SwitchRandom - a2 op: weighted random switch: d0 = random word; branches by the offset of the first (threshold, offset) pair whose threshold <= d0
; In:  a2 = script: (word threshold, word offset) pairs, descending thresholds
; Out: branch; the list has no end marker, so its last threshold is 0 (inferred)
; Evidence: loop at $A00: skip offset, cmp threshold, bcs keeps scanning; executed in the TAS
; ---------------------------------------------------------------------------
Op2_SwitchRandom:
	jsr	(Rand_Next).l
	cmp.w	(a2)+,d0
	bcc.w	Op2_Goto
loc_000A00:
	addq.l	#$2,a2
	cmp.w	(a2)+,d0
	bcs.s	loc_000A00
	adda.w	(a2),a2
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_ClearPopulation - a2 op: removes the object's population weight: g_ObjPopulation -= $18(a6) (floored at 0) and $18(a6) = 0
; In:  a2 = script (no operand)
; Out: NEXT
; Evidence: same subtraction as Obj_Destroy; executed in the TAS
; ---------------------------------------------------------------------------
Op2_ClearPopulation:
	moveq	#$0,d0
	move.b	$18(a6),d0
	clr.b	$18(a6)
	sub.w	d0,(g_ObjPopulation).w
	bcc.w	VM2_Next
	clr.w	(g_ObjPopulation).w
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_IncPopulation - a2 op: adds 1 to the object's population weight $18(a6) and to g_ObjPopulation
; In:  a2 = script (no operand)
; Out: NEXT
; Evidence: executed in the TAS
; ---------------------------------------------------------------------------
Op2_IncPopulation:
	addq.b	#$1,$18(a6)
	addq.w	#$1,(g_ObjPopulation).w
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_AddPopulation - a2 op: adds a word operand to the object's population weight $18(a6) (byte) and to g_ObjPopulation
; In:  a2 = script: word n
; Out: NEXT
; Evidence: spawners wait on g_ObjPopulation ($437A); executed in the TAS
; ---------------------------------------------------------------------------
Op2_AddPopulation:
	move.w	(a2)+,d0
	add.b	d0,$18(a6)
	add.w	d0,(g_ObjPopulation).w
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_SetScoreValue - a2 op: sets the points the object is worth, $36(a4), from a word operand
; In:  a2 = script: word points
; Out: NEXT
; Evidence: read by Op2_AwardScoreToHitter/Op2_AwardScoreToPlayers and added to the score long at g_Player1/g_Player2; executed in the TAS
; ---------------------------------------------------------------------------
Op2_SetScoreValue:
	move.w	(a2)+,$36(a4)
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_AwardScoreToHitter - a2 op: adds the object's points $36(a4) to the score of the player who hit it (owner id at g_StruckBox+$1A: 1/3 g_Player1, 2/4 g_Player2, other ids nothing), then runs its spawn action
; In:  a2 = script (no operand); g_StruckBox = the box that was hit
; Out: continues in Op2_RunSpawnAction
; Evidence: add.l to +0 of the player record, the long that rises with the TAS score; executed in the TAS
; ---------------------------------------------------------------------------
Op2_AwardScoreToHitter:
	movea.l	(g_StruckBox).w,a0
	move.w	$1A(a0),d0
	lea.l	(g_Player1).w,a0
	cmpi.w	#$1,d0
	beq.s	loc_000A72
	cmpi.w	#$3,d0
	beq.s	loc_000A72
	lea.l	(g_Player2).w,a0
	cmpi.w	#$2,d0
	beq.s	loc_000A72
	cmpi.w	#$4,d0
	bne.w	VM2_Next
loc_000A72:
	move.w	$36(a4),d0
	ext.l	d0
	dc.w	$D1A8,$0000	; insn: add.l d0, $0(a0)
	bra.w	Op2_RunSpawnAction
; ---------------------------------------------------------------------------
; Op2_AwardScoreToPlayers - a2 op: adds the object's points $36(a4) to the score of every player in play (bit 0 of +$25 in g_Player1 / g_Player2), then runs its spawn action
; In:  a2 = script (no operand)
; Out: continues in Op2_RunSpawnAction
; Evidence: executed in the TAS
; ---------------------------------------------------------------------------
Op2_AwardScoreToPlayers:
	move.w	$36(a4),d0
	ext.l	d0
	lea.l	(g_Player1).w,a0
	btst	#$0,$25(a0)
	beq.s	loc_000A96
	dc.w	$D1A8,$0000	; insn: add.l d0, $0(a0)
loc_000A96:
	btst	#$0,$65(a0)
	beq.w	Op2_RunSpawnAction
	add.l	d0,$40(a0)
	bra.s	Op2_RunSpawnAction
; ---------------------------------------------------------------------------
; Op2_SetSpawnAction - a2 op: sets the object's spawn action type $19(a6) (index into Tbl_012EF0, run by Op2_RunSpawnAction) and clears its argument $1A(a6)
; In:  a2 = script: word type
; Out: NEXT; when $19(a6) is already 1 (spawn-group member) it NEXTs without skipping the operand word, as the code reads
; Evidence: Obj_Destroy treats type 1 as a group member (SpawnAction_GroupMemberGone with $1A(a6)); executed in the TAS
; ---------------------------------------------------------------------------
Op2_SetSpawnAction:
	cmpi.b	#$1,$19(a6)
	beq.w	VM2_Next
	move.w	(a2)+,d0
	move.b	d0,$19(a6)
	clr.w	$1A(a6)
	movea.w	(a2)+,a0
	jmp	(a0)
