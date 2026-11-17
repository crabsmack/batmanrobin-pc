; ---------------------------------------------------------------------------
; VM2_RunObject - object handler that runs one frame of an object's a2 script: saves sp in g_VM2SavedSP, loads a3/a4/a5 from the object's block handles $8/$A/$C(a6) and resumes the script at $2A(a4); a yield op restores sp and returns
; In:  a6 = object (main or V-int list entry)
; Out: d0 = 0 after a yield (Op2_Yield/Op2_YieldGoto saved the resume point in $2A(a4)), d0 = -1 after the object deleted itself (Op2_Kill); a3-a5 = its blocks
; Evidence: installed as an object's handler by $1E70A (lea VM2_RunObject,a0 / jsr $8F0E); called directly by $102D6, $290B6, $29572 and four times by $2604C (which saves/restores g_VM2SavedSP around the nested runs); its tail VM2_Next at $7E0 (movea.w (a2)+,a0 / jmp (a0)) is the VM's NEXT, the target of 46 op branches; runs on 129k TAS frames
; ---------------------------------------------------------------------------
VM2_RunObject:
	move.l	a7,(g_VM2SavedSP).w
	movem.w	$8(a6),a3-a5
	movea.l	$2A(a4),a2
VM2_Next:
	movea.w	(a2)+,a0
	jmp	(a0)
