; ---------------------------------------------------------------------------
; Op4_SetResume - a4 op: sets the object's resume pc $8(a6) to the operand address + word offset and continues here
; In:  a4 = script: word offset
; Out: NEXT
; Evidence: VM4_RunObject reloads a4 from $8(a6); script uses $1A3B6, $1A5CC, $1F234 (offset back to a draw loop, then a draw op); 39 corpus runs
; ---------------------------------------------------------------------------
Op4_SetResume:
	movea.l	a4,a0
	adda.w	(a4)+,a0
	move.l	a0,$8(a6)
	movea.w	(a4)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op4_SetResumeHere - a4 op: sets the object's resume pc $8(a6) to the next op (no operand), so later frames restart from here
; In:  a4 = script (no operand)
; Out: NEXT
; Evidence: move.l a4,$8(a6) then NEXT; script uses $15062, $150A0 (followed by the motion op $6286); 61 corpus runs
; ---------------------------------------------------------------------------
Op4_SetResumeHere:
	move.l	a4,$8(a6)
	movea.w	(a4)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op4_YieldGotoAbs - a4 op: ends this frame's run; the object resumes next frame at the absolute address in the operand
; In:  a4 = script: long address
; Out: returns (rts) to the object runner (or to the native caller of the spawn script)
; Evidence: move.l (a4)+,$8(a6) / rts; script uses $1AB70, $25B82 (dc.l $000151B2 / $0001513E); 56 corpus runs
; ---------------------------------------------------------------------------
Op4_YieldGotoAbs:
	move.l	(a4)+,$8(a6)
	rts
; ---------------------------------------------------------------------------
; Op4_YieldGoto - a4 op: ends this frame's run; the object resumes next frame at the operand address + word offset
; In:  a4 = script: word offset
; Out: returns (rts) to the object runner
; Evidence: adda.w (a4),a4 / move.l a4,$8(a6) / rts; script uses $26BA0, $26D0E, $363DC; 37 corpus runs
; ---------------------------------------------------------------------------
Op4_YieldGoto:
	adda.w	(a4),a4
	move.l	a4,$8(a6)
; ---------------------------------------------------------------------------
; Op4_Return - a4 op: plain rts: returns from an Op4_Gosub subroutine, or ends the run of a spawn script / this frame's run of an object without moving its resume pc $8(a6)
; In:  a4 = script (no operand)
; Out: returns to Op4_Gosub, Fx_RunScript or the object runner
; Evidence: label-only entry at the rts of Op4_YieldGoto; ctarget in 151 corpus runs; 188 a4-script uses, e.g. the shot scripts $108D6/$108FC end with it after their spawn op
; ---------------------------------------------------------------------------
Op4_Return:
	rts
; ---------------------------------------------------------------------------
; Op4_Gosub - a4 op: calls a script subroutine at operand address + word offset (pushes the address after the operand, runs it until an op returns, then continues there)
; In:  a4 = script: word offset
; Out: NEXT after the operand once the subroutine returns (Op4_Return)
; Evidence: pea 2(a4) / adda.w (a4),a4 / jsr the first op / movea.l (a7)+,a4; script uses $1AAA6, $1ED1A, $26B0A (after $63A2, before Op4_Kill); 11 corpus runs
; ---------------------------------------------------------------------------
Op4_Gosub:
	pea.l	$2(a4)
	adda.w	(a4),a4
	movea.w	(a4)+,a0
	jsr	(a0)
	movea.l	(a7)+,a4
	movea.w	(a4)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op4_Kill - a4 op: deletes the object: frees its collision box $14(a6) (unlinked) if any, releases its sprite frame art $20(a6), unlinks it from the object list and frees its pool block
; In:  a6 = object; also the target of the off-screen and counter ops
; Out: returns (rts from $8ABE) to the object runner
; Evidence: Hitbox_Unlink + ObjPool_Free on $14(a6), Spr_ReleaseFrame d2 = $20(a6), bra $8ABE (list unlink + free); 63 a4-script uses ($108F2 shot scripts); 151 corpus runs
; ---------------------------------------------------------------------------
Op4_Kill:
	move.w	$14(a6),d0
	beq.s	loc_005416
	movea.w	d0,a0
	bsr.w	Hitbox_Unlink
	bsr.w	ObjPool_Free
loc_005416:
	move.w	$20(a6),d2
	bsr.w	Spr_ReleaseFrame
	bra.w	Obj_UnlinkFree
; ---------------------------------------------------------------------------
; Op4_GotoAbs - a4 op: continues at the absolute script address in the operand
; In:  a4 = script: long address
; Out: NEXT at the target
; Evidence: movea.l (a4),a4 then NEXT; script uses $1ABB2, $1F11E (dc.l $00014C58 / $00014C3C, shared scripts); 28 corpus runs
; ---------------------------------------------------------------------------
Op4_GotoAbs:
	movea.l	(a4),a4
	movea.w	(a4)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op4_RandomSwitch - a4 op: branches to one of n relative offsets picked at random
; In:  a4 = script: word n, then n word offsets (each relative to its own address)
; Out: NEXT at the picked target
; Evidence: Rand_Below(n), a4 += 2*index, then the Op4_Goto code; script uses $20EC4 (n = 4), $278EA (n = 6), $30652 (n = 3); 54 corpus runs
; ---------------------------------------------------------------------------
Op4_RandomSwitch:
	move.w	(a4)+,d0
	bsr.w	Rand_Below
	add.w	d0,d0
	adda.w	d0,a4
; ---------------------------------------------------------------------------
; Op4_Goto - a4 op: relative goto: continues at the operand address + word offset; also the taken path of the a4 branch ops
; In:  a4 = script: word offset
; Out: NEXT at the target
; Evidence: adda.w (a4),a4 then NEXT; 130 a4-script uses (e.g. $132BA, $146CC); branch target of Op4_TimerBranchOut, Op4_CounterBranchOut, Op4_TickerLoop, the box op $66E2; 151 corpus runs
; ---------------------------------------------------------------------------
Op4_Goto:
	adda.w	(a4),a4
	movea.w	(a4)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op4_CallNative - a4 op: calls the 68000 routine at the absolute address in the operand (a6/a5/a4 as in the script; it may move a4) and continues with the next op
; In:  a4 = script: long routine address
; Out: NEXT after the routine returns
; Evidence: movea.l (a4)+,a0 / jsr (a0) / NEXT; 76 a4-script uses: Arc_PlanToMark $926C, Arc_Step $92EA ($1AB82), Shot_DamageTarget (moves a4 to a branch target), $13296 ($1324A); 148 corpus runs
; ---------------------------------------------------------------------------
Op4_CallNative:
	movea.l	(a4)+,a0
	jsr	(a0)
	movea.w	(a4)+,a0
	jmp	(a0)
