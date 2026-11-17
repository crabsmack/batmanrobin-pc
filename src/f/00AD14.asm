; ---------------------------------------------------------------------------
; Tint_StepLine3 - steps the line tint and applies it to palette line 3 (equal red, green and blue brightness offsets)
; In:  g_Tint*
; Out: d0 = g_TintLevel (Z set when 0), g_Pal3TintR/G/B = d0; d1 trashed
; Evidence: Tint_Step then three byte stores; the native $01A182 calls it and while the tint is nonzero sets $E000 in the sprite attribute $38(a5) (palette 3, priority); pointers in stage scripts ($01A184, $01C5AE, $025CA2 ...); TAS 11,575 frames from 4540; RAM f4800-f5880 shows line 3 tint 2-$A during a boss fight
; ---------------------------------------------------------------------------
Tint_StepLine3:
	bsr.w	Tint_Step
	move.b	d0,(g_Pal3TintR).w
	move.b	d0,(g_Pal3TintG).w
	move.b	d0,(g_Pal3TintB).w
	rts
