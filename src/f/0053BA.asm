; ---------------------------------------------------------------------------
; VM4_Next - a4-VM NEXT: fetches the next op word from a4 and jumps to that handler; also the entry for native code that has set a4 and a6 itself
; In:  a4 = script, a6 = object (or g_SpawnCtx), a5 = child or a6
; Out: never returns here: the handlers chain until an op returns (rts)
; Evidence: movea.w (a4)+,a0 / jmp (a0), the tail of every a4 handler; bsr/jsr from Fx_RunScript ($91F2), Fx_RunScriptNoParent, $915C, Weapon_Fire ($10A7E); 99,175 TAS frames
; ---------------------------------------------------------------------------
VM4_Next:
	movea.w	(a4)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op4_SkipWord - a4 VM: skips one script word (a branch offset not taken) and continues with NEXT
; In:  a4 = script: the word to skip
; Out: NEXT after the word
; Evidence: addq.l #2,a4 then NEXT; branch-not-taken target of Op4_TimerLoop and the op at $607C (no script uses it as an op)
; ---------------------------------------------------------------------------
Op4_SkipWord:
	addq.l	#$2,a4
	movea.w	(a4)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op4_Native - a4 op: leaves the script and runs the 68000 code that follows the op word (jmp (a4)); that code re-enters script with jsr VM4_Enter
; In:  a4 = address of the native code right after the op word
; Out: whatever the fragment does (it ends with jsr $53C6.w or a jump to an op such as Op4_Kill)
; Evidence: script uses at $3D8FE / $3D912 followed by 68000 code; $3D914 ends in jsr ($53C6).w or jmp Op4_Kill; 4 corpus runs incl. the TAS
; ---------------------------------------------------------------------------
Op4_Native:
	jmp	(a4)
; ---------------------------------------------------------------------------
; VM4_Enter - enters a4 script from native code: pops the return address of jsr $53C6.w into a4 and dispatches, so the script is the bytes after the jsr
; In:  (sp) = return address = script; a6/a5 = the running object
; Out: NEXT; never returns to the jsr
; Evidence: 2 jsr ($53C6).w sites, $3D92E and $42922, each ending a native fragment entered by Op4_Native; dkit.toml [disasm] noreturn
; ---------------------------------------------------------------------------
VM4_Enter:
	movea.l	(a7)+,a4
	movea.w	(a4)+,a0
	jmp	(a0)
