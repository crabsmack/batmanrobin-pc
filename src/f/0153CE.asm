; ---------------------------------------------------------------------------
; Rot3D_ProjectPoint - transforms a 3D point by the rotation matrix and translation of a 3D record, then divides x and y by the depth (perspective)
; In:  a0 = 3D record (+$C/+$10/+$14 translation x/y/z longs, +$18-$28 3x3 matrix words built by Rot3D_BuildMatrix), a1 = point: words x, y, z
; Out: d4/d5 = projected x/y (x'/8 / (z'/4096)), d7 = depth z'/4096 (1 when 0, then no divide); a1 past the point
; Evidence: muls of each coordinate with the matrix columns $18/$1E/$24, $1A/$20/$26, $1C/$22/$28, zero coordinates skipped; asr.l #3 / divs.w d7; callers $1EB2C/$1EB86 with the record $F3C4 (they add the object's x/y and test depth $700-$800), $471C0; TAS f10512-f118790
; ---------------------------------------------------------------------------
Rot3D_ProjectPoint:
	move.l	$14(a0),d7
	move.l	$C(a0),d4
	move.l	$10(a0),d5
	move.w	(a1)+,d0
	beq.s	loc_0153F4
	move.w	$18(a0),d1
	muls.w	d0,d1
	add.l	d1,d4
	move.w	$1E(a0),d1
	muls.w	d0,d1
	add.l	d1,d5
	muls.w	$24(a0),d0
	add.l	d0,d7
loc_0153F4:
	move.w	(a1)+,d0
	beq.s	loc_01540E
	move.w	$1A(a0),d1
	muls.w	d0,d1
	add.l	d1,d4
	move.w	$20(a0),d1
	muls.w	d0,d1
	add.l	d1,d5
	muls.w	$26(a0),d0
	add.l	d0,d7
loc_01540E:
	move.w	(a1)+,d0
	beq.s	loc_015428
	move.w	$1C(a0),d1
	muls.w	d0,d1
	add.l	d1,d4
	move.w	$22(a0),d1
	muls.w	d0,d1
	add.l	d1,d5
	muls.w	$28(a0),d0
	add.l	d0,d7
loc_015428:
	asr.l	#$3,d4
	asr.l	#$3,d5
	asl.l	#$4,d7
	swap	d7
	tst.w	d7
	beq.s	loc_01543A
	divs.w	d7,d4
	divs.w	d7,d5
	rts
loc_01543A:
	moveq	#$1,d7
	rts
; ---------------------------------------------------------------------------
; Rot3D_TransformPoint - rotates and translates a 3D point by a 3D record without perspective, keeping 1.15 fixed point
; In:  a0 = 3D record (as Rot3D_ProjectPoint), a1 = point: words x, y, z
; Out: d4/d5/d7 = transformed x/y/z (sum*2 >> 16); a1 past the point
; Evidence: same products as Rot3D_ProjectPoint, then add.l / swap instead of the divide; caller $1EB06 stores 3 transformed points (+$10E on y) with a dc.l each; $471C0
; ---------------------------------------------------------------------------
Rot3D_TransformPoint:
	move.l	$14(a0),d7
	move.l	$C(a0),d4
	move.l	$10(a0),d5
	move.w	(a1)+,d0
	beq.s	loc_015464
	move.w	$18(a0),d1
	muls.w	d0,d1
	add.l	d1,d4
	move.w	$1E(a0),d1
	muls.w	d0,d1
	add.l	d1,d5
	muls.w	$24(a0),d0
	add.l	d0,d7
loc_015464:
	move.w	(a1)+,d0
	beq.s	loc_01547E
	move.w	$1A(a0),d1
	muls.w	d0,d1
	add.l	d1,d4
	move.w	$20(a0),d1
	muls.w	d0,d1
	add.l	d1,d5
	muls.w	$26(a0),d0
	add.l	d0,d7
loc_01547E:
	move.w	(a1)+,d0
	beq.s	loc_015498
	move.w	$1C(a0),d1
	muls.w	d0,d1
	add.l	d1,d4
	move.w	$22(a0),d1
	muls.w	d0,d1
	add.l	d1,d5
	muls.w	$28(a0),d0
	add.l	d0,d7
