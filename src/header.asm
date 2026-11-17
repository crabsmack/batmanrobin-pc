; Vector table
Vectors:
	dc.l	$00000000	;  0 initial SSP
	dc.l	Boot_Reset	;  1 reset
	dc.l	$0240800C	;  2 bus error
	dc.l	$0340800C	;  3 address error
	dc.l	$0440800C	;  4 illegal instruction
	dc.l	$0540800C	;  5 zero divide
	dc.l	$0640800C	;  6 CHK
	dc.l	$0740800C	;  7 TRAPV
	dc.l	$0840800C	;  8 privilege
	dc.l	$0940800C	;  9 trace
	dc.l	$08007734	; 10 line A
	dc.l	$09007734	; 11 line F
	dc.l	$0A007734	; 12
	dc.l	$0A007734	; 13
	dc.l	$0A007734	; 14
	dc.l	$0A007734	; 15
	dc.l	$0A007734	; 16
	dc.l	$0A007734	; 17
	dc.l	$0A007734	; 18
	dc.l	$0A007734	; 19
	dc.l	$0A007734	; 20
	dc.l	$0A007734	; 21
	dc.l	$0A007734	; 22
	dc.l	$0A007734	; 23
	dc.l	$0B007734	; 24 spurious
	dc.l	$0C007734	; 25 IRQ1
	dc.l	$0D007734	; 26 IRQ2 (ext)
	dc.l	$0E007734	; 27 IRQ3
	dc.l	$FFFFE560	; 28 IRQ4 / H-int
	dc.l	$10007734	; 29 IRQ5
	dc.l	VInt_Handler	; 30 IRQ6 / V-int
	dc.l	$12007734	; 31 IRQ7
	dc.l	$00408008	; 32 TRAP #0
	dc.l	$14007734	; 33 TRAP #1
	dc.l	$15007734	; 34 TRAP #2
	dc.l	$16007734	; 35 TRAP #3
	dc.l	$17007734	; 36 TRAP #4
	dc.l	$18007734	; 37 TRAP #5
	dc.l	$19007734	; 38 TRAP #6
	dc.l	$1A007734	; 39 TRAP #7
	dc.l	$1B007734	; 40 TRAP #8
	dc.l	$1C007734	; 41 TRAP #9
	dc.l	$1D007734	; 42 TRAP #10
	dc.l	$1E007734	; 43 TRAP #11
	dc.l	$1F007734	; 44 TRAP #12
	dc.l	$20007734	; 45 TRAP #13
	dc.l	$21007734	; 46 TRAP #14
	dc.l	$22007734	; 47 TRAP #15
	dc.l	$0A007734	; 48
	dc.l	$0A007734	; 49
	dc.l	$0A007734	; 50
	dc.l	$0A007734	; 51
	dc.l	$0A007734	; 52
	dc.l	$0A007734	; 53
	dc.l	$0A007734	; 54
	dc.l	$0A007734	; 55
	dc.l	$0A007734	; 56
	dc.l	$0A007734	; 57
	dc.l	$0A007734	; 58
	dc.l	$0A007734	; 59
	dc.l	$0A007734	; 60
	dc.l	$0A007734	; 61
	dc.l	$0A007734	; 62
	dc.l	$0A007734	; 63

; ROM header
Header:
	dc.b	"SEGA GENESIS    "	; system type
	dc.b	"(C)SEGA 1995.APR"	; copyright
	dc.b	"THE ADVENTURES OF BATMAN AND ROBIN              "	; domestic name
	dc.b	"THE ADVENTURES OF BATMAN AND ROBIN              "	; overseas name
	dc.b	"GM MK-1535 -00"	; serial / version
	dc.w	$C661	; checksum
	dc.b	"J               "	; I/O support
	dc.l	$00000000,$001FFFFF	; ROM start/end
	dc.l	$00FF0000,$00FFFFFF	; RAM start/end
	dc.b	"            "	; SRAM info
	dc.b	"            "	; modem info
	dc.b	"                                        "	; notes
Hdr_Regions:	; $0001F0: the header's region string, scanned by the boot region check (Boot_CheckRegion)
	dc.b	"JU              "	; regions
