; ---------------------------------------------------------------------------
; TextMap_CmdBlinkGoto - text-map command $108: goes to one of two offsets by the 16-frame blink phase (bit 4 of the frame counter)
; In:  a0 = operands: word offset (phase off), word offset (phase on)
; Out: list continues at the chosen offset
; Evidence: btst #4,$E031 then TextMap_CmdGoto on the first or second offset; computed entry in scenario-continue
; ---------------------------------------------------------------------------
TextMap_CmdBlinkGoto:
	btst	#$4,(g_FrameCounter+3).w
	beq.w	TextMap_CmdGoto
	addq.l	#$2,a0
	bra.w	TextMap_CmdGoto
; ---------------------------------------------------------------------------
; TextMap_CmdTypewriter - text-map command $118: switches the rest of the list to typewriter mode, one character each time the speed accumulator carries
; In:  a0 = operand: word speed (added per call, carry = one character)
; Out: g_TextMapTyping set, g_TextMapTypeSpeed = g_TextMapTypeAcc = speed
; Evidence: st.b $F4EE; move.w d0,$F3BC / $F3BA; read by TextMap_ResumeTyping; computed entry in scenario-ending
; ---------------------------------------------------------------------------
TextMap_CmdTypewriter:
	st.b	(g_TextMapTyping).w
	move.w	(a0)+,d0
	move.w	d0,(g_TextMapTypeSpeed).w
	move.w	d0,(g_TextMapTypeAcc).w
	bra.w	TextMap_NextCmd
