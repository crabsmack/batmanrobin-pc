; ---------------------------------------------------------------------------
; Mot_Atan2 - approximate atan2: the heading (0-$3FF, 1024 per turn) of the vector (d0, d1), linear in the ratio inside each octant
; In:  d0 = dx, d1 = dy (screen y down)
; Out: d2 = heading: 0 = +x, $100 = +y (down), $200 = -x, $300 = -y; (0, 0) gives 0
; Evidence: axis cases return 0/$100/$200/$300; octants add or subtract (minor*$80)/major from them; callers the aim ops via Mot_SetTarget ($1364/$1382/$13A4), Op2_SetHeading's neighbours, $29534, $334D2, $37708; TAS f32291-f128550, 15758 frames
; ---------------------------------------------------------------------------
Mot_Atan2:
	tst.w	d0
	bmi.s	Mot_Atan2_LeftHalf
	bne.s	Mot_Atan2_RightHalf
	tst.w	d1
	bmi.s	Mot_Atan2_Up
	bne.s	Mot_Atan2_Down
; ---------------------------------------------------------------------------
; Mot_Atan2_Zero - the (0, 0) and dx > 0, dy = 0 exit of Mot_Atan2: heading 0 (+x)
; In:  none
; Out: d2 = 0 (moveq)
; Evidence: moveq #0,d2 / rts; reached from Mot_Atan2 (dx = dy = 0) and by beq from Mot_Atan2_RightHalf (dy = 0)
; ---------------------------------------------------------------------------
Mot_Atan2_Zero:
	moveq	#$0,d2
	rts
; ---------------------------------------------------------------------------
; Mot_Atan2_Down - the dx = 0, dy > 0 exit of Mot_Atan2: heading $100 (straight down)
; In:  none
; Out: d2 low word = $100
; Evidence: move.w #$100,d2 / rts after tst.w d1 / bne in Mot_Atan2
; ---------------------------------------------------------------------------
Mot_Atan2_Down:
	move.w	#$100,d2
	rts
; ---------------------------------------------------------------------------
; Mot_Atan2_Up - the dx = 0, dy < 0 exit of Mot_Atan2: heading $300 (straight up)
; In:  none
; Out: d2 low word = $300
; Evidence: move.w #$300,d2 / rts after tst.w d1 / bmi in Mot_Atan2
; ---------------------------------------------------------------------------
Mot_Atan2_Up:
	move.w	#$300,d2
	rts