loc_015498:
	add.l	d4,d4
	swap	d4
	add.l	d5,d5
	swap	d5
	add.l	d7,d7
	swap	d7
	rts
; ---------------------------------------------------------------------------
; Rot3D_BuildMatrix - builds the 3x3 rotation matrix of a 3D record from its three angles with the sine table
; In:  a0 = 3D record: angle words at +0, +4, +8 (0-$3FF, 1024 per turn, masked & $3FE)
; Out: +$18..+$28 = the matrix words (1.15), rows x ($18/$1A/$1C), y ($1E/$20/$22), z ($24/$26/$28)
; Evidence: sine Tbl_059B22 and cosine at +$100, sum/difference products (sin(a+b) +- sin(a-b))/2; caller $1EB06 before Rot3D_TransformPoint, $1E9A4, $471C0; computed entry in 7 fuzz runs
; ---------------------------------------------------------------------------
Rot3D_BuildMatrix:
	lea.l	(Tbl_059B22).l,a3
	lea.l	$100(a3),a4
	move.w	#$3FE,d7
	move.w	(a0),d4
	move.w	$4(a0),d5
	move.w	$8(a0),d6
	and.w	d7,d4
	and.w	d7,d5
	and.w	d7,d6
	move.w	(a3,d5.w),$1E(a0)
	move.w	d4,d0
	add.w	d5,d0
	and.w	d7,d0
	move.w	(a3,d0.w),d1
	ext.l	d1
	movea.w	(a4,d0.w),a2
	move.w	d4,d0
	sub.w	d5,d0
	and.w	d7,d0
	movea.w	(a3,d0.w),a1
	move.w	(a4,d0.w),d2
	ext.l	d2
	move.l	d2,d0
	add.l	a2,d0
	asr.l	#$1,d0
	move.w	d0,$18(a0)
	sub.l	a2,d2
	asr.l	#$1,d2
	move.l	d1,d0
	add.l	a1,d0
	asr.l	#$1,d0
	neg.w	d0
	move.w	d0,$24(a0)
	sub.l	a1,d1
	asr.l	#$1,d1
	move.w	d5,d0
	add.w	d6,d0
	and.w	d7,d0
	move.w	(a3,d0.w),d3
	ext.l	d3
	move.w	(a4,d0.w),d0
	ext.l	d0
	sub.w	d6,d5
	and.w	d7,d5
	movea.w	(a3,d5.w),a1
	movea.w	(a4,d5.w),a2
	add.l	a2,d0
	asr.l	#$1,d0
	move.w	d0,$20(a0)
	sub.l	a1,d3
	asr.l	#$1,d3
	neg.w	d3
	move.w	d3,$22(a0)
	move.w	d4,d0
	add.w	d6,d0
	and.w	d7,d0
	move.w	(a3,d0.w),d3
	ext.l	d3
	movea.w	(a4,d0.w),a2
	sub.w	d6,d4
	and.w	d7,d4
	movea.w	(a3,d4.w),a1
	move.w	(a4,d4.w),d4
	ext.l	d4
	move.l	d4,d0
	add.l	a2,d0
	asr.l	#$1,d0
	sub.l	a2,d4
	asr.l	#$1,d4
	move.l	d3,d5
	add.l	a1,d5
	asr.l	#$1,d5
	sub.l	a1,d3
	asr.l	#$1,d3
	move.w	(a4,d6.w),d7
	muls.w	d2,d7
	add.l	d7,d7
	swap	d7
	add.w	d7,d3
	move.w	d3,$26(a0)
	move.w	(a4,d6.w),d7
	muls.w	d1,d7
	add.l	d7,d7
	swap	d7
	sub.w	d7,d4
	move.w	d4,$1A(a0)
	muls.w	(a3,d6.w),d2
	add.l	d2,d2
	swap	d2
	sub.w	d2,d0
	move.w	d0,$28(a0)
	muls.w	(a3,d6.w),d1
	add.l	d1,d1
	swap	d1
	add.w	d1,d5
	move.w	d5,$1C(a0)
	rts
